/*
	Created by Matt Hartley on 24/04/2026.
	Copyright 2026 GiantJelly. All rights reserved.
*/

#ifndef __CORE_GLTF_HEADER__
#define __CORE_GLTF_HEADER__


#include "core.h"


#ifndef GLTF_ALLOC
#	ifndef CORE_ALLOC
#		define GLTF_ALLOC _gltf_alloc_memory
#	else
#		define GLTF_ALLOC CORE_ALLOC
#	endif
#endif
#ifndef GLTF_FREE
#	ifndef CORE_FREE
#		define GLTF_FREE _gltf_free_memory
#	else
#		define GLTF_FREE CORE_FREE
#	endif
#endif


typedef struct {
	vec3_t pos;
	vec3_t color;
	vec3_t normal;
	vec2_t uv;
} gltf_vertex_t;

typedef struct {
	uint32_t id;
	int vertexCount;
	gltf_vertex_t vertices[];
} gltf_mesh_t;


#define CORE_IMPL 
#ifdef CORE_IMPL

#include "json.h"


typedef struct {
	char magic[4];
	uint32_t version;
	uint32_t length;
} gltf_header_t;

typedef struct {
	uint32_t length;
	char type[4];
	uint8_t data[];
} gltf_chunk_t;

typedef struct {
	uint8_t* data;
	int count;
	int type;
	int componentType;
} gltf_accessor_t;


void* _gltf_alloc_memory(size_t size)
{
	void* p = malloc(size);
	return p;
}

void _gltf_free_memory(void* p)
{
	free(p);
}


// This is copied from town game, once the math lib from core
// is updated and tested properly this can move over to that.
/************************************************************************** */
// PROJECTION
mat4_t PerspectiveMatrix(float fov, float aspect, float near, float far)
{
	float a = aspect;
	float s = 1.0f / tanf((fov/180.0f*PI) / 2.0f);
	float n = near;
	float f = far;

	// (2*f*n)/(n-f) scales Z
	// -1 sets W to -Z
	mat4_t mat = {
		s/a, 0,  0,             0,
		0,   s,  0,             0,
		0,   0, (f+n)/(n-f),   -1,
		0,   0, (2*f*n)/(n-f),  0,
	};
	return mat;
}

// IDENTITY
mat4_t Identity4x4()
{
	mat4_t result = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1,
	};
	return result;
}

mat3_t Identity3x3()
{
	mat3_t result = {
		1, 0, 0,
		0, 1, 0,
		0, 0, 1,
	};
	return result;
}

// TRANSPOSE
mat4_t Transpose4x4(mat4_t m)
{
	mat4_t mat;
	for (int i=0; i<4; ++i) {
		for (int j=0; j<4; ++j) {
			// swap(mat.f[y*4+x], mat.f[x*4+y]);
			mat.f[j*4+i] = m.f[i*4+j];
		}
	}
	return mat;
}
mat3_t Transpose3x3(mat3_t m)
{
	mat3_t mat = m;
	for (int y=0; y<3; ++y) {
		for (int x=0; x<3; ++x) {
			// swap(mat.f[y*3+x], mat.f[x*3+y]);
			mat.f[y*3+x] = m.f[x*3+y];
		}
	}

	return mat;
}

// MULTIPLY
mat4_t Mul4x4(mat4_t m1, mat4_t m2)
{
	mat4_t out = {0};
	
	for (int y = 0; y < 4; ++y) {
		for (int x = 0; x < 4; ++x) {
			for (int i = 0; i < 4; ++i) {
				out.f[y*4+x] += m1.f[y*4+i] * m2.f[i*4+x];
			}
		}
	}
	
	return out;
}
vec4_t Mul4_4x4(vec4_t in, mat4_t mat)
{
	vec4_t result = {0};
	for (int v = 0; v < 4; ++v) {
		for (int i = 0; i < 4; ++i) {
			result.f[v] += in.f[i] * mat.f[i*4+v];
		}
	}
	return result;
}
vec4_t Mul4x4_4(mat4_t mat, vec4_t in)
{
	vec4_t result = {0};
	for (int v = 0; v < 4; ++v) {
		for (int i = 0; i < 4; ++i) {
			result.f[v] += mat.f[v*4+i] * in.f[i];
		}
	}
	return result;
}
vec3_t Mul3x3_3(mat3_t mat, vec3_t in)
{
	vec3_t result = {0};
	for (int v = 0; v < 3; ++v) {
		for (int i = 0; i < 3; ++i) {
			result.f[v] += mat.f[v*3+i] * in.f[i];
		}
	}
	return result;
}

// TRANSLATION
mat4_t Translation4x4(vec3_t pos)
{
	mat4_t result = {
		1,     0,     0,     0,
		0,     1,     0,     0,
		0,     0,     1,     0,
		pos.x, pos.y, pos.z, 1,
	};
	return result;
}

// ROTATION
mat4_t RotationX4x4(float rads)
{
	float s = sinf(rads);
	float c = cosf(rads);
	mat4_t result = {
		1,  0,  0,  0,
		0,  c,  s,  0,
		0, -s,  c,  0,
		0,  0,  0,  1,
	};
	return result;
}
mat4_t RotationY4x4(float rads)
{
	float s = sinf(rads);
	float c = cosf(rads);
	mat4_t result = {
		c,  0, -s,  0,
		0,  1,  0,  0,
		s,  0,  c,  0,
		0,  0,  0,  1,
	};
	return result;
}
mat4_t RotationZ4x4(float rads)
{
	float s = sinf(rads);
	float c = cosf(rads);
	mat4_t result = {
		c, s, 0, 0,
		-s, c,  0, 0,
		0,          0,           1, 0,
		0,          0,           0, 1,
	};
	return result;
}

// SCALE
mat4_t Scale4x4(vec3_t s)
{
	mat4_t result = {
		s.x, 0,   0,   0,
		0,   s.y, 0,   0,
		0,   0,   s.z, 0,
		0,   0,   0,   1,
	};
	return result;
}

/*
	Vec3()
	Mul3()
	Add3()

	Identity4x4()
	Mul4x4()
	Transpose4x4()
	Rotation4x4()

	---

	Mat4Identity()
	Mat4Mul()
	Mat4MulVec4()
	Vec4MulMat4()
	Mat4Transpose()
	Mat4RotationX()

	QuatIdentity()
	QuatMul()
	QuatAdd()
	QuatRotation()
	QuatToMat4()
	Mat4FromQuat()
*/

// QUATERNIONS
quat_t QuatIdentity()
{
	quat_t result = {0.0f, 0.0f, 0.0f, 1.0f};
	return result;
}

quat_t QuatMul(quat_t q1, quat_t q2)
{
	quat_t q;
	q.w = q1.w*q2.w - q1.x*q2.x - q1.y*q2.y - q1.z*q2.z;
	q.x = q1.w*q2.x + q1.x*q2.w + q1.y*q2.z - q1.z*q2.y;
	q.y = q1.w*q2.y - q1.x*q2.z + q1.y*q2.w + q1.z*q2.x;
	q.z = q1.w*q2.z + q1.x*q2.y - q1.y*q2.x + q1.z*q2.w;
	return q;
}

quat_t QuatDiv(quat_t q, float f)
{
	quat_t result;
	result.x = q.x / f;
	result.y = q.y / f;
	result.z = q.z / f;
	result.w = q.w / f;
	return result;
}

float QuatDot(quat_t a, quat_t b)
{
	return (a.x*b.x) + (a.y*b.y) + (a.z*b.z) + (a.w*b.w);
}

quat_t QuatInverse(quat_t q)
{
	quat_t result;
	result.x = -q.x;
	result.y = -q.y;
	result.z = -q.z;
	result.w = q.w;
	return QuatDiv(result, qdot(q,q));
}

quat_t QuatRotation(vec3_t axis, float angle)
{
	quat_t local;
	local.w = cosf(angle/2.0f);
	local.x = axis.x * sinf(angle/2.0f);
	local.y = axis.y * sinf(angle/2.0f);
	local.z = axis.z * sinf(angle/2.0f);
	
	return local;
}

// CORE_MATH_FUNC void qrotate_apply(quaternion_t *q, vec3_t axis, float angle) {
// 	quaternion_t local;
// 	local.w = cosf(angle/2.0f);
// 	local.x = axis.x * sinf(angle/2.0f);
// 	local.y = axis.y * sinf(angle/2.0f);
// 	local.z = axis.z * sinf(angle/2.0f);
	
// 	*q = qmul(local, *q);
// 	*q = normalize4(*q); // TODO: check if it needs this first
// }

vec3_t QuatRotateVec3(vec3_t v, quat_t q)
{
	vec4_t p = vec4f3(v, 0);
	vec4_t conjugate = vec4(-q.x, -q.y, -q.z, q.w);
	vec4_t vQ = QuatMul(QuatMul(q, p), conjugate);
	return vQ.xyz;
}

quat_t QuatNLerp(quat_t a, quat_t b, float t)
{
	quat_t result;
	result.x = lerp(a.x, b.x, t);
	result.y = lerp(a.y, b.y, t);
	result.z = lerp(a.z, b.z, t);
	result.w = lerp(a.w, b.w, t);
	return normalize4(result);
}

mat4_t QuatMat4(quat_t q)
{
	float len = sqrtf(q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w);
	float x = q.x / len;
	float y = q.y / len;
	float z = q.z / len;
	float w = q.w / len;

	mat4_t result;
	result.f[0] = 1.0f - 2.0f * (y*y + z*z);
    result.f[1] = 2.0f * (x*y + w*z);
    result.f[2] = 2.0f * (x*z - w*y);
    result.f[3] = 0.0f;
	
    result.f[4] = 2.0f * (x*y - w*z);
    result.f[5] = 1.0f - 2.0f * (x*x + z*z);
    result.f[6] = 2.0f * (y*z + w*x);
    result.f[7] = 0.0f;
	
    result.f[8] = 2.0f * (x*z + w*y);
    result.f[9] = 2.0f * (y*z - w*x);
    result.f[10] = 1.0f - 2.0f * (x*x + y*y);
    result.f[11] = 0.0f;
	
    result.f[12] = 0.0f;
    result.f[13] = 0.0f;
    result.f[14] = 0.0f;
    result.f[15] = 1.0f;
	
	return result;
}
/************************************************************************** */


enum {
	GLTF_BYTE			= 5120,
	GLTF_UNSIGNED_BYTE	= 5121,
	GLTF_SHORT			= 5122,
	GLTF_UNSIGNED_SHORT	= 5123,
	GLTF_INT			= 5124,
	GLTF_UNSIGNED_INT	= 5125,
	GLTF_FLOAT			= 5126,
};
int _gltfComponentSizes[] = {
	1, 1, 2, 2, 4, 4, 4,
};

enum {
	GLTF_SCALAR = 0,
	GLTF_VEC2,
	GLTF_VEC3,
	GLTF_VEC4,
	GLTF_MAT2,
	GLTF_MAT3,
	GLTF_MAT4,
};
int _gltfTypeSizes[] = {
	1, 2, 3, 4, 4, 9, 16,
};
char* _gltfTypeNames[] = {
	"SCALAR",
	"VEC2",
	"VEC3",
	"VEC4",
	"MAT2",
	"MAT3",
	"MAT4",
};

gltf_accessor_t GLTF_CreateAccessor(uint8_t* data, char* type, int componentType)
{
	int t = -1;
	// if (strcompare(type, "VEC2")) {
	// 	t = GLTF_VEC2;
	// }
	// else if (strcompare(type, "VEC3")) {
	// 	t = GLTF_VEC3;
	// }
	for (int i=0; i<array_size(_gltfTypeNames); ++i) {
		if (strcompare(type, _gltfTypeNames[i])) {
			t = i;
			break;
		}
	}

	if (t == -1) {
		print_err("Unknown type name: %s \n", type);
		exit(1);
	}

	gltf_accessor_t result = {
		.data = data,
		.type = t,
		.componentType = componentType,
	};

	return result;
}

vec3_t GLTF_GetVertex(gltf_accessor_t accessor, uint32_t index)
{
	int typeSize = _gltfTypeSizes[accessor.type];
	int componentSize = _gltfComponentSizes[accessor.componentType - GLTF_BYTE];
	int stride = typeSize * componentSize;

	uint8_t* data = accessor.data + index*stride;
	vec3_t result = {0};

	for (int i=0; i<min(typeSize, 3); ++i) {
		switch (accessor.componentType) {
			case GLTF_BYTE: {
				result.f[i] = (float)(*(int8_t*)data);
			} break;
			case GLTF_UNSIGNED_BYTE: {
				result.f[i] = (float)(*(uint8_t*)data);
			} break;
			case GLTF_SHORT: {
				result.f[i] = (float)(*(int16_t*)data);
			} break;
			case GLTF_UNSIGNED_SHORT: {
				result.f[i] = (float)(*(uint16_t*)data);
			} break;
			case GLTF_INT: {
				result.f[i] = (float)(*(int32_t*)data);
			} break;
			case GLTF_UNSIGNED_INT: {
				result.f[i] = (float)(*(uint32_t*)data);
			} break;
			case GLTF_FLOAT: {
				result.f[i] = *(float*)data;
			} break;
			default:
				print_err("GLTF: Invalid vertex accessor component type \n");
				exit(1);
		}

		data += componentSize;
	}

	return result;
}

uint32_t GLTF_GetIndex(gltf_accessor_t accessor, uint32_t index)
{
	assert(accessor.type == GLTF_SCALAR);
	int componentSize = _gltfComponentSizes[accessor.componentType - GLTF_BYTE];

	uint8_t* data = accessor.data + index*componentSize;

	switch (accessor.componentType) {
		case GLTF_UNSIGNED_BYTE: {
			return *(uint8_t*)data;
		} break;
		case GLTF_UNSIGNED_SHORT: {
			return *(uint16_t*)data;
		} break;
		case GLTF_UNSIGNED_INT: {
			return *(uint32_t*)data;
		} break;
		default:
			print_err("GLTF: Invalid index accessor component type \n");
			exit(1);
	}
}

gltf_mesh_t* Gltf_Load(void* data)
{
	gltf_header_t* header = (gltf_header_t*)data;
	gltf_chunk_t* jsonChunk = (gltf_chunk_t*)(header + 1);
	gltf_chunk_t* dataChunk = (gltf_chunk_t*)(data + sizeof(gltf_header_t) + sizeof(gltf_chunk_t) + jsonChunk->length);

	json_value_t json = Json_Parse(jsonChunk->data, jsonChunk->length);
	// Json_PrintValue(json);

	gltf_mesh_t* finalMesh = GLTF_ALLOC(sizeof(gltf_mesh_t));
	// sizeof(vertex_t)*indexCount
	// finalMesh->id = ASSET_LOG;
	// mesh->vertexCount = indexCount;
	finalMesh->vertexCount = 0;

	json_value_t meshes = Json_ObjGetArray(json, "meshes");
	json_value_t accessors = Json_GetArray(json, "accessors");
	json_value_t bufferViews = Json_GetArray(json, "bufferViews");
	json_value_t nodes = Json_ObjGetArray(json, "nodes");

	for (int meshIndex=0; meshIndex<meshes.array->length; ++meshIndex) {
		// TODO: start with nodes instead

		json_value_t mesh0 = Json_ArrayGetObj(meshes, meshIndex);
		json_value_t primitives = Json_GetArray(mesh0, "primitives");
		json_value_t node0 = Json_GetObj(nodes, meshIndex);
		char* mesh0Name = Json_GetStr(mesh0, "name");
		char* node0Name = Json_GetStr(node0, "name");

		for (int primitiveIndex=0; primitiveIndex<primitives.array->length; ++primitiveIndex) {
			json_value_t prim0 = Json_GetObj(primitives, primitiveIndex);
			// char* mesh0Name = Json_ObjGetStr(mesh0, "name");
			int indexAccIndex = Json_GetInt(prim0, "indices");
			json_value_t attributes = Json_GetObj(prim0, "attributes");
			int posAccIndex = Json_GetInt(attributes, "POSITION");
			int normalAccIndex = Json_GetInt(attributes, "NORMAL");
			int texcoordAccIndex = Json_GetInt(attributes, "TEXCOORD_0");
			// print("mesh0 %s \n", mesh0Name);

			vec3_t translation = vec3f(0);
			quaternion_t rotationQuart = QuatIdentity();
			vec3_t scale = vec3f(1);

			json_value_t rotation = Json_GetArray(node0, "rotation");
			json_value_t translationObj = Json_GetArray(node0, "translation");
			json_value_t scaleObj = Json_GetArray(node0, "scale");

			if (translationObj.type) {
				translation = vec3(
					Json_GetFloat(translationObj, 0),
					Json_GetFloat(translationObj, 1),
					Json_GetFloat(translationObj, 2)
				);
			}
			if (rotation.type) {
				rotationQuart = vec4(
					Json_GetFloat(rotation, 0),
					Json_GetFloat(rotation, 1),
					Json_GetFloat(rotation, 2),
					Json_GetFloat(rotation, 3)
				);
			}
			if (scaleObj.type) {
				scale = vec3(
					Json_GetFloat(scaleObj, 0),
					Json_GetFloat(scaleObj, 1),
					Json_GetFloat(scaleObj, 2)
				);
			}
			// print("translation %f,%f,%f \n", translation.x, translation.y, translation.z);
			// print("scale %f,%f,%f \n", scale.x, scale.y, scale.z);

			mat4_t translationMatrix = Translation4x4(translation);
			mat4_t rotationMatrix = QuatMat4(rotationQuart); //mat4_mul(qmat4(qrotate(vec3(1, 0, 0), 1)), qmat4(rotationQuart)); 
			mat4_t scaleMatrix = Scale4x4(scale);
			// mat4_t extraRotMatrix = QuatMat4(QuatRotation(vec3(1, 0, 0), 1));
			// mat4_t transform = mat4_mul(mat4_mul(pMatrix, rMatrix), sMatrix);
			mat4_t zupRotation = RotationX4x4(PI/2);
			mat4_t yinRotation = RotationZ4x4(PI);
			mat4_t engineAdjustRotation = Mul4x4(zupRotation, yinRotation);
			mat4_t transform = Mul4x4(Mul4x4(Mul4x4(scaleMatrix, rotationMatrix), translationMatrix), engineAdjustRotation);
			// transform = mat4_mul(transform, extraRotMatrix);
			// mat3_t inverseRotTransform = Transpose4x4(mat3_inverse(mat4_mat3(rotationMatrix)));

			json_value_t posAccessor = Json_GetObj(accessors, posAccIndex);
			json_value_t normalAccessorObj = Json_GetObj(accessors, normalAccIndex);
			// int vertexCount = Json_GetInt(posAccessor, "count");

			json_value_t indexAccessorObj = Json_GetObj(accessors, indexAccIndex);
			int indexCount = Json_GetInt(indexAccessorObj, "count");

			json_value_t indexBufferView = Json_GetObj(bufferViews, Json_GetInt(indexAccessorObj, "bufferView"));
			int indexByteOffset = Json_GetInt(indexBufferView, "byteOffset");
			// uint16_t* indices = (uint16_t*)(dataChunk->data + indexByteOffset);

			json_value_t posBufferView = Json_GetObj(bufferViews, Json_GetInt(posAccessor, "bufferView"));
			int posByteOffset = Json_GetInt(posBufferView, "byteOffset");
			// vec3_t* positions = (vec3_t*)(dataChunk->data + posByteOffset);

			json_value_t normalBufferView = Json_GetObj(bufferViews, Json_GetInt(normalAccessorObj, "bufferView"));
			// vec3_t* normals = (vec3_t*)(dataChunk->data + Json_GetInt(normalBufferView, "byteOffset"));

			gltf_accessor_t indexAccessor = GLTF_CreateAccessor(
				dataChunk->data + indexByteOffset,
				Json_GetStr(indexAccessorObj, "type"),
				Json_GetInt(indexAccessorObj, "componentType")
			);
			gltf_accessor_t positionAccessor = GLTF_CreateAccessor(
				dataChunk->data + posByteOffset,
				Json_GetStr(posAccessor, "type"),
				Json_GetInt(posAccessor, "componentType")
			);
			gltf_accessor_t normalAccessor = GLTF_CreateAccessor(
				dataChunk->data + Json_GetInt(normalBufferView, "byteOffset"),
				Json_GetStr(normalAccessorObj, "type"),
				Json_GetInt(normalAccessorObj, "componentType")
			);

			// vec3_t pos0 = ((vec3_t*)(dataChunk->data + byteOffset))[0];
			// vec3_t pos1 = ((vec3_t*)(dataChunk->data + byteOffset))[1];
			// vec3_t pos2 = ((vec3_t*)(dataChunk->data + byteOffset))[2];

			// float det = mat3_determinant(mat4_mat3(rMatrix));
			// print("det %f \n", det);

			gltf_vertex_t* vertices = GLTF_ALLOC(sizeof(gltf_vertex_t)*indexCount);
			finalMesh->vertexCount += indexCount;

			for (int i=0; i<indexCount; ++i) {
				// vec3_t pos = positions[indices[i]];
				// mesh->vertices[i].pos = pos;
				// mesh->vertices[i].normal = mul3(normals[indices[i]], vec3f(1));

				uint32_t index = GLTF_GetIndex(indexAccessor, i);
				vec3_t pos = GLTF_GetVertex(positionAccessor, index);
				vec3_t normal = GLTF_GetVertex(normalAccessor, index);

				// pos = vec3(pos.x, pos.z, pos.y);
				
				vertices[i].pos = Mul4_4x4(vec4f3(pos, 1), transform).xyz;
				// vertices[i].pos = add3(vertices[i].pos, translation);
				// vertices[i].normal = Mul4_4x4(vec4f3(mul3f(normal, -1), 0), Mul4x4(rotationMatrix, zupRotation)).xyz;
				vertices[i].normal = Mul4_4x4(vec4f3(normal, 0), Mul4x4(rotationMatrix, engineAdjustRotation)).xyz;
				vertices[i].color = vec3(0.8f, 0.5f, 0.2f); //mesh->vertices[i].normal;
				// mesh->vertices[i].normal = normalize3(mat3_mul_vec3(inverseRotTransform, normal));
			}

			// mesh_t* cubeMesh = R_BeginMesh();
			// R_MeshPushCube(cubeMesh, vec3f(0.5f), vec3f(1));
			// R_EndMesh(&cubeMesh);
		}
	}

	return finalMesh;
}


#endif
#endif
