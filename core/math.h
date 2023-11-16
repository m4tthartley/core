//
//  math.h
//  Core
//
//  Created by Matt Hartley on 26/10/2023.
//  Copyright 2023 GiantJelly. All rights reserved.
//

#include <stdlib.h>
#include <math.h>

#ifndef __CORE_MATH_HEADER__
#define __CORE_MATH_HEADER__

typedef __int64  i64;
typedef unsigned __int64  u64;
typedef __int32  i32;
typedef unsigned __int32  u32;

#undef min
#undef max
#undef near
#undef far

#define PI 3.14159265359f
#define PI2 (PI*2.0f)

#define CORE_INLINE 
// __attribute__((always_inline))

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
} int3_t;

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
	vec3_t xyz;
	vec3_t rgb;
	vec2_t xy;
	float f[4];
} vec4_t;

typedef vec4_t quaternion_t;
typedef quaternion_t quat_t;

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

/*int align(int n, int stride);
int iRoundUp(int n, int stride);
int iRoundDown(int n, int stride);

point _point(int x, int y);
vec2_t vec2_create(float x, float y);
vec2_t add2(vec2_t a, vec2_t b);
vec2_t mul2(vec2_t a, vec2_t b);
vec2_t sub2(vec2_t a, vec2_t b);
vec2_t normalize2(vec2_t v);
float len2(vec2_t a);
vec2_t floor2(vec2_t a);
vec2_t fract2(vec2_t a);
float dot2(vec2_t a, vec2_t b);

vec3_t vec3_create(float x, float y, float z);
vec3_t mix3(vec3_t a, vec3_t b, float t);
vec3_t add3(vec3_t a, vec3_t b);
vec3_t sub3(vec3_t a, vec3_t b);
vec3_t mul3(vec3_t a, vec3_t b);
vec3_t div3(vec3_t a, vec3_t b);

vec3_t diff3(vec3_t a, vec3_t b);
vec3_t normalize3(vec3_t v);
vec3_t cross3(vec3_t a, vec3_t b);

int ipow(int num, int e);
float randf();
float randfr(float min, float max);
float min(float a, float b);
float max(float a, float b);
int mini(int a, int b);
int maxi(int a, int b);
float diff(float a, float b);
float len(float x, float y);
float clamp(float a, float minimum, float maximum);
int clampi(int a, int minimum, int maximum);
float smoothstep(float x, float y, float a);
float mix(float x, float y, float a);
float fract(float a);

mat4 perspective_matrix(float fov, float aspect, float near, float far);

quaternion qidentity();
quaternion qmul(quaternion q1, quaternion q2);
void qrotate(quaternion *q, vec3_t axis, float angle);

float rand2d(vec2_t st);
float noise (vec2_t st);
float fbm (vec2_t st);*/

//#ifndef _MATH_DECLARATIONS
//#undef  _MATH_DECLARATIONS


CORE_INLINE int2_t int2(int x, int y);
CORE_INLINE int2_t point(int x, int y);
CORE_INLINE int3_t int3(int x, int y, int z);
CORE_INLINE vec2_t vec2(float x, float y);
CORE_INLINE vec2_t point_to_vec2(point_t p);
CORE_INLINE vec3_t vec3(float x, float y, float z);
CORE_INLINE vec3_t vec3f2(vec2_t v, float z);
CORE_INLINE vec4_t vec4(float x, float y, float z, float w);
CORE_INLINE vec4_t vec4f2(vec2_t v, float z, float w);
CORE_INLINE vec4_t vec4f3(vec3_t v, float w);
CORE_INLINE vec2_t add2(vec2_t a, vec2_t b);
CORE_INLINE vec2_t add2f(vec2_t a, float b);
CORE_INLINE vec3_t add3(vec3_t a, vec3_t b);
CORE_INLINE vec3_t add3f(vec3_t a, float b);
CORE_INLINE vec4_t add4(vec4_t a, vec4_t b);
CORE_INLINE vec4_t add4f(vec4_t a, float b);
CORE_INLINE vec2_t sub2(vec2_t a, vec2_t b);
CORE_INLINE vec2_t sub2f(vec2_t a, float b);
CORE_INLINE vec3_t sub3(vec3_t a, vec3_t b);
CORE_INLINE vec3_t sub3f(vec3_t a, float b);
CORE_INLINE vec4_t sub4(vec4_t a, vec4_t b);
CORE_INLINE vec4_t sub4f(vec4_t a, float b);
CORE_INLINE vec2_t mul2(vec2_t a, vec2_t b);
CORE_INLINE vec2_t mul2f(vec2_t a, float b);
CORE_INLINE vec3_t mul3(vec3_t a, vec3_t b);
CORE_INLINE vec3_t mul3f(vec3_t a, float b);
CORE_INLINE vec4_t mul4(vec4_t a, vec4_t b);
CORE_INLINE vec4_t mul4f(vec4_t a, float b);
CORE_INLINE vec2_t div2(vec2_t a, vec2_t b);
CORE_INLINE vec2_t div2f(vec2_t a, float b);
CORE_INLINE vec3_t div3(vec3_t a, vec3_t b);
CORE_INLINE vec3_t div3f(vec3_t a, float b);
CORE_INLINE vec4_t div4(vec4_t a, vec4_t b);
CORE_INLINE vec4_t div4f(vec4_t a, float b);
CORE_INLINE float len2(vec2_t a);
CORE_INLINE float len3(vec3_t a);
CORE_INLINE float len4(vec4_t a);
CORE_INLINE vec2_t normalize2(vec2_t v);
CORE_INLINE vec3_t normalize3(vec3_t v);
CORE_INLINE vec4_t normalize4(vec4_t v);
CORE_INLINE vec2_t floor2(vec2_t a);
CORE_INLINE vec3_t floor3(vec3_t a);
CORE_INLINE vec4_t floor4(vec4_t a);
CORE_INLINE float fract(float a);
CORE_INLINE vec2_t fract2(vec2_t a);
CORE_INLINE vec3_t fract3(vec3_t a);
CORE_INLINE vec4_t fract4(vec4_t a);
CORE_INLINE float diff(float a, float b);
CORE_INLINE vec2_t diff2(vec2_t a, vec2_t b);
CORE_INLINE vec3_t diff3(vec3_t a, vec3_t b);
CORE_INLINE vec4_t diff4(vec4_t a, vec4_t b);
CORE_INLINE float dot2(vec2_t a, vec2_t b);
CORE_INLINE float dot3(vec3_t a, vec3_t b);
CORE_INLINE float dot4(vec4_t a, vec4_t b);
CORE_INLINE vec3_t cross3(vec3_t a, vec3_t b);
CORE_INLINE float mix(float a, float b, float t);
CORE_INLINE vec2_t mix2(vec2_t a, vec2_t b, float t);
CORE_INLINE vec3_t mix3(vec3_t a, vec3_t b, float t);
CORE_INLINE vec4_t mix4(vec4_t a, vec4_t b, float t);
CORE_INLINE float lerp(float a, float b, float t);
CORE_INLINE vec2_t lerp2(vec2_t a, vec2_t b, float t);
CORE_INLINE vec3_t lerp3(vec3_t a, vec3_t b, float t);
CORE_INLINE vec4_t lerp4(vec4_t a, vec4_t b, float t);
CORE_INLINE float min(float a, float b);
CORE_INLINE vec2_t min2(vec2_t a, vec2_t b);
CORE_INLINE vec3_t min3(vec3_t a, vec3_t b);
CORE_INLINE vec4_t min4(vec4_t a, vec4_t b);
CORE_INLINE float max(float a, float b);
CORE_INLINE vec2_t max2(vec2_t a, vec2_t b);
CORE_INLINE vec3_t max3(vec3_t a, vec3_t b);
CORE_INLINE vec4_t max4(vec4_t a, vec4_t b);
CORE_INLINE int ipow(int num, int e);
CORE_INLINE i32 smin(i32 a, i32 b);
CORE_INLINE u32 umin(u32 a, u32 b);
CORE_INLINE i64 smin64(i64 a, i64 b);
CORE_INLINE u64 umin64(u64 a, u64 b);
CORE_INLINE i32 smax(i32 a, i32 b);
CORE_INLINE u32 umax(u32 a, u32 b);
CORE_INLINE i64 smax64(i64 a, i64 b);
CORE_INLINE u64 umax64(u64 a, u64 b);
CORE_INLINE float clamp(float a, float minimum, float maximum);
CORE_INLINE int iclamp(int a, int minimum, int maximum);
CORE_INLINE float smoothstep(float x, float y, float a);
CORE_INLINE float todeg(float rad);
CORE_INLINE float torad(float deg);
CORE_INLINE mat4_t mat4_identity();
CORE_INLINE mat4_t mat4_translation(vec3_t pos);
CORE_INLINE mat4_t perspective_matrix(float fov, float aspect, float near, float far);
CORE_INLINE mat4_t mat4_camera(vec3_t position, vec3_t direction, vec3_t up);
CORE_INLINE mat4_t mat4_mul(mat4_t m1, mat4_t m2);
CORE_INLINE void mat4_translate(mat4_t *m, vec3_t pos);
CORE_INLINE void mat4_rotate_x(mat4_t *m, float rads);
CORE_INLINE void mat4_rotate_y(mat4_t *m, float rads);
CORE_INLINE void mat4_rotate_z(mat4_t *m, float rads);
CORE_INLINE quaternion_t qidentity();
CORE_INLINE quaternion_t qmul(quaternion_t q1, quaternion_t q2);
CORE_INLINE quaternion_t qdiv(quaternion_t q, float f);
CORE_INLINE float qdot(quaternion_t a, quaternion_t b);
CORE_INLINE quaternion_t qinverse(quaternion_t q);
CORE_INLINE void qrotate(quaternion_t *q, vec3_t axis, float angle);
CORE_INLINE void qrotatevec3_create(vec3_t* v, quaternion_t q);
CORE_INLINE quaternion_t qnlerp(quaternion_t a, quaternion_t b, float t);
CORE_INLINE mat4_t qmat4(quaternion_t q);
CORE_INLINE int randr(int min, int max);
CORE_INLINE float randf();
CORE_INLINE float randfr(float min, float max);
CORE_INLINE float rand2d(vec2_t st);
CORE_INLINE float noise (vec2_t st);
CORE_INLINE float fbm (vec2_t st);

#endif
