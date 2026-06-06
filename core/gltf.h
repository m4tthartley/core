/*
	Created by Matt Hartley on 24/04/2026.
	Copyright 2026 GiantJelly. All rights reserved.
*/

#ifndef __CORE_GLTF_HEADER__
#define __CORE_GLTF_HEADER__


#include "core.h"


#ifndef GLTF_ALLOC
#define GLTF_ALLOC _gltf_alloc_memory
#endif
#ifndef GLTF_FREE
#define GLTF_FREE _gltf_free_memory
#endif


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

void Gltf_Load(void* data)
{
	gltf_header_t* header = (gltf_header_t*)data;
	gltf_chunk_t* jsonChunk = (gltf_chunk_t*)(header + 1);
	gltf_chunk_t* dataChunk = (gltf_chunk_t*)(data + sizeof(gltf_header_t) + sizeof(gltf_chunk_t) + jsonChunk->length);

	json_value_t json = Json_Parse(jsonChunk->data, jsonChunk->length);
	// Json_PrintValue(json);

	mesh_t* finalMesh = push_memory(&meshMemory, sizeof(mesh_t));
	// sizeof(vertex_t)*indexCount
	// finalMesh->id = ASSET_LOG;
	// mesh->vertexCount = indexCount;
	finalMesh->vertexCount = 0;

	json_value_t meshes = JsonObjGetArray(json, "meshes");
	json_value_t accessors = JsonGetArray(json, "accessors");
	json_value_t bufferViews = JsonGetArray(json, "bufferViews");
	json_value_t nodes = JsonObjGetArray(json, "nodes");

	for (int meshIndex=0; meshIndex<meshes.array->length; ++meshIndex) {
		// TODO: start with nodes instead

		json_value_t mesh0 = JsonArrayGetObj(meshes, meshIndex);
		json_value_t primitives = JsonGetArray(mesh0, "primitives");
		json_value_t node0 = JsonGetObj(nodes, meshIndex);
		char* mesh0Name = JsonGetStr(mesh0, "name");
		char* node0Name = JsonGetStr(node0, "name");

		for (int primitiveIndex=0; primitiveIndex<primitives.array->length; ++primitiveIndex) {
			json_value_t prim0 = JsonGetObj(primitives, primitiveIndex);
			// char* mesh0Name = JsonObjGetStr(mesh0, "name");
			int indexAccIndex = JsonGetInt(prim0, "indices");
			json_value_t attributes = JsonGetObj(prim0, "attributes");
			int posAccIndex = JsonGetInt(attributes, "POSITION");
			int normalAccIndex = JsonGetInt(attributes, "NORMAL");
			int texcoordAccIndex = JsonGetInt(attributes, "TEXCOORD_0");
			// print("mesh0 %s \n", mesh0Name);

			vec3_t translation = vec3f(0);
			quaternion_t rotationQuart = QuatIdentity();
			vec3_t scale = vec3f(1);

			json_value_t rotation = JsonGetArray(node0, "rotation");
			json_value_t translationObj = JsonGetArray(node0, "translation");
			json_value_t scaleObj = JsonGetArray(node0, "scale");

			if (translationObj.type) {
				translation = vec3(
					JsonGetFloat(translationObj, 0),
					JsonGetFloat(translationObj, 1),
					JsonGetFloat(translationObj, 2)
				);
			}
			if (rotation.type) {
				rotationQuart = vec4(
					JsonGetFloat(rotation, 0),
					JsonGetFloat(rotation, 1),
					JsonGetFloat(rotation, 2),
					JsonGetFloat(rotation, 3)
				);
			}
			if (scaleObj.type) {
				scale = vec3(
					JsonGetFloat(scaleObj, 0),
					JsonGetFloat(scaleObj, 1),
					JsonGetFloat(scaleObj, 2)
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

			json_value_t posAccessor = JsonGetObj(accessors, posAccIndex);
			json_value_t normalAccessorObj = JsonGetObj(accessors, normalAccIndex);
			// int vertexCount = JsonGetInt(posAccessor, "count");

			json_value_t indexAccessorObj = JsonGetObj(accessors, indexAccIndex);
			int indexCount = JsonGetInt(indexAccessorObj, "count");

			json_value_t indexBufferView = JsonGetObj(bufferViews, JsonGetInt(indexAccessorObj, "bufferView"));
			int indexByteOffset = JsonGetInt(indexBufferView, "byteOffset");
			// uint16_t* indices = (uint16_t*)(dataChunk->data + indexByteOffset);

			json_value_t posBufferView = JsonGetObj(bufferViews, JsonGetInt(posAccessor, "bufferView"));
			int posByteOffset = JsonGetInt(posBufferView, "byteOffset");
			// vec3_t* positions = (vec3_t*)(dataChunk->data + posByteOffset);

			json_value_t normalBufferView = JsonGetObj(bufferViews, JsonGetInt(normalAccessorObj, "bufferView"));
			// vec3_t* normals = (vec3_t*)(dataChunk->data + JsonGetInt(normalBufferView, "byteOffset"));

			gltf_accessor_t indexAccessor = GLTFCreateAccessor(
				dataChunk->data + indexByteOffset,
				JsonGetStr(indexAccessorObj, "type"),
				JsonGetInt(indexAccessorObj, "componentType")
			);
			gltf_accessor_t positionAccessor = GLTFCreateAccessor(
				dataChunk->data + posByteOffset,
				JsonGetStr(posAccessor, "type"),
				JsonGetInt(posAccessor, "componentType")
			);
			gltf_accessor_t normalAccessor = GLTFCreateAccessor(
				dataChunk->data + JsonGetInt(normalBufferView, "byteOffset"),
				JsonGetStr(normalAccessorObj, "type"),
				JsonGetInt(normalAccessorObj, "componentType")
			);

			// vec3_t pos0 = ((vec3_t*)(dataChunk->data + byteOffset))[0];
			// vec3_t pos1 = ((vec3_t*)(dataChunk->data + byteOffset))[1];
			// vec3_t pos2 = ((vec3_t*)(dataChunk->data + byteOffset))[2];

			// float det = mat3_determinant(mat4_mat3(rMatrix));
			// print("det %f \n", det);

			vertex_t* vertices = push_memory(&meshMemory, sizeof(vertex_t)*indexCount);
			finalMesh->vertexCount += indexCount;

			for (int i=0; i<indexCount; ++i) {
				// vec3_t pos = positions[indices[i]];
				// mesh->vertices[i].pos = pos;
				// mesh->vertices[i].normal = mul3(normals[indices[i]], vec3f(1));

				uint32_t index = GLTFGetIndex(indexAccessor, i);
				vec3_t pos = GLTFGetVertex(positionAccessor, index);
				vec3_t normal = GLTFGetVertex(normalAccessor, index);

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
