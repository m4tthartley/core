//
//  math.h
//  Core
//
//  Created by Matt Hartley on 26/10/2023.
//  Copyright 2023 GiantJelly. All rights reserved.
//

#include <stdlib.h>
#include <stdint.h>
#include <math.h>

#ifndef __CORE_MATH_HEADER__
#define __CORE_MATH_HEADER__

#undef min
#undef max
#undef near
#undef far

#define PI 3.14159265359f
#define PI2 (PI*2.0f)

// #define CORE_INLINE
#define CORE_MATH_FUNC

typedef union {
	struct {
		int x;
		int y;
	};
	struct {
		int u;
		int v;
	};
	struct {
		int w;
		int h;
	};
	int i[2];
	uint64_t large;
} int2_t;
typedef int2_t point_t;

typedef union {
	struct {
		int x;
		int y;
		int z;
	};
	struct {
		int r;
		int g;
		int a;
	};
	int2_t xy;
	int i[3];
} int3_t;

typedef union {
	struct {
		int x;
		int y;
		int z;
		int w;
	};
	struct {
		int r;
		int g;
		int b;
		int a;
	};
	struct {
		int left;
		int bottom;
		int right;
		int top;
	};
	int3_t xyz;
	int3_t rgb;
	int2_t xy;
	int i[4];
} int4_t;

typedef union {
	struct {
		float x;
		float y;
	};
	struct {
		float u;
		float v;
	};
	float f[2];
} vec2_t;

typedef union {
	struct {
		float x;
		float y;
		float z;
	};
	struct {
		float r;
		float g;
		float b;
	};
	vec2_t xy;
	float f[3];
} vec3_t;

typedef union {
	struct {
		float x;
		float y;
		float z;
		float w;
	};
	struct {
		float r;
		float g;
		float b;
		float a;
	};
	struct {
		float left;
		float bottom;
		float right;
		float top;
	};
	vec3_t xyz;
	vec3_t rgb;
	vec2_t xy;
	float f[4];
} vec4_t;

typedef vec4_t quaternion_t;
typedef quaternion_t quat_t;
typedef vec2_t v2;
typedef vec3_t v3;
typedef vec4_t v4;
typedef int2_t i2;
typedef int3_t i3;

typedef union {
	struct {
		float m00;
		float m01;
		float m02;
		float m03;
		float m10;
		float m11;
		float m12;
		float m13;
		float m20;
		float m21;
		float m22;
		float m23;
		float m30;
		float m31;
		float m32;
		float m33;
	};
	float f[16];
} mat4_t;


CORE_MATH_FUNC int2_t int2(int x, int y);
CORE_MATH_FUNC int2_t point(int x, int y);
CORE_MATH_FUNC int3_t int3(int x, int y, int z);
CORE_MATH_FUNC vec2_t vec2(float x, float y);
CORE_MATH_FUNC vec2_t vec2f(float f);
CORE_MATH_FUNC vec2_t point_to_vec2(point_t p);
CORE_MATH_FUNC vec3_t vec3(float x, float y, float z);
CORE_MATH_FUNC vec3_t vec3f(float f);
CORE_MATH_FUNC vec3_t vec3f2(vec2_t v, float z);
CORE_MATH_FUNC vec4_t vec4(float x, float y, float z, float w);
CORE_MATH_FUNC vec4_t vec4f(float f);
CORE_MATH_FUNC vec4_t vec4f2(vec2_t v, float z, float w);
CORE_MATH_FUNC vec4_t vec4f3(vec3_t v, float w);
CORE_MATH_FUNC vec2_t add2(vec2_t a, vec2_t b);
CORE_MATH_FUNC vec2_t add2f(vec2_t a, float b);
CORE_MATH_FUNC vec3_t add3(vec3_t a, vec3_t b);
CORE_MATH_FUNC vec3_t add3f(vec3_t a, float b);
CORE_MATH_FUNC vec4_t add4(vec4_t a, vec4_t b);
CORE_MATH_FUNC vec4_t add4f(vec4_t a, float b);
CORE_MATH_FUNC vec2_t sub2(vec2_t a, vec2_t b);
CORE_MATH_FUNC vec2_t sub2f(vec2_t a, float b);
CORE_MATH_FUNC vec3_t sub3(vec3_t a, vec3_t b);
CORE_MATH_FUNC vec3_t sub3f(vec3_t a, float b);
CORE_MATH_FUNC vec4_t sub4(vec4_t a, vec4_t b);
CORE_MATH_FUNC vec4_t sub4f(vec4_t a, float b);
CORE_MATH_FUNC vec2_t mul2(vec2_t a, vec2_t b);
CORE_MATH_FUNC vec2_t mul2f(vec2_t a, float b);
CORE_MATH_FUNC vec3_t mul3(vec3_t a, vec3_t b);
CORE_MATH_FUNC vec3_t mul3f(vec3_t a, float b);
CORE_MATH_FUNC vec4_t mul4(vec4_t a, vec4_t b);
CORE_MATH_FUNC vec4_t mul4f(vec4_t a, float b);
CORE_MATH_FUNC vec2_t div2(vec2_t a, vec2_t b);
CORE_MATH_FUNC vec2_t div2f(vec2_t a, float b);
CORE_MATH_FUNC vec3_t div3(vec3_t a, vec3_t b);
CORE_MATH_FUNC vec3_t div3f(vec3_t a, float b);
CORE_MATH_FUNC vec4_t div4(vec4_t a, vec4_t b);
CORE_MATH_FUNC vec4_t div4f(vec4_t a, float b);
CORE_MATH_FUNC float len2(vec2_t a);
CORE_MATH_FUNC float len3(vec3_t a);
CORE_MATH_FUNC float len4(vec4_t a);
CORE_MATH_FUNC vec2_t normalize2(vec2_t v);
CORE_MATH_FUNC vec3_t normalize3(vec3_t v);
CORE_MATH_FUNC vec4_t normalize4(vec4_t v);
CORE_MATH_FUNC vec2_t floor2(vec2_t a);
CORE_MATH_FUNC vec3_t floor3(vec3_t a);
CORE_MATH_FUNC vec4_t floor4(vec4_t a);
CORE_MATH_FUNC float fract(float a);
CORE_MATH_FUNC vec2_t fract2(vec2_t a);
CORE_MATH_FUNC vec3_t fract3(vec3_t a);
CORE_MATH_FUNC vec4_t fract4(vec4_t a);
CORE_MATH_FUNC float diff(float a, float b);
CORE_MATH_FUNC vec2_t diff2(vec2_t a, vec2_t b);
CORE_MATH_FUNC vec3_t diff3(vec3_t a, vec3_t b);
CORE_MATH_FUNC vec4_t diff4(vec4_t a, vec4_t b);
CORE_MATH_FUNC int idiff(int a, int b);
CORE_MATH_FUNC int2_t idiff2(int2_t a, int2_t b);
CORE_MATH_FUNC int3_t idiff3(int3_t a, int3_t b);
CORE_MATH_FUNC int4_t idiff4(int4_t a, int4_t b);
CORE_MATH_FUNC float dot2(vec2_t a, vec2_t b);
CORE_MATH_FUNC float dot3(vec3_t a, vec3_t b);
CORE_MATH_FUNC float dot4(vec4_t a, vec4_t b);
CORE_MATH_FUNC vec3_t cross2(vec2_t a, vec2_t b);
CORE_MATH_FUNC vec3_t cross3(vec3_t a, vec3_t b);
CORE_MATH_FUNC float mix(float a, float b, float t);
CORE_MATH_FUNC vec2_t mix2(vec2_t a, vec2_t b, float t);
CORE_MATH_FUNC vec3_t mix3(vec3_t a, vec3_t b, float t);
CORE_MATH_FUNC vec4_t mix4(vec4_t a, vec4_t b, float t);
CORE_MATH_FUNC float lerp(float a, float b, float t);
CORE_MATH_FUNC vec2_t lerp2(vec2_t a, vec2_t b, float t);
CORE_MATH_FUNC vec3_t lerp3(vec3_t a, vec3_t b, float t);
CORE_MATH_FUNC vec4_t lerp4(vec4_t a, vec4_t b, float t);
CORE_MATH_FUNC float min(float a, float b);
CORE_MATH_FUNC vec2_t min2(vec2_t a, vec2_t b);
CORE_MATH_FUNC vec3_t min3(vec3_t a, vec3_t b);
CORE_MATH_FUNC vec4_t min4(vec4_t a, vec4_t b);
CORE_MATH_FUNC float max(float a, float b);
CORE_MATH_FUNC vec2_t max2(vec2_t a, vec2_t b);
CORE_MATH_FUNC vec3_t max3(vec3_t a, vec3_t b);
CORE_MATH_FUNC vec4_t max4(vec4_t a, vec4_t b);
CORE_MATH_FUNC int ipow(int num, int e);
CORE_MATH_FUNC int32_t smin(int32_t a, int32_t b);
CORE_MATH_FUNC uint32_t umin(uint32_t a, uint32_t b);
CORE_MATH_FUNC int64_t smin64(int64_t a, int64_t b);
CORE_MATH_FUNC uint64_t umin64(uint64_t a, uint64_t b);
CORE_MATH_FUNC int32_t smax(int32_t a, int32_t b);
CORE_MATH_FUNC uint32_t umax(uint32_t a, uint32_t b);
CORE_MATH_FUNC int64_t smax64(int64_t a, int64_t b);
CORE_MATH_FUNC uint64_t umax64(uint64_t a, uint64_t b);
CORE_MATH_FUNC float clamp(float a, float minimum, float maximum);
CORE_MATH_FUNC int iclamp(int a, int minimum, int maximum);
CORE_MATH_FUNC float smoothstep(float x, float y, float a);
CORE_MATH_FUNC float todeg(float rad);
CORE_MATH_FUNC float torad(float deg);
CORE_MATH_FUNC mat4_t mat4_identity();
CORE_MATH_FUNC mat4_t perspective_matrix(float fov, float aspect, float near, float far);
CORE_MATH_FUNC mat4_t mat4_camera(vec3_t position, vec3_t direction, vec3_t up);
CORE_MATH_FUNC mat4_t mat4_inverse(mat4_t m);
CORE_MATH_FUNC mat4_t mat4_mul(mat4_t m1, mat4_t m2);
CORE_MATH_FUNC mat4_t mat4_translate(vec3_t pos);
CORE_MATH_FUNC void mat4_apply_translate(mat4_t *m, vec3_t pos);
CORE_MATH_FUNC mat4_t mat4_rotate_x(float rads);
CORE_MATH_FUNC void mat4_apply_rotate_x(mat4_t *m, float rads);
CORE_MATH_FUNC mat4_t mat4_rotate_y(float rads);
CORE_MATH_FUNC void mat4_apply_rotate_y(mat4_t *m, float rads);
CORE_MATH_FUNC mat4_t mat4_rotate_z(float rads);
CORE_MATH_FUNC void mat4_apply_rotate_z(mat4_t *m, float rads);
CORE_MATH_FUNC mat4_t mat4_scale(vec3_t s);
CORE_MATH_FUNC void mat4_apply_scale(mat4_t *m, vec3_t s);
CORE_MATH_FUNC vec4_t vec4_mul_mat4(vec4_t in, mat4_t mat);
CORE_MATH_FUNC vec3_t vec3_mul_mat4(vec3_t in, mat4_t mat);
CORE_MATH_FUNC vec2_t vec2_mul_mat4(vec2_t in, mat4_t mat);
CORE_MATH_FUNC quaternion_t qidentity();
CORE_MATH_FUNC quaternion_t qmul(quaternion_t q1, quaternion_t q2);
CORE_MATH_FUNC quaternion_t qdiv(quaternion_t q, float f);
CORE_MATH_FUNC float qdot(quaternion_t a, quaternion_t b);
CORE_MATH_FUNC quaternion_t qinverse(quaternion_t q);
CORE_MATH_FUNC void qrotate(quaternion_t *q, vec3_t axis, float angle);
CORE_MATH_FUNC void qrotatevec3_create(vec3_t* v, quaternion_t q);
CORE_MATH_FUNC quaternion_t qnlerp(quaternion_t a, quaternion_t b, float t);
CORE_MATH_FUNC mat4_t qmat4(quaternion_t q);
CORE_MATH_FUNC int randr(int min, int max);
CORE_MATH_FUNC float randf();
CORE_MATH_FUNC float randfr(float min, float max);
CORE_MATH_FUNC float rand2d(vec2_t st);
CORE_MATH_FUNC float noise (vec2_t st);
CORE_MATH_FUNC float fbm (vec2_t st);

#endif


#ifdef CORE_IMPL


// CONSTRUCTORS
CORE_MATH_FUNC int2_t int2(int x, int y) {
	int2_t p = {x, y};
	return p;
}
CORE_MATH_FUNC int2_t point(int x, int y) {
	int2_t p = {x, y};
	return p;
}
CORE_MATH_FUNC int3_t int3(int x, int y, int z) {
	int3_t p = {x, y, z};
	return p;
}
CORE_MATH_FUNC vec2_t vec2(float x, float y) {
	vec2_t a = {x, y};
	return a;
}
CORE_MATH_FUNC vec2_t vec2f(float f) {
	vec2_t a = {f, f};
	return a;
}
CORE_MATH_FUNC vec2_t point_to_vec2(point_t p) {
	vec2_t a = {p.x, p.y};
	return a;
}
CORE_MATH_FUNC vec3_t vec3(float x, float y, float z) {
	vec3_t a = {x, y, z};
	return a;
}
CORE_MATH_FUNC vec3_t vec3f(float f) {
	vec3_t a = {f, f, f};
	return a;
}
CORE_MATH_FUNC vec3_t vec3f2(vec2_t v, float z) {
	vec3_t a = {v.x, v.y, z};
	return a;
}
CORE_MATH_FUNC vec4_t vec4(float x, float y, float z, float w) {
	vec4_t a = {x, y, z, w};
	return a;
}
CORE_MATH_FUNC vec4_t vec4f(float f) {
	vec4_t a = {f, f, f, f};
	return a;
}
CORE_MATH_FUNC vec4_t vec4f2(vec2_t v, float z, float w) {
	vec4_t a = {v.x, v.y, z, w};
	return a;
}
CORE_MATH_FUNC vec4_t vec4f3(vec3_t v, float w) {
	vec4_t a = {v.x, v.y, v.z, w};
	return a;
}


// OPERATIONS
CORE_MATH_FUNC vec2_t add2(vec2_t a, vec2_t b) {
	return vec2(a.x+b.x, a.y+b.y);
}
CORE_MATH_FUNC vec2_t add2f(vec2_t a, float b) {
	return vec2(a.x+b, a.y+b);
}
CORE_MATH_FUNC vec3_t add3(vec3_t a, vec3_t b) {
	vec3_t result = vec3(a.x+b.x, a.y+b.y, a.z+b.z);
	return result;
}
CORE_MATH_FUNC vec3_t add3f(vec3_t a, float b) {
	vec3_t result = vec3(a.x+b, a.y+b, a.z+b);
	return result;
}
CORE_MATH_FUNC vec4_t add4(vec4_t a, vec4_t b) {
	vec4_t result = vec4(a.x+b.x, a.y+b.y, a.z+b.z, a.w+b.w);
	return result;
}
CORE_MATH_FUNC vec4_t add4f(vec4_t a, float b) {
	vec4_t result = vec4(a.x+b, a.y+b, a.z+b, a.w+b);
	return result;
}

CORE_MATH_FUNC vec2_t sub2(vec2_t a, vec2_t b) {
	return vec2(a.x-b.x, a.y-b.y);
}
CORE_MATH_FUNC vec2_t sub2f(vec2_t a, float b) {
	return vec2(a.x-b, a.y-b);
}
CORE_MATH_FUNC vec3_t sub3(vec3_t a, vec3_t b) {
	vec3_t result = vec3(a.x-b.x, a.y-b.y, a.z-b.z);
	return result;
}
CORE_MATH_FUNC vec3_t sub3f(vec3_t a, float b) {
	vec3_t result = vec3(a.x-b, a.y-b, a.z-b);
	return result;
}
CORE_MATH_FUNC vec4_t sub4(vec4_t a, vec4_t b) {
	vec4_t result = vec4(a.x-b.x, a.y-b.y, a.z-b.z, a.w-b.w);
	return result;
}
CORE_MATH_FUNC vec4_t sub4f(vec4_t a, float b) {
	vec4_t result = vec4(a.x-b, a.y-b, a.z-b, a.w-b);
	return result;
}

CORE_MATH_FUNC vec2_t mul2(vec2_t a, vec2_t b) {
	return vec2(a.x*b.x, a.y*b.y);
}
CORE_MATH_FUNC vec2_t mul2f(vec2_t a, float b) {
	return vec2(a.x*b, a.y*b);
}
CORE_MATH_FUNC vec3_t mul3(vec3_t a, vec3_t b) {
	vec3_t result = vec3(a.x*b.x, a.y*b.y, a.z*b.z);
	return result;
}
CORE_MATH_FUNC vec3_t mul3f(vec3_t a, float b) {
	vec3_t result = vec3(a.x*b, a.y*b, a.z*b);
	return result;
}
CORE_MATH_FUNC vec4_t mul4(vec4_t a, vec4_t b) {
	vec4_t result = vec4(a.x*b.x, a.y*b.y, a.z*b.z, a.w*b.w);
	return result;
}
CORE_MATH_FUNC vec4_t mul4f(vec4_t a, float b) {
	vec4_t result = vec4(a.x*b, a.y*b, a.z*b, a.w*b);
	return result;
}

CORE_MATH_FUNC vec2_t div2(vec2_t a, vec2_t b) {
	return vec2(a.x/b.x, a.y/b.y);
}
CORE_MATH_FUNC vec2_t div2f(vec2_t a, float b) {
	return vec2(a.x/b, a.y/b);
}
CORE_MATH_FUNC vec3_t div3(vec3_t a, vec3_t b) {
	vec3_t result = vec3(a.x/b.x, a.y/b.y, a.z/b.z);
	return result;
}
CORE_MATH_FUNC vec3_t div3f(vec3_t a, float b) {
	vec3_t result = vec3(a.x/b, a.y/b, a.z/b);
	return result;
}
CORE_MATH_FUNC vec4_t div4(vec4_t a, vec4_t b) {
	vec4_t result = vec4(a.x/b.x, a.y/b.y, a.z/b.z, a.w/b.w);
	return result;
}
CORE_MATH_FUNC vec4_t div4f(vec4_t a, float b) {
	vec4_t result = vec4(a.x/b, a.y/b, a.z/b, a.w/b);
	return result;
}

CORE_MATH_FUNC float len2(vec2_t a) {
	return (float)sqrt(a.x*a.x + a.y*a.y);
}
CORE_MATH_FUNC float len3(vec3_t a) {
	return (float)sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
}
CORE_MATH_FUNC float len4(vec4_t a) {
	return (float)sqrt(a.x*a.x + a.y*a.y + a.z*a.z + a.w*a.w);
}

CORE_MATH_FUNC vec2_t normalize2(vec2_t v) {
	float l = len2(v);
	return vec2(v.x/l, v.y/l);
}
CORE_MATH_FUNC vec3_t normalize3(vec3_t v) {
	float len = len3(v);
	return vec3(v.x/len, v.y/len, v.z/len);
}
CORE_MATH_FUNC vec4_t normalize4(vec4_t v) {
	float len = len4(v);
	return vec4(v.x/len, v.y/len, v.z/len, v.w/len);
}

// TODO Check crt floor gets inlined nicely, I worry about what it's doing
CORE_MATH_FUNC vec2_t floor2(vec2_t a) {
	return vec2(floorf(a.x), floorf(a.y));
}
CORE_MATH_FUNC vec3_t floor3(vec3_t a) {
	return vec3(floorf(a.x), floorf(a.y), floorf(a.z));
}
CORE_MATH_FUNC vec4_t floor4(vec4_t a) {
	return vec4(floorf(a.x), floorf(a.y), floorf(a.z), floorf(a.w));
}

CORE_MATH_FUNC float fract(float a) {
	return a-floor(a);
}
CORE_MATH_FUNC vec2_t fract2(vec2_t a) {
	vec2_t f = {a.x-floorf(a.x), a.y-floorf(a.y)};
	return f;
}
CORE_MATH_FUNC vec3_t fract3(vec3_t a) {
	vec3_t f = {a.x-floorf(a.x), a.y-floorf(a.y), a.z-floorf(a.z)};
	return f;
}
CORE_MATH_FUNC vec4_t fract4(vec4_t a) {
	vec4_t f = {a.x-floorf(a.x), a.y-floorf(a.y), a.z-floorf(a.z), a.w-floorf(a.w)};
	return f;
}

CORE_MATH_FUNC float diff(float a, float b) {
	return (float)fabs(a-b);
}
CORE_MATH_FUNC vec2_t diff2(vec2_t a, vec2_t b) {
	return vec2(b.x-a.x, b.y-a.y);
}
CORE_MATH_FUNC vec3_t diff3(vec3_t a, vec3_t b) {
	vec3_t result = {b.x-a.x, b.y-a.y, b.z-a.z};
	return result;
}
CORE_MATH_FUNC vec4_t diff4(vec4_t a, vec4_t b) {
	vec4_t result = {b.x-a.x, b.y-a.y, b.z-a.z, b.w-a.w};
	return result;
}

CORE_MATH_FUNC int idiff(int a, int b) {
	return a-b;
}
CORE_MATH_FUNC int2_t idiff2(int2_t a, int2_t b) {
	return int2(b.x-a.x, b.y-a.y);
}
CORE_MATH_FUNC int3_t idiff3(int3_t a, int3_t b) {
	int3_t result = {b.x-a.x, b.y-a.y, b.z-a.z};
	return result;
}
CORE_MATH_FUNC int4_t idiff4(int4_t a, int4_t b) {
	int4_t result = {b.x-a.x, b.y-a.y, b.z-a.z, b.w-a.w};
	return result;
}

CORE_MATH_FUNC float dot2(vec2_t a, vec2_t b) {
	return a.x*b.x + a.y*b.y;
}
CORE_MATH_FUNC float dot3(vec3_t a, vec3_t b) {
	return a.x*b.x + a.y*b.y + a.z*b.z;
}
CORE_MATH_FUNC float dot4(vec4_t a, vec4_t b) {
	return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
}

// Cross Product is only defined in 3D,
// cross2 does a cross product with vec2 by setting z to 0
CORE_MATH_FUNC vec3_t cross2(vec2_t a, vec2_t b) {
	vec3_t result;
	result.x = a.y*0.0f - 0.0f*b.y;
	result.y = 0.0f*b.x - a.x*0.0f;
	result.z = a.x*b.y - a.y*b.x;
	return normalize3(result);
}

// Cross Product is only defined in 3D and 7D spaces apparently
CORE_MATH_FUNC vec3_t cross3(vec3_t a, vec3_t b) {
	vec3_t result;
	result.x = a.y*b.z - a.z*b.y;
	result.y = a.z*b.x - a.x*b.z;
	result.z = a.x*b.y - a.y*b.x;
	return normalize3(result);
}

CORE_MATH_FUNC float mix(float a, float b, float t) {
	return a + (b-a)*t;
}
CORE_MATH_FUNC vec2_t mix2(vec2_t a, vec2_t b, float t) {
	return vec2(mix(a.x, b.x, t),
				mix(a.y, b.y, t));
}
CORE_MATH_FUNC vec3_t mix3(vec3_t a, vec3_t b, float t) {
	return vec3(mix(a.x, b.x, t),
				mix(a.y, b.y, t),
				mix(a.z, b.z, t));
}
CORE_MATH_FUNC vec4_t mix4(vec4_t a, vec4_t b, float t) {
	return vec4(mix(a.x, b.x, t),
				mix(a.y, b.y, t),
				mix(a.z, b.z, t),
				mix(a.w, b.w, t));
}

CORE_MATH_FUNC float lerp(float a, float b, float t) {
	return a + (b-a)*t;
}
CORE_MATH_FUNC vec2_t lerp2(vec2_t a, vec2_t b, float t) {
	return vec2(lerp(a.x, b.x, t),
				lerp(a.y, b.y, t));
}
CORE_MATH_FUNC vec3_t lerp3(vec3_t a, vec3_t b, float t) {
	return vec3(lerp(a.x, b.x, t),
				lerp(a.y, b.y, t),
				lerp(a.z, b.z, t));
}
CORE_MATH_FUNC vec4_t lerp4(vec4_t a, vec4_t b, float t) {
	return vec4(lerp(a.x, b.x, t),
				lerp(a.y, b.y, t),
				lerp(a.z, b.z, t),
				lerp(a.w, b.w, t));
}

CORE_MATH_FUNC float min(float a, float b) {
	return a<b ? a : b;
}
CORE_MATH_FUNC vec2_t min2(vec2_t a, vec2_t b) {
	return vec2(a.x<b.x ? a.x : b.x,
				 a.y<b.y ? a.y : b.y);
}
CORE_MATH_FUNC vec3_t min3(vec3_t a, vec3_t b) {
	return vec3(a.x<b.x ? a.x : b.x,
				 a.y<b.y ? a.y : b.y,
				 a.z<b.z ? a.z : b.z);
}
CORE_MATH_FUNC vec4_t min4(vec4_t a, vec4_t b) {
	return vec4(a.x<b.x ? a.x : b.x,
				a.y<b.y ? a.y : b.y,
				a.z<b.z ? a.z : b.z,
				a.w<b.w ? a.w : b.w);
}

CORE_MATH_FUNC float max(float a, float b) {
	return a>b ? a : b;
}
CORE_MATH_FUNC vec2_t max2(vec2_t a, vec2_t b) {
	return vec2(a.x>b.x ? a.x : b.x,
				a.y>b.y ? a.y : b.y);
}
CORE_MATH_FUNC vec3_t max3(vec3_t a, vec3_t b) {
	return vec3(a.x>b.x ? a.x : b.x,
				a.y>b.y ? a.y : b.y,
				a.z>b.z ? a.z : b.z);
}
CORE_MATH_FUNC vec4_t max4(vec4_t a, vec4_t b) {
	return vec4(a.x>b.x ? a.x : b.x,
				a.y>b.y ? a.y : b.y,
				a.z>b.z ? a.z : b.z,
				a.w>b.w ? a.w : b.w);
}


// UTIL
CORE_MATH_FUNC int ipow(int num, int e) {
	while(e>1) num*=num;
	return num;
}

CORE_MATH_FUNC int32_t smin(int32_t a, int32_t b) {
	return a<b ? a : b;
}
CORE_MATH_FUNC uint32_t umin(uint32_t a, uint32_t b) {
	return a<b ? a : b;
}
CORE_MATH_FUNC int64_t smin64(int64_t a, int64_t b) {
	return a<b ? a : b;
}
CORE_MATH_FUNC uint64_t umin64(uint64_t a, uint64_t b) {
	return a<b ? a : b;
}

CORE_MATH_FUNC int32_t smax(int32_t a, int32_t b) {
	return a>b ? a : b;
}
CORE_MATH_FUNC uint32_t umax(uint32_t a, uint32_t b) {
	return a>b ? a : b;
}
CORE_MATH_FUNC int64_t smax64(int64_t a, int64_t b) {
	return a>b ? a : b;
}
CORE_MATH_FUNC uint64_t umax64(uint64_t a, uint64_t b) {
	return a>b ? a : b;
}

CORE_MATH_FUNC float clamp(float a, float minimum, float maximum) {
	return min(max(a, minimum), maximum);
}
CORE_MATH_FUNC int iclamp(int a, int minimum, int maximum) {
	return smin(smax(a, minimum), maximum);
}

// smoothstep is meant to use Hermite Interpolation
CORE_MATH_FUNC float smoothstep(float x, float y, float a) {
	return clamp((a-x)/(y-x), 0.0, 1.0);
}

CORE_MATH_FUNC float todeg(float rad) {
	return rad/PI * 180.0f;
}
CORE_MATH_FUNC float torad(float deg) {
	return deg/180.0f * PI;
}


// MATRICES
CORE_MATH_FUNC mat4_t mat4_identity() {
	mat4_t result = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1,
	};
	return result;
}

CORE_MATH_FUNC mat4_t perspective_matrix(float fov, float aspect, float near, float far) {
	float f = 1.0f / tanf((fov/180.0f*PI) / 2.0f);
	mat4_t mat = {
		f / aspect, 0, 0, 0,
		0, f, 0, 0,
		0, 0, (far + near) / (near - far), -1,
		0, 0, (2.0f * far * near) / (near - far), 0,
	};
	return mat;
}

CORE_MATH_FUNC mat4_t mat4_camera(vec3_t position, vec3_t direction, vec3_t up) {
	vec3_t x = {0};
	vec3_t y = {0};
	vec3_t z = {0};
	
	vec3_t d = sub3(direction, position);
	
	z = (normalize3(d));
	y = up;
	x = cross3(y, z);
	y = cross3(z, x);
	x = normalize3(x);
	y = normalize3(y);
	
	mat4_t result = {
		-x.x, y.x, -z.x, 0,
		-x.y, y.y, -z.y, 0,
		-x.z, y.z, -z.z, 0,
		-dot3(x, position), -dot3(y, position), dot3(z, position), 1.0f,
	};
	
	return result;
}

CORE_MATH_FUNC mat4_t mat4_inverse(mat4_t m) {
	float augmented[4][8];
	for (int i=0; i<4; ++i) {
		for (int j=0; j<4; ++j) {
			augmented[i][j] = m.f[i*4+j];
		}
		for (int j=4; j<8; ++j) {
			augmented[i][j] = (float)(i==j-4);
		}
	}

	for (int i=0; i<4; ++i) {
		float diag = augmented[i][i];
		if (fabs(diag) < 1e-10) {
#ifdef __CORE_HEADER__
			print_error("Matrix cannot be inversed");
#endif
		}
		for (int j=0; j<8; ++j) {
			augmented[i][j] /= diag;
		}

		for (int j=0; j<4; ++j) {
			if (i!=j) {
				float factor = augmented[j][i];
				for (int k=0; k<8; ++k) {
					augmented[j][k] -= factor * augmented[i][k];
				}
			}
		}
	}

	mat4_t inverse;
	for (int i=0; i<4; ++i) {
		for (int j=0; j<4; ++j) {
			inverse.f[i*4+j] = augmented[i][j+4];
		}
	}

	return inverse;
}

CORE_MATH_FUNC mat4_t mat4_mul(mat4_t m1, mat4_t m2) {
	mat4_t out = {0};
	
	for (int row = 0; row < 4; ++row) {
		for (int col = 0; col < 4; ++col) {
			for (int i = 0; i < 4; ++i) {
				out.f[row*4 + col] += m1.f[row*4 + i] * m2.f[i*4 + col];
			}
		}
	}
	
	return out;
}
CORE_MATH_FUNC mat4_t mat4_translate(vec3_t pos) {
	mat4_t result = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		pos.x, pos.y, pos.z, 1,
	};
	return result;
}
CORE_MATH_FUNC void mat4_apply_translate(mat4_t *m, vec3_t pos) {
	mat4_t result = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		pos.x, pos.y, pos.z, 1,
	};
	*m = mat4_mul(*m, result);
}
CORE_MATH_FUNC mat4_t mat4_rotate_x(float rads) {
	mat4_t result = {
		1, 0,          0,           0,
		0, cosf(rads), -sinf(rads), 0,
		0, sinf(rads), cosf(rads),  0,
		0, 0,          0,           1,
	};
	return result;
}
CORE_MATH_FUNC void mat4_apply_rotate_x(mat4_t *m, float rads) {
	mat4_t result = {
		1, 0,          0,           0,
		0, cosf(rads), -sinf(rads), 0,
		0, sinf(rads), cosf(rads),  0,
		0, 0,          0,           1,
	};
	*m = mat4_mul(*m, result);
}
CORE_MATH_FUNC mat4_t mat4_rotate_y(float rads) {
	mat4_t result = {
		cosf(rads),  0, sinf(rads), 0,
		0,           1, 0,          0,
		-sinf(rads), 0, cosf(rads), 0,
		0,           0, 0,          1,
	};
	return result;
}
CORE_MATH_FUNC void mat4_apply_rotate_y(mat4_t *m, float rads) {
	mat4_t result = {
		cosf(rads),  0, sinf(rads), 0,
		0,           1, 0,          0,
		-sinf(rads), 0, cosf(rads), 0,
		0,           0, 0,          1,
	};
	*m = mat4_mul(*m, result);
}
CORE_MATH_FUNC mat4_t mat4_rotate_z(float rads) {
	mat4_t result = {
		cosf(rads), -sinf(rads), 0, 0,
		sinf(rads), cosf(rads),  0, 0,
		0,          0,           1, 0,
		0,          0,           0, 1,
	};
	return result;
}
CORE_MATH_FUNC void mat4_apply_rotate_z(mat4_t *m, float rads) {
	mat4_t result = {
		cosf(rads), -sinf(rads), 0, 0,
		sinf(rads), cosf(rads),  0, 0,
		0,          0,           1, 0,
		0,          0,           0, 1,
	};
	*m = mat4_mul(*m, result);
}
CORE_MATH_FUNC mat4_t mat4_scale(vec3_t s) {
	mat4_t result = {
		s.x, 0, 0, 0,
		0, s.y, 0, 0,
		0, 0, s.z, 0,
		0, 0, 0, 1,
	};
	return result;
}
CORE_MATH_FUNC void mat4_apply_scale(mat4_t *m, vec3_t s) {
	mat4_t result = {
		s.x, 0, 0, 0,
		0, s.y, 0, 0,
		0, 0, s.z, 0,
		0, 0, 0, 1,
	};
	*m = mat4_mul(*m, result);
}

CORE_MATH_FUNC vec4_t vec4_mul_mat4(vec4_t in, mat4_t mat) {
	vec4_t result = {0};
	for (int col = 0; col < 4; ++col) {
		for (int i = 0; i < 4; ++i) {
			result.f[col] += in.f[i] * mat.f[i*4 + col];
		}
	}
	return result;
}
CORE_MATH_FUNC vec3_t vec3_mul_mat4(vec3_t in, mat4_t mat) {
	vec4_t vec = vec4f3(in, 1);
	vec4_t result = {0};
	for (int col = 0; col < 4; ++col) {
		for (int i = 0; i < 4; ++i) {
			result.f[col] += vec.f[i] * mat.f[i*4 + col];
		}
	}
	return result.xyz;
}
CORE_MATH_FUNC vec2_t vec2_mul_mat4(vec2_t in, mat4_t mat) {
	vec4_t vec = vec4f2(in, 0, 1);
	vec4_t result = {0};
	for (int col = 0; col < 4; ++col) {
		for (int i = 0; i < 4; ++i) {
			result.f[col] += vec.f[i] * mat.f[i*4 + col];
		}
	}
	return result.xy;
}

// QUATERNIONS
CORE_MATH_FUNC quaternion_t qidentity() {
	quaternion_t result = {0.0f, 0.0f, 0.0f, 1.0f};
	return result;
}
CORE_MATH_FUNC quaternion_t qmul(quaternion_t q1, quaternion_t q2) {
	quaternion_t q;
	q.w = q1.w*q2.w - q1.x*q2.x - q1.y*q2.y - q1.z*q2.z;
	q.x = q1.w*q2.x + q1.x*q2.w + q1.y*q2.z - q1.z*q2.y;
	q.y = q1.w*q2.y - q1.x*q2.z + q1.y*q2.w + q1.z*q2.x;
	q.z = q1.w*q2.z + q1.x*q2.y - q1.y*q2.x + q1.z*q2.w;
	return q;
}
CORE_MATH_FUNC quaternion_t qdiv(quaternion_t q, float f) {
	quaternion_t result;
	result.x = q.x / f;
	result.y = q.y / f;
	result.z = q.z / f;
	result.w = q.w / f;
	return result;
}
CORE_MATH_FUNC float qdot(quaternion_t a, quaternion_t b) {
	return (a.x*b.x) + (a.y*b.y) + (a.z*b.z) + (a.w*b.w);
}
CORE_MATH_FUNC quaternion_t qinverse(quaternion_t q) {
	quaternion_t result;
	result.x = -q.x;
	result.y = -q.y;
	result.z = -q.z;
	result.w = q.w;
	return qdiv(result, qdot(q,q));
}
CORE_MATH_FUNC void qrotate(quaternion_t *q, vec3_t axis, float angle) {
	quaternion_t local;
	local.w = cosf(angle/2.0f);
	local.x = axis.x * sinf(angle/2.0f);
	local.y = axis.y * sinf(angle/2.0f);
	local.z = axis.z * sinf(angle/2.0f);
	
	*q = qmul(local, *q);
	*q = normalize4(*q); // TODO: check if it needs this first
}
CORE_MATH_FUNC void qrotatevec3_create(vec3_t* v, quaternion_t q) {
	quaternion_t qp = {v->x, v->y, v->z, 0};
	quaternion_t temp = qmul(q, qp);
	q = qmul(temp, vec4(-q.x, -q.y, -q.z, q.w));
	*v = q.xyz;
}
CORE_MATH_FUNC quaternion_t qnlerp(quaternion_t a, quaternion_t b, float t) {
	quaternion_t result;
	result.x = lerp(a.x, b.x, t);
	result.y = lerp(a.y, b.y, t);
	result.z = lerp(a.z, b.z, t);
	result.w = lerp(a.w, b.w, t);
	return normalize4(result);
}
CORE_MATH_FUNC mat4_t qmat4(quaternion_t q) {
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


// NOISE
CORE_MATH_FUNC int randr(int min, int max) {
	int result = rand() % (max-min);
	return min + result;
}
CORE_MATH_FUNC float randf() {
	return (float)rand() / RAND_MAX;
}
CORE_MATH_FUNC float randfr(float min, float max) {
	return min + randf()*(max-min);
}
CORE_MATH_FUNC float rand2d(vec2_t st) {
    return fract((float)sin(dot2(st, vec2(12.9898f,78.233f)))*43758.5453123f);
}
CORE_MATH_FUNC float noise (vec2_t st) {
    vec2_t i = floor2(st);
    vec2_t f = fract2(st);
	
    // Four corners in 2D of a tile
    float a = rand2d(i);
    float b = rand2d(add2(i, vec2(1.0, 0.0)));
    float c = rand2d(add2(i, vec2(0.0, 1.0)));
    float d = rand2d(add2(i, vec2(1.0, 1.0)));
	
	vec2_t u = f;
	
	return mix(mix(a, b, u.x), mix(c, d, u.x), u.y);
}
CORE_MATH_FUNC float fbm (vec2_t st) {
    float value = 0.0;
    float amplitude = .5;
    float frequency = 0.;
    int o;

#define OCTAVES 6
    {for (o=0; o<OCTAVES; o++) {
        value += amplitude * noise(st);
        st = mul2(st, vec2(2,2));
        amplitude *= .5;
    }}

    return value;
}


#endif
