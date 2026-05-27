/*
	Created by Matt Hartley on 24/04/2026.
	Copyright 2026 GiantJelly. All rights reserved.
*/

#ifndef __CORE_JSON_HEADER__
#define __CORE_JSON_HEADER__

#include "core.h"

#ifndef JSON_ALLOC
#define JSON_ALLOC _json_alloc_memory
#endif
#ifndef JSON_FREE
#define JSON_FREE _json_free_memory
#endif


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

typedef struct json_obj_t json_obj_t;

typedef struct {
	uint8_t type;
	union {
		int32_t i;
		float f;
		char* s;
		json_obj_t* obj;
		json_obj_t* array;
	};
} json_value_t;

// TODO: Try using linked list
typedef struct json_obj_t {
	struct {
		char* key;
		json_value_t value;
	} items[64]; // TODO: Change this stuff to a more dynamic system
	uint32_t length;
} json_obj_t;

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

json_value_t _Json_ParseValue(json_parser_t* parser);
void _Json_PrintValueWithKey(char* key, json_value_t value);
void _Json_PrintValue(json_value_t value);

void* _json_alloc_memory(size_t size)
{
	void* p = malloc(size);
	return p;
}

void _json_free_memory(void* p)
{
	free(p);
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

json_obj_t _Json_ParseObject(json_parser_t* parser)
{
	json_obj_t obj = {0};

	json_token_t token;
	while (_Json_ReadToken(parser, &token) && token.type == JSON_TOKEN_STRING) {
		_Json_ExpectReadToken(parser, ':');
		json_value_t value = _Json_ParseValue(parser);
		// print("key value: %s: %s \n", token.str, GetValueString(value));

		assert(obj.length < array_size(obj.items));
		obj.items[obj.length].key = token.str;
		obj.items[obj.length].value = value;
		++obj.length;

		_Json_ReadToken(parser, &token);
		if (token.type != ',') {
			_Json_ExpectToken(token, '}');
			break;
		}
	}

	return obj;
}

json_obj_t _Json_ParseArray(json_parser_t* parser)
{
	json_obj_t obj = {0};

	// json_token_t token;
	json_value_t value;
	while ((value = _Json_ParseValue(parser)), _True) {
		// json_value_t value = ParseValue(parser);
		// print("array value: %s \n", GetValueString(value));

		assert(obj.length < array_size(obj.items));
		obj.items[obj.length].value = value;
		++obj.length;

		json_token_t token;
		_Json_ReadToken(parser, &token);
		if (token.type != ',') {
			_Json_ExpectToken(token, ']');
			break;
		}
	}

	return obj;
}

json_value_t _Json_ParseValue(json_parser_t* parser)
{
	json_token_t token;
	_Json_ReadToken(parser, &token);

	switch (token.type) {
		case JSON_TOKEN_STRING: {
			json_value_t value = {
				.type = JSON_STRING,
				.s = token.str,
			};
			// int len = strsize(value.s);
			// value.s[len-1] = 0;
			// ++value.s;
			return value;
		} break;
		case JSON_TOKEN_NUMBER:
			if (strfind(token.str, ".")) {
				float value = atof(token.str);
				return (json_value_t){
					.type = JSON_FLOAT,
					.f = value,
				};
			} else {
				int value = atoi(token.str);
				return (json_value_t){
					.type = JSON_INT,
					.i = value,
				};
			}
			break;
		case JSON_TOKEN_BOOLEAN:
			return (json_value_t){
				.type = JSON_BOOLEAN,
				.i = token.b,
			};
			break;
		case '{': {
			// parse object
			json_obj_t obj = _Json_ParseObject(parser);
			json_obj_t* objp = JSON_ALLOC(sizeof(obj));
			*objp = obj;
			return (json_value_t){
				.type = JSON_OBJECT,
				.obj = objp,
			};
		} break;
		case '[': {
			// parse array
			json_obj_t obj = _Json_ParseArray(parser);
			json_obj_t* objp = JSON_ALLOC(sizeof(obj));
			*objp = obj;
			return (json_value_t){
				.type = JSON_ARRAY,
				.obj = objp,
			};
		} break;
		default:
			// error 
			print_err("ParseValue: Unexpected token: %i, %c, \"%s\" \n", (int)token.type, token.type, token.str);
			exit(1);
	}
}

int indent = 0;
void _Json_PrintValueWithKey(char* key, json_value_t value)
{
	// char indentStr[16] = {0};
	// for (int i=0; i<min(indent, 15); ++i) indentStr[i] = ' ';
	// print(indentStr);
	char* indentToken = "\t";
	for (int i=0; i<indent; ++i) print(indentToken);

	if (key) {
		print("\"%s\": ", key);
	}

	switch (value.type) {
		case JSON_STRING:
			print("\"%s\"", value.s);
			break;
		case JSON_INT:
			print("%i", value.i);
			break;
		case JSON_FLOAT:
			print("%f", value.f);
			break;
		case JSON_BOOLEAN:
			print("%s", value.i ? "true" : "false");
			break;
		case JSON_OBJECT: {
			json_obj_t obj = *(json_obj_t*)value.obj;
			print("{ \n");
			++indent;
			for (int i=0; i<obj.length; ++i) {
				// print("\"%s\": ", obj.items[i].key);
				// print("%s: ", obj.items[i].key);
				_Json_PrintValueWithKey(obj.items[i].key, obj.items[i].value);
				print(i+1==obj.length ? "\n" : ",\n");
			}
			--indent;
			for (int i=0; i<indent; ++i) print(indentToken);
			print("}");
			// PrintJsonValue()
		} break;
		case JSON_ARRAY: {
			json_obj_t obj = *(json_obj_t*)value.obj;
			print("[ \n");
			++indent;
			for (int i=0; i<obj.length; ++i) {
				// print("\"%s\": ", obj.items[i].key);
				_Json_PrintValue(obj.items[i].value);
				print(i+1==obj.length ? "\n" : ",\n");
			}
			--indent;
			for (int i=0; i<indent; ++i) print(indentToken);
			print("]");
			// PrintJsonValue()
		} break;
	}
}
void _Json_PrintValue(json_value_t value)
{
	_Json_PrintValueWithKey(NULL, value);
}

void _Json_WriteToFile(file_t file, char* str)
{
	int size = strsize(str);
	sys_write_seq(file, str, size);
}

void _Json_OutputValueToFileHandle(file_t file, char* key, json_value_t value)
{
	// char indentStr[16] = {0};
	// for (int i=0; i<min(indent, 15); ++i) indentStr[i] = ' ';
	// print(indentStr);
	char* indentToken = "\t";
	for (int i=0; i<indent; ++i) _Json_WriteToFile(file, strformat(indentToken));

	if (key) {
		_Json_WriteToFile(file, strformat("\"%s\": ", key));
	}

	switch (value.type) {
		case JSON_STRING:
			_Json_WriteToFile(file, strformat("\"%s\"", value.s));
			break;
		case JSON_INT:
			_Json_WriteToFile(file, strformat("%i", value.i));
			break;
		case JSON_FLOAT:
			_Json_WriteToFile(file, strformat("%f", value.f));
			break;
		case JSON_BOOLEAN:
			_Json_WriteToFile(file, strformat("%s", value.i ? "true" : "false"));
			break;
		case JSON_OBJECT: {
			json_obj_t obj = *(json_obj_t*)value.obj;
			_Json_WriteToFile(file, strformat("{ \n"));
			++indent;
			for (int i=0; i<obj.length; ++i) {
				// WriteToFile(file, strformat("\"%s\": ", obj.items[i].key));
				// WriteToFile(file, strformat("%s: ", obj.items[i].key));
				_Json_OutputValueToFileHandle(file, obj.items[i].key, obj.items[i].value);
				_Json_WriteToFile(file, strformat(i+1==obj.length ? "\n" : ",\n"));
			}
			--indent;
			for (int i=0; i<indent; ++i) _Json_WriteToFile(file, strformat(indentToken));
			_Json_WriteToFile(file, strformat("}"));
			// PrintJsonValue()
		} break;
		case JSON_ARRAY: {
			json_obj_t obj = *(json_obj_t*)value.obj;
			_Json_WriteToFile(file, strformat("[ \n"));
			++indent;
			for (int i=0; i<obj.length; ++i) {
				// WriteToFile(file, strformat("\"%s\": ", obj.items[i].key));
				_Json_OutputValueToFileHandle(file, NULL, obj.items[i].value);
				_Json_WriteToFile(file, strformat(i+1==obj.length ? "\n" : ",\n"));
			}
			--indent;
			for (int i=0; i<indent; ++i) _Json_WriteToFile(file, strformat(indentToken));
			_Json_WriteToFile(file, strformat("]"));
		} break;
	}
}
void _Json_OutputValueToFile(char* filename, json_value_t value)
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

json_value_t Json_GetValueByIndex(json_value_t value, int index)
{
	assert(index < array_size(value.array->items));
	json_value_t v = value.obj->items[index].value;
	return v;
}

json_value_t Json_GetValueByKey(json_value_t value, char* key)
{
	for (int i=0; i<value.obj->length; ++i) {
		if (strcompare(value.obj->items[i].key, key)) {
			return Json_GetValueByIndex(value, i);
		}
	}

	print_err("JSON value not found: %s \n", key);
	// exit(1);
	return (json_value_t){0};
}

json_value_t Json_GetValueArgs(json_value_t value, ...)
{
	va_list args;
	va_start(args, value);

	switch(value.type) {
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
			// exit(1);
			return (json_value_t){0};
			break;
	}

	va_end(args);
}

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

json_value_t Json_GetObjOrArrayValue(json_value_t value, va_list args)
{
	switch(value.type) {
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
			return (json_value_t){0};
			break;
	}
}

int Json_GetInt(json_value_t value, ...)
{
	va_list args;
	va_start(args, value);
	json_value_t result = Json_GetObjOrArrayValue(value, args);
	va_end(args);
	// assert(result.type == JSON_INT);
	return result.i;
}
float Json_GetFloat(json_value_t value, ...)
{
	va_list args;
	va_start(args, value);
	json_value_t result = Json_GetObjOrArrayValue(value, args);
	va_end(args);
	if (result.type == JSON_INT) {
		return (float)result.i;
	}
	// assert(result.type == JSON_INT || result.type == JSON_FLOAT);
	return result.f;
}
char* Json_GetStr(json_value_t value, ...)
{
	va_list args;
	va_start(args, value);
	json_value_t result = Json_GetObjOrArrayValue(value, args);
	va_end(args);
	// assert(result.type == JSON_STRING);
	return result.s;
}
_Bool Json_GetBool(json_value_t value, ...)
{
	va_list args;
	va_start(args, value);
	json_value_t result = Json_GetObjOrArrayValue(value, args);
	va_end(args);
	// assert(result.type == JSON_BOOLEAN);
	return result.i;
}
json_value_t Json_GetObj(json_value_t value, ...)
{
	va_list args;
	va_start(args, value);
	json_value_t result = Json_GetObjOrArrayValue(value, args);
	va_end(args);
	// assert(result.type == JSON_OBJECT);
	return result;
}
json_value_t Json_GetArray(json_value_t value, ...)
{
	va_list args;
	va_start(args, value);
	json_value_t result = Json_GetObjOrArrayValue(value, args);
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

json_value_t Json_ObjGetArray(json_value_t value, char* key)
{
	assert(value.type == JSON_OBJECT);
	for (int i=0; i<value.obj->length; ++i) {
		if (strcompare(value.obj->items[i].key, key)) {
			json_value_t v = value.obj->items[i].value;
			assert(v.type == JSON_ARRAY);
			return v;
		}
	}

	print_err("JSON array not found: %s \n", key);
	exit(1);
}

json_value_t Json_ArrayGetObj(json_value_t value, int index)
{
	assert(value.type == JSON_ARRAY);
	assert(index < array_size(value.array->items));

	json_value_t v = value.obj->items[index].value;
	assert(v.type == JSON_OBJECT);
	return v;

	print_err("JSON obj not found: %i \n", index);
	exit(1);
}

char* Json_ObjGetStr(json_value_t value, char* key)
{
	assert(value.type == JSON_OBJECT);
	for (int i=0; i<value.obj->length; ++i) {
		if (strcompare(value.obj->items[i].key, key)) {
			json_value_t v = value.obj->items[i].value;
			assert(v.type == JSON_STRING);
			return v.s;
		}
	}

	print_err("JSON item not found: %s \n", key);
	exit(1);
}


#endif
#endif
