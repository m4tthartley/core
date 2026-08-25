/*
	Created by Matt Hartley on 24/04/2026.
	Copyright 2026 GiantJelly. All rights reserved.
*/

#ifndef __CORE_JSON_HEADER__
#define __CORE_JSON_HEADER__

#include "core.h"
#include "core/sys.h"

#ifndef JSON_ALLOC
#	ifndef CORE_ALLOC
#		define JSON_ALLOC _json_alloc_memory
// #		warning "No alloc function set for CORE_JSON, using malloc()"
#	else
#		define JSON_ALLOC CORE_ALLOC
#	endif
#endif
#ifndef JSON_FREE
#	ifndef CORE_FREE
#		define JSON_FREE _json_free_memory
#	else
#		define JSON_FREE CORE_FREE
#	endif
#endif


// typedef struct json_obj_t json_obj_t;
typedef struct json_value_t json_value_t;

typedef struct json_value_t {
	uint8_t type;
	union {
		int32_t i;
		float f;
		char* s;
		// json_obj_t* obj;
		// json_obj_t* array;
	};

	// New linked list objects/arrays
	char* key;
	int arrayLength;
	// json_value_t* object;
	json_value_t* first;
	json_value_t* next;
	// Next is for adjacent items
	// first is for an object/arrays first child
	// Since this lib is mostly just for parsing stupid formats that use json,
	// not doing much in json generally. So being able to remove values from
	// objects and array is not important.
	// So I can do very simple linked list loops.
} json_value_t;


json_value_t* Json_Parse(void* data, uint32_t length);
void Json_PrintValue(json_value_t* value);


#define CORE_IMPL 
#ifdef CORE_IMPL


typedef struct {
	char* data;
	char* cursor;
	uint32_t length;
} json_parser_t;

typedef enum {
	JSON_NULL,
	JSON_INT,
	JSON_FLOAT,
	JSON_STRING,
	JSON_BOOLEAN,
	JSON_OBJECT,
	JSON_ARRAY,
} json_type_t;


// TODO: Try using linked list
// typedef struct json_obj_t {
// 	struct {
// 		char* key;
// 		json_value_t value;
// 	} items[64]; // TODO: Change this stuff to a more dynamic system
// 	uint32_t length;
// } json_obj_t;

typedef enum {
	JSON_TOKEN_STRING,
	JSON_TOKEN_NUMBER,
	JSON_TOKEN_BOOLEAN,
} json_token_type_t;

typedef struct {
	uint8_t type;
	char* str;
	bool b;
} json_token_t;

json_value_t* _Json_ParseValue(json_parser_t* parser);
void _Json_PrintValueWithKey(char* key, json_value_t* value);

void* _json_alloc_memory(size_t size)
{
	void* p = malloc(size);
	return p;
}

void _json_free_memory(void* p)
{
	free(p);
}

json_value_t* json_alloc_value()
{
	json_value_t* value = JSON_ALLOC(sizeof(json_value_t));
	sys_zero_memory(value, sizeof(json_value_t));
	return value;
}

char* _Json_GetValueString(json_value_t value)
{
	switch (value.type) {
		case JSON_INT:
			return strformat("%i", value.i);
			break;
		case JSON_FLOAT:
			return strformat("%f", value.f);
			break;
		case JSON_STRING:
			return strformat("%s", value.s);
			break;
		case JSON_BOOLEAN:
			return strformat("%s", value.i ? "true" : "false");
			break;
		case JSON_OBJECT:
			return strformat("{object}");
			break;
		case JSON_ARRAY:
			return strformat("[array]");
			break;
		default:
			return "Unknown Value";
			break;
	}
}

_Bool _Json_PeakToken(json_parser_t* parser, json_token_t* token)
{
	*token = (json_token_t){0};

	while ((*parser->cursor==' ' || *parser->cursor=='\t' || *parser->cursor=='\n')
		&& parser->cursor < parser->data+parser->length) {
		++parser->cursor;
	}

	if (parser->cursor >= parser->data+parser->length) {
		return _False;
	}

	switch (*parser->cursor) {
		case '"': {
			// ++parser->cursor;
			char* c = parser->cursor + 1;
			while (*c != '"') ++c;
			int len = c - parser->cursor;
			char* str = malloc(len);
			sprint(str, len, parser->cursor + 1);
			// print("string token: \"%s\" \n", str);
			// parser->cursor = c;
			*token = (json_token_t){JSON_TOKEN_STRING, str};
		} break;
		case '{': {
			*token = (json_token_t){*parser->cursor};
		} break;
		case '}': {
			*token = (json_token_t){*parser->cursor};
		} break;
		case '[': {
			*token = (json_token_t){*parser->cursor};
		} break;
		case ']': {
			*token = (json_token_t){*parser->cursor};
		} break;
		case ':': {
			*token = (json_token_t){*parser->cursor};
		} break;
		case ',': {
			*token = (json_token_t){*parser->cursor};
		} break;

		default: {
			if (*parser->cursor=='-' ||
				(*parser->cursor >= '0' && *parser->cursor <= '9')) {

				char* c = parser->cursor + 1;
				while ((*c >= '0' && *c <= '9')
					|| *c=='.') {
					++c;
				}
				int len = c - parser->cursor + 1;
				char* str = malloc(len);
				sprint(str, len, parser->cursor);
				// parser->cursor = c;
				*token = (json_token_t){JSON_TOKEN_NUMBER, str};
				return _True;
			}

			if (strncompare(parser->cursor, "true", 4)) {
				char* str = malloc(4+1);
				sprint(str, 4+1, "true");
				*token = (json_token_t){JSON_TOKEN_BOOLEAN, .str=str, .b=TRUE};
				return _True;
			}
			if (strncompare(parser->cursor, "false", 5)) {
				char* str = malloc(5+1);
				sprint(str, 5+1, "false");
				*token = (json_token_t){JSON_TOKEN_BOOLEAN, .str=str, .b=FALSE};
				return _True;
			}

			*token = (json_token_t){*parser->cursor};
		} break;
	}

	// ++parser->cursor;
	return _True;
}

_Bool _Json_ReadToken(json_parser_t* parser, json_token_t* token)
{
	_Bool result = _Json_PeakToken(parser, token);
	int len = 1;
	if (token->str) {
		len = strsize(token->str);
		if (token->type == JSON_TOKEN_STRING) {
			len += 2;
		}
	}

	parser->cursor += len;

	return result;
}

json_token_t _Json_ExpectReadToken(json_parser_t* parser, uint8_t type)
{
	json_token_t token;
	_Json_ReadToken(parser, &token);
	if (token.type != type) {
		print_err("ExpectReadToken: Unexpected token: %c %s \n", token.type, token.str ? token.str : "");
		exit(1);
	}

	return token;
}

_Bool _Json_ExpectToken(json_token_t token, uint8_t type)
{
	if (token.type != type) {
		print_err("ExpectToken: Unexpected token: %c %s \n", token.type, token.str ? token.str : "");
		exit(1);
	}

	return _True;
}

void _json_value_add_child(json_value_t* value, json_value_t* child)
{
	json_value_t* node = value->first;
	if (node) {
		while (node) {
			if (!node->next) {
				node->next = child;
				break;
			}
			node = node->next;
		}
	} else {
		value->first = child;
	}
}

json_value_t* _Json_ParseObject(json_parser_t* parser)
{
	json_value_t* obj = json_alloc_value();
	obj->type = JSON_OBJECT;

	json_token_t token;
	while (_Json_ReadToken(parser, &token) && token.type == JSON_TOKEN_STRING) {
		_Json_ExpectReadToken(parser, ':');
		json_value_t* value = _Json_ParseValue(parser);
		// print("key value: %s: %s \n", token.str, GetValueString(value));

		// assert(obj.length < array_size(obj.items));
		// obj.items[obj.length].key = token.str;
		// obj.items[obj.length].value = value;
		// ++obj.length;

		value->key = token.str;
		// value->next = obj->first;
		// obj->first = value;
		_json_value_add_child(obj, value);

		_Json_ReadToken(parser, &token);
		if (token.type != ',') {
			_Json_ExpectToken(token, '}');
			break;
		}
	}

	// json_value_t* result = JSON_ALLOC(sizeof(json_value_t));
	// *result = obj;
	// return result;
	return obj;
}

json_value_t* _Json_ParseArray(json_parser_t* parser)
{
	json_value_t* obj = json_alloc_value();
	obj->type = JSON_ARRAY;

	// json_token_t token;
	json_value_t* value;
	while ((value = _Json_ParseValue(parser)), _True) {
		// json_value_t value = ParseValue(parser);
		// print("array value: %s \n", GetValueString(value));

		// assert(obj.length < array_size(obj.items));
		// obj.items[obj.length].value = value;
		// ++obj.length;

		// value->next = obj->first;
		// obj->first = value;
		_json_value_add_child(obj, value);

		++obj->arrayLength;

		json_token_t token;
		_Json_ReadToken(parser, &token);
		if (token.type != ',') {
			_Json_ExpectToken(token, ']');
			break;
		}
	}

	return obj;
}

json_value_t* _Json_ParseValue(json_parser_t* parser)
{
	json_token_t token;
	_Json_ReadToken(parser, &token);

	switch (token.type) {
		case JSON_TOKEN_STRING: {
			json_value_t* value = json_alloc_value();
			value->type = JSON_STRING;
			value->s = token.str;
			return value;
			// int len = strsize(value.s);
			// value.s[len-1] = 0;
			// ++value.s;
			// return value;
		} break;
		case JSON_TOKEN_NUMBER:
			if (strfind(token.str, ".")) {
				float num = atof(token.str);
				json_value_t* value = json_alloc_value();
				value->type = JSON_FLOAT;
				value->f = num;
				return value;
			} else {
				int num = atoi(token.str);
				json_value_t* value = json_alloc_value();
				value->type = JSON_INT;
				value->i = num;
				return value;
			}
			break;
		case JSON_TOKEN_BOOLEAN: {
			json_value_t* value = json_alloc_value();
			value->type = JSON_BOOLEAN;
			value->i = token.b;
			return value;
		} break;
		case '{': {
			// parse object
			return _Json_ParseObject(parser);
			// json_value_t* objp = JSON_ALLOC(sizeof(obj));
			// *objp = obj;
			// value.type = JSON_OBJECT;
			// value.obj = objp;
		} break;
		case '[': {
			// parse array
			return _Json_ParseArray(parser);
			// json_obj_t* objp = JSON_ALLOC(sizeof(obj));
			// *objp = obj;
			// return (json_value_t){
			// 	.type = JSON_ARRAY,
			// 	.obj = objp,
			// };
		} break;
		default:
			// error 
			print_err("ParseValue: Unexpected token: %i, %c, \"%s\" \n", (int)token.type, token.type, token.str);
			exit(1);
	}

	// json_value_t* result = JSON_ALLOC(sizeof(json_value_t));
	// *result = value;
	// return result;
}

json_value_t* Json_Parse(void* data, uint32_t length)
{
	json_parser_t parser = {
		.data = (char*)data,
		.cursor = (char*)data,
		.length = length,
	};

	json_value_t* value = _Json_ParseValue(&parser);

	return value;
}

int indent = 0;
void _Json_PrintValueWithKey(char* key, json_value_t* value)
{
	// char indentStr[16] = {0};
	// for (int i=0; i<min(indent, 15); ++i) indentStr[i] = ' ';
	// print(indentStr);
	char* indentToken = "\t";
	for (int i=0; i<indent; ++i) print(indentToken);

	if (key) {
		print("\"%s\": ", key);
	}

	switch (value->type) {
		case JSON_STRING:
			print("\"%s\"", value->s);
			break;
		case JSON_INT:
			print("%i", value->i);
			break;
		case JSON_FLOAT:
			print("%f", value->f);
			break;
		case JSON_BOOLEAN:
			print("%s", value->i ? "true" : "false");
			break;
		case JSON_OBJECT: {
			// json_obj_t obj = *(json_obj_t*)value.obj;
			print("{ \n");
			++indent;
			// for (int i=0; i<obj.length; ++i) {
			json_value_t* v = value->first;
			while (v) {
				// print("\"%s\": ", obj.items[i].key);
				// print("%s: ", obj.items[i].key);
				_Json_PrintValueWithKey(v->key, v);
				v = v->next;
				print(v ? ",\n" : "\n");
			}
			--indent;
			for (int i=0; i<indent; ++i) print(indentToken);
			print("}");
			// PrintJsonValue()
		} break;
		case JSON_ARRAY: {
			// json_obj_t obj = *(json_obj_t*)value.obj;
			print("[ \n");
			++indent;
			// for (int i=0; i<obj.length; ++i) {
			json_value_t* v = value->first;
			while (v) {
				// print("\"%s\": ", obj.items[i].key);
				Json_PrintValue(v);
				v = v->next;
				print(v ? ",\n" : "\n");
			}
			--indent;
			for (int i=0; i<indent; ++i) print(indentToken);
			print("]");
			// PrintJsonValue()
		} break;
	}
}
void Json_PrintValue(json_value_t* value)
{
	_Json_PrintValueWithKey(NULL, value);
}

void _Json_WriteToFile(file_t file, char* str)
{
	int size = strsize(str);
	sys_write_seq(file, str, size);
}

void _Json_OutputValueToFileHandle(file_t file, char* key, json_value_t* value)
{
	// char indentStr[16] = {0};
	// for (int i=0; i<min(indent, 15); ++i) indentStr[i] = ' ';
	// print(indentStr);
	char* indentToken = "\t";
	for (int i=0; i<indent; ++i) _Json_WriteToFile(file, strformat(indentToken));

	if (key) {
		_Json_WriteToFile(file, strformat("\"%s\": ", key));
	}

	switch (value->type) {
		case JSON_STRING:
			_Json_WriteToFile(file, strformat("\"%s\"", value->s));
			break;
		case JSON_INT:
			_Json_WriteToFile(file, strformat("%i", value->i));
			break;
		case JSON_FLOAT:
			_Json_WriteToFile(file, strformat("%f", value->f));
			break;
		case JSON_BOOLEAN:
			_Json_WriteToFile(file, strformat("%s", value->i ? "true" : "false"));
			break;
		case JSON_OBJECT: {
			// json_obj_t obj = *(json_obj_t*)value.obj;
			_Json_WriteToFile(file, strformat("{ \n"));
			++indent;
			// for (int i=0; i<obj.length; ++i) {
			json_value_t* v = value->first;
			while (v) {
				// WriteToFile(file, strformat("\"%s\": ", obj.items[i].key));
				// WriteToFile(file, strformat("%s: ", obj.items[i].key));
				_Json_OutputValueToFileHandle(file, v->key, v);
				v = v->next;
				_Json_WriteToFile(file, strformat(v ? ",\n" : "\n"));
			}
			--indent;
			for (int i=0; i<indent; ++i) _Json_WriteToFile(file, strformat(indentToken));
			_Json_WriteToFile(file, strformat("}"));
			// PrintJsonValue()
		} break;
		case JSON_ARRAY: {
			// json_obj_t obj = *(json_obj_t*)value.obj;
			_Json_WriteToFile(file, strformat("[ \n"));
			++indent;
			// for (int i=0; i<obj.length; ++i) {
			json_value_t* v = value->first;
			while (v) {
				// WriteToFile(file, strformat("\"%s\": ", obj.items[i].key));
				_Json_OutputValueToFileHandle(file, NULL, v);
				v = v->next;
				_Json_WriteToFile(file, strformat(v ? ",\n" : "\n"));
			}
			--indent;
			for (int i=0; i<indent; ++i) _Json_WriteToFile(file, strformat(indentToken));
			_Json_WriteToFile(file, strformat("]"));
		} break;
	}
}
void _Json_OutputValueToFile(char* filename, json_value_t* value)
{
	print(filename);
	char* parts[8];
	int num = strsplit(parts, 8, filename, "/");

	char* outputFilename = strformat("./build/%s.txt", parts[num-1]);

	file_t file = sys_create(outputFilename);
	_Json_OutputValueToFileHandle(file, NULL, value);
	sys_close(file);
}

// JSON_INT,
// JSON_FLOAT,
// JSON_STRING,
// JSON_BOOLEAN,
// JSON_OBJECT,
// JSON_ARRAY,

json_value_t* Json_GetValueByIndex(json_value_t* value, int index)
{
	// assert(index < array_size(value.array->items));
	// json_value_t v = value.obj->items[index].value;
	// return v;

	int i = 0;
	json_value_t* v = value->first;
	while (v) {
		if (i == index) {
			return v;
		}
		++i;
		v = v->next;
	}

	return NULL;
}

json_value_t* Json_GetValueByKey(json_value_t* value, char* key)
{
	json_value_t* v = value->first;
	while (v) {
		if (strcompare(v->key, key)) {
			return v;
		}
		v = v->next;
	}

	print_err("JSON value not found: %s \n", key);
	// exit(1);
	return NULL;
}

// json_value_t Json_GetValueArgs(json_value_t value, ...)
// {
// 	va_list args;
// 	va_start(args, value);

// 	switch(value.type) {
// 		case JSON_OBJECT: {
// 			char* key = va_arg(args, char*);
// 			return Json_GetValueByKey(value, key);
// 		} break;
// 		case JSON_ARRAY: {
// 			int index = va_arg(args, int);
// 			return Json_GetValueByIndex(value, index);
// 		} break;
// 		default:
// 			print_err("JSON value is not an object or an array \n");
// 			// exit(1);
// 			return (json_value_t){0};
// 			break;
// 	}

// 	va_end(args);
// }

// JSON_INT,
// JSON_FLOAT,
// JSON_STRING,
// JSON_BOOLEAN,
// JSON_OBJECT,
// JSON_ARRAY,
#define _JSON_TYPE_GEN()\
	_JSON_TYPE(Int, int, result.i)\
	_JSON_TYPE(Float, float, result.f)\
	_JSON_TYPE(Str, char*, result.s)\
	_JSON_TYPE(Bool, _Bool, result.i)\
	_JSON_TYPE(Obj, json_value_t, result)\
	_JSON_TYPE(Array, json_value_t, result)\

// #define _JSON_TYPE(funcSuffix, valueType, resultName)\
// valueType JsonGet##funcSuffix(json_value_t value, ...)\
// {\
// 	va_list args;\
// 	va_start(args, value);\
// 	json_value_t result = {0};\
// \
// 	switch(value.type) {\
// 		case JSON_OBJECT: {\
// 			char* key = va_arg(args, char*);\
// 			result = JsonGetValueByKey(value, key);\
// 		} break;\
// 		case JSON_ARRAY: {\
// 			int index = va_arg(args, int);\
// 			result = JsonGetValueByIndex(value, index);\
// 		} break;\
// 		default:\
// 			print_err("JSON value is not an object or an array \n");\
// 			/*exit(1);*/\
// 			break;\
// 	}\
// \
// 	va_end(args);\
// \
// 	return resultName;\
// }

// _JSON_TYPE_GEN()

json_value_t* Json_GetObjOrArrayValue(json_value_t* value, va_list args)
{
	switch(value->type) {
		case JSON_OBJECT: {
			char* key = va_arg(args, char*);
			return Json_GetValueByKey(value, key);
		} break;
		case JSON_ARRAY: {
			int index = va_arg(args, int);
			return Json_GetValueByIndex(value, index);
		} break;
		default:
			print_err("JSON value is not an object or an array \n");
			/*exit(1);*/
			return NULL;
			break;
	}
}

int Json_GetInt(json_value_t* value, ...)
{
	va_list args;
	va_start(args, value);
	json_value_t* result = Json_GetObjOrArrayValue(value, args);
	va_end(args);
	// assert(result.type == JSON_INT);
	return result->i;
}
float Json_GetFloat(json_value_t* value, ...)
{
	va_list args;
	va_start(args, value);
	json_value_t* result = Json_GetObjOrArrayValue(value, args);
	va_end(args);
	if (result->type == JSON_INT) {
		return (float)result->i;
	}
	// assert(result.type == JSON_INT || result.type == JSON_FLOAT);
	return result->f;
}
char* Json_GetStr(json_value_t* value, ...)
{
	va_list args;
	va_start(args, value);
	json_value_t* result = Json_GetObjOrArrayValue(value, args);
	va_end(args);
	// assert(result.type == JSON_STRING);
	return result->s;
}
_Bool Json_GetBool(json_value_t* value, ...)
{
	va_list args;
	va_start(args, value);
	json_value_t* result = Json_GetObjOrArrayValue(value, args);
	va_end(args);
	// assert(result.type == JSON_BOOLEAN);
	return result->i;
}
json_value_t* Json_GetObj(json_value_t* value, ...)
{
	va_list args;
	va_start(args, value);
	json_value_t* result = Json_GetObjOrArrayValue(value, args);
	va_end(args);
	// assert(result.type == JSON_OBJECT);
	return result;
}
json_value_t* Json_GetArray(json_value_t* value, ...)
{
	va_list args;
	va_start(args, value);
	json_value_t* result = Json_GetObjOrArrayValue(value, args);
	va_end(args);
	// assert(result.type == JSON_ARRAY);
	return result;
}

// int JsonGetInt(json_value_t value, ...)
// {
// 	va_list args;
// 	va_start(args, value);
// 	json_value_t result = JsonGetValueArgs(value, args);
// 	assert(result.type == JSON_INT);
// 	return result.i;
// 	va_end(args);
// }

// #define JsonGetInt(value, keyOrIndex)\
// 	(json_value_t __result = JsonGetValueArgs(value, keyOrIndex), assert(result.type == JSON_INT), __result.i)

// #define JsonGetStr(value, keyOrIndex)\
// 	(json_value_t __result = JsonGetValueArgs(value, keyOrIndex), assert(result.type == JSON_STRING), __result.s)

json_value_t* Json_ObjGetArray(json_value_t* value, char* key)
{
	assert(value->type == JSON_OBJECT);
	json_value_t* v = value->first;
	while (v) {
		if (strcompare(v->key, key)) {
			assert(v->type == JSON_ARRAY);
			return v;
		}
		v = v->next;
	}

	print_err("JSON array not found: %s \n", key);
	// exit(1);
	return NULL;
}

json_value_t* Json_ArrayGetObj(json_value_t* value, int index)
{
	assert(value->type == JSON_ARRAY);
	assert(index < value->arrayLength);

	// json_value_t v = value.obj->items[index].value;
	json_value_t* v = Json_GetValueByIndex(value, index);
	assert(v->type == JSON_OBJECT);
	return v;

	print_err("JSON obj not found: %i \n", index);
	// exit(1);
	return NULL;
}

char* Json_ObjGetStr(json_value_t* value, char* key)
{
	assert(value->type == JSON_OBJECT);
	json_value_t* v = value->first;
	// for (int i=0; i<value.obj->length; ++i) {
	while (v) {
		if (strcompare(v->key, key)) {
			// json_value_t v = value.obj->items[i].value;
			assert(v->type == JSON_STRING);
			return v->s;
		}
		v = v->next;
	}

	print_err("JSON item not found: %s \n", key);
	exit(1);
}


#endif
#endif
