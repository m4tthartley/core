#include <STDLIB.H>
#include <MATH.H>

#ifndef _MATH_C
#define _MATH_C

#undef min
#undef max
#undef near
#undef far

#define PI 3.14159265359f
#define PI2 (PI*2.0f)

#ifndef MATH_LEGACY_NAMING
#	define vec2_create vec2
#	define vec3_create vec3
#	define vec4_create vec4
#else
#	define vec2_create vec2_create
#	define vec3_create vec3_create
#	define vec4_create vec4_create
#	define vec2_t vec2
#	define vec3_t vec3
#	define vec4_t vec4
#endif
// #	define GFX_VEC2_STRUCT_NAME vec2_t
// #	define GFX_VEC3_STRUCT_NAME vec3_t
// #	define GFX_VEC4_STRUCT_NAME vec4_t
// #	define GFX_VEC2_CREATE_NAME vec2
/* #	define GFX_VEC3_CREATE_NAME vec3
#	define GFX_VEC4_CREATE_NAME vec4
#else
#	define GFX_VEC2_STRUCT_NAME vec2
#	define GFX_VEC3_STRUCT_NAME vec3
#	define GFX_VEC4_STRUCT_NAME vec4
#	define GFX_VEC2_CREATE_NAME vec2_create
#	define GFX_VEC3_CREATE_NAME vec3_create
#	define GFX_VEC4_CREATE_NAME vec4_create
#endif */

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
} int2;
typedef int2 point;

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
	int2 xy;
} int3;

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
	vec2_t xy;
	float f[4];
} vec4_t;
typedef vec4_t quaternion;
typedef quaternion quat;

/*typedef struct vec2_t {
	float x;
	float y;
} vec2;
typedef struct vec3_t {
	float x;
	float y;
	float z;
} vec3;*/

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
} mat4;

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

#define math_inline 

// CONSTRUCTORS
math_inline int2 _int2(int x, int y) {
	int2 p = {x, y};
	return p;
}
math_inline int2 _point(int x, int y) {
	int2 p = {x, y};
	return p;
}
math_inline int3 _int3(int x, int y, int z) {
	int3 p = {x, y, z};
	return p;
}
math_inline vec2_t vec2_create(float x, float y) {
	vec2_t a = {x, y};
	return a;
}
math_inline vec2_t _pointToVec2(point p) {
	vec2_t a = {p.x, p.y};
	return a;
}
math_inline vec3_t vec3_create(float x, float y, float z) {
	vec3_t a = {x, y, z};
	return a;
}
math_inline vec3_t vec2_createto3(vec2_t v, float z) {
	vec3_t a = {v.x, v.y, z};
	return a;
}
math_inline vec4_t vec4_create(float x, float y, float z, float w) {
	vec4_t a = {x, y, z, w};
	return a;
}
math_inline vec4_t vec3_createto4(vec3_t v, float w) {
	vec4_t a = {v.x, v.y, v.z, w};
	return a;
}

// OPERATIONS
math_inline vec2_t add2(vec2_t a, vec2_t b) {
	return vec2_create(a.x+b.x, a.y+b.y); // todo: make sure these get inlined
}
math_inline vec2_t add2f(vec2_t a, float b) {
	return vec2_create(a.x+b, a.y+b);
}
math_inline vec3_t add3(vec3_t a, vec3_t b) {
	vec3_t result = vec3_create(a.x+b.x, a.y+b.y, a.z+b.z);
	return result;
}
math_inline vec3_t add3f(vec3_t a, float b) {
	vec3_t result = vec3_create(a.x+b, a.y+b, a.z+b);
	return result;
}

math_inline vec2_t sub2(vec2_t a, vec2_t b) {
	return vec2_create(a.x-b.x, a.y-b.y);
}
math_inline vec2_t sub2f(vec2_t a, float b) {
	return vec2_create(a.x-b, a.y-b);
}
math_inline vec3_t sub3(vec3_t a, vec3_t b) {
	vec3_t result = vec3_create(a.x-b.x, a.y-b.y, a.z-b.z);
	return result;
}
math_inline vec3_t sub3f(vec3_t a, float b) {
	vec3_t result = vec3_create(a.x-b, a.y-b, a.z-b);
	return result;
}

math_inline vec2_t mul2(vec2_t a, vec2_t b) {
	return vec2_create(a.x*b.x, a.y*b.y);
}
math_inline vec2_t mul2f(vec2_t a, float b) {
	return vec2_create(a.x*b, a.y*b);
}
math_inline vec3_t mul3(vec3_t a, vec3_t b) {
	vec3_t result = vec3_create(a.x*b.x, a.y*b.y, a.z*b.z);
	return result;
}
math_inline vec3_t mul3f(vec3_t a, float b) {
	vec3_t result = vec3_create(a.x*b, a.y*b, a.z*b);
	return result;
}

math_inline vec2_t div2(vec2_t a, vec2_t b) {
	return vec2_create(a.x/b.x, a.y/b.y);
}
math_inline vec2_t div2f(vec2_t a, float b) {
	return vec2_create(a.x/b, a.y/b);
}
math_inline vec3_t div3(vec3_t a, vec3_t b) {
	vec3_t result = vec3_create(a.x/b.x, a.y/b.y, a.z/b.z);
	return result;
}
math_inline vec3_t div3f(vec3_t a, float b) {
	vec3_t result = vec3_create(a.x/b, a.y/b, a.z/b);
	return result;
}

math_inline float len2(vec2_t a) {
	return (float)sqrt(a.x*a.x + a.y*a.y);
}
math_inline float len3(vec3_t a) {
	return (float)sqrt(a.x*a.x + a.y*a.y + a.z*a.z);
}
math_inline float len4(vec4_t a) {
	return (float)sqrt(a.x*a.x + a.y*a.y + a.z*a.z + a.w*a.w);
}

math_inline vec2_t normalize2(vec2_t v) {
	float l = len2(v);
	return vec2_create(v.x/l, v.y/l);
}
math_inline vec3_t normalize3(vec3_t v) {
	float len = len3(v);
	return vec3_create(v.x/len, v.y/len, v.z/len);
}
math_inline vec4_t normalize4(vec4_t v) {
	float len = len4(v);
	return vec4_create(v.x/len, v.y/len, v.z/len, v.w/len);
}

math_inline vec2_t floor2(vec2_t a) {
	return vec2_create(floorf(a.x), floorf(a.y));
}
math_inline vec3_t floor3(vec3_t a) {
	return vec3_create(floorf(a.x), floorf(a.y), floorf(a.z));
}

math_inline float fract(float a) {
	return a-floor(a);
}
math_inline vec2_t fract2(vec2_t a) {
	vec2_t f = {a.x-floorf(a.x), a.y-floorf(a.y)};
	return f;
}
math_inline vec3_t fract3(vec3_t a) {
	vec3_t f = {a.x-floorf(a.x), a.y-floorf(a.y), a.z-floorf(a.z)};
	return f;
}

math_inline float diff(float a, float b) {
	return (float)fabs(a-b);
}
math_inline vec2_t diff2(vec2_t a, vec2_t b) {
	return vec2_create(b.x-a.x, b.y-a.y);
}
math_inline vec3_t diff3(vec3_t a, vec3_t b) {
	vec3_t result = {b.x-a.x, b.y-a.y, b.z-a.z};
	return result;
}

math_inline float dot2(vec2_t a, vec2_t b) {
	return a.x*b.x + a.y*b.y;
}
math_inline float dot3(vec3_t a, vec3_t b) {
	return a.x*b.x + a.y*b.y + a.z*b.z;
}
math_inline float dot4(vec4_t a, vec4_t b) {
	return a.x*b.x + a.y*b.y + a.z*b.z + a.w*b.w;
}

// math_inline vec2_t cross2(vec2_t a, vec2_t b) {
// 	vec2_t result;
// 	result.x = a.y*b.z - a.z*b.y;
// 	result.y = a.z*b.x - a.x*b.z;
// 	return normalize2(result);
// }
math_inline vec3_t cross3(vec3_t a, vec3_t b) {
	vec3_t result;
	result.x = a.y*b.z - a.z*b.y;
	result.y = a.z*b.x - a.x*b.z;
	result.z = a.x*b.y - a.y*b.x;
	return normalize3(result);
}

/*
vec3_t mix3(vec3_t a, vec3_t b, float t) {
	vec3_t result = vec3_create(a.x + (b.x-a.x)*t, a.y + (b.y-a.y)*t, a.z + (b.z-a.z)*t);
	return result;
}*/

math_inline float mix(float a, float b, float t) {
	return a + (b-a)*t;
}
math_inline float lerp(float a, float b, float t) {
	return a + (b-a)*t;
}
math_inline vec2_t lerp2(vec2_t a, vec2_t b, float t) {
	return vec2_create(lerp(a.x, b.x, t),
				 lerp(a.y, b.y, t));
}
math_inline vec3_t lerp3(vec3_t a, vec3_t b, float t) {
	return vec3_create(lerp(a.x, b.x, t),
				 lerp(a.y, b.y, t),
				 lerp(a.z, b.z, t));
}
math_inline vec4_t lerp4(vec4_t a, vec4_t b, float t) {
	return vec4_create(lerp(a.x, b.x, t),
				 lerp(a.y, b.y, t),
				 lerp(a.z, b.z, t),
				 lerp(a.w, b.w, t));
}

// UTIL
math_inline int ipow(int num, int e) {
	while(e>1) num*=num;
	return num;
}

math_inline float min(float a, float b) {
	return a<b ? a : b;
}
math_inline vec2_t min2(vec2_t a, vec2_t b) {
	return vec2_create(a.x<b.x ? a.x : b.x,
				 a.y<b.y ? a.y : b.y);
}
math_inline vec3_t min3(vec3_t a, vec3_t b) {
	return vec3_create(a.x<b.x ? a.x : b.x,
				 a.y<b.y ? a.y : b.y,
				 a.z<b.z ? a.z : b.z);
}
math_inline float max(float a, float b) {
	return a>b ? a : b;
}
math_inline vec2_t max2(vec2_t a, vec2_t b) {
	return vec2_create(a.x>b.x ? a.x : b.x,
				 a.y>b.y ? a.y : b.y);
}
math_inline vec3_t max3(vec3_t a, vec3_t b) {
	return vec3_create(a.x>b.x ? a.x : b.x,
				 a.y>b.y ? a.y : b.y,
				 a.z>b.z ? a.z : b.z);
}
math_inline int imin(int a, int b) {
	return a<b ? a : b;
}
math_inline int imax(int a, int b) {
	return a>b ? a : b;
}

math_inline float clamp(float a, float minimum, float maximum) {
	return min(max(a, minimum), maximum);
}
math_inline int iclamp(int a, int minimum, int maximum) {
	return imin(imax(a, minimum), maximum);
}

math_inline float smoothstep(float x, float y, float a) {
	return clamp((a-x)/(y-x), 0.0, 1.0);
}

math_inline int align(int n, int stride) {
	return (n/stride + 1)*stride;
}
/*int roundUp(int n, int stride) {
	return (n/stride + 1)*stride;
}
int roundDown(int n, int stride) {
	return (n/stride)*stride;
}*/

math_inline float todeg(float rad) {
	return rad/PI * 180.0f;
}
math_inline float torad(float deg) {
	return deg/180.0f * PI;
}

// MATRICES
math_inline mat4 mat4_identity() {
	mat4 result = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1,
	};
	return result;
}
math_inline mat4 mat4_mul(mat4 m1, mat4 m2) {
	mat4 out = {0};
	
	for (int row = 0; row < 4; ++row) {
		for (int col = 0; col < 4; ++col) {
			for (int i = 0; i < 4; ++i) {
				out.f[row*4 + col] += m1.f[row*4 + i] * m2.f[i*4 + col];
			}
		}
	}
	
	return out;
}
math_inline void mat4_translate(mat4 *m, vec3_t pos) {
	mat4 result = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		pos.x, pos.y, pos.z, 1,
	};
	*m = mat4_mul(*m, result);
}

math_inline mat4 mat4_translation(vec3_t pos) {
	mat4 result = {
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		pos.x, pos.y, pos.z, 1,
	};
	return result;
}

math_inline void mat4_rotate_x(mat4 *m, float rads) {
	mat4 result = {
		1, 0,          0,           0,
		0, cosf(rads), -sinf(rads), 0,
		0, sinf(rads), cosf(rads),  0,
		0, 0,          0,           1,
	};
	*m = mat4_mul(*m, result);
}
math_inline void mat4_rotate_y(mat4 *m, float rads) {
	mat4 result = {
		cosf(rads),  0, sinf(rads), 0,
		0,           1, 0,          0,
		-sinf(rads), 0, cosf(rads), 0,
		0,           0, 0,          1,
	};
	*m = mat4_mul(*m, result);
}
math_inline void mat4_rotate_z(mat4 *m, float rads) {
	mat4 result = {
		cosf(rads), -sinf(rads), 0, 0,
		sinf(rads), cosf(rads),  0, 0,
		0,          0,           1, 0,
		0,          0,           0, 1,
	};
	*m = mat4_mul(*m, result);
}
math_inline mat4 perspective_matrix(float fov, float aspect, float near, float far) {
	float f = 1.0f / tanf((fov/180.0f*PI) / 2.0f);
	mat4 mat = {
		f / aspect, 0, 0, 0,
		0, f, 0, 0,
		0, 0, (far + near) / (near - far), -1,
		0, 0, (2.0f * far * near) / (near - far), 0,
	};
	return mat;
}
math_inline mat4 mat4_camera(vec3_t position, vec3_t direction, vec3_t up) {
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
	
	mat4 result = {
		-x.x, y.x, -z.x, 0,
		-x.y, y.y, -z.y, 0,
		-x.z, y.z, -z.z, 0,
		-dot3(x, position), -dot3(y, position), dot3(z, position), 1.0f,
	};
	
	return result;
}
vec4_t vec4_mul_mat4(vec4_t in, mat4 mat) {
	vec4_t result = {0};
	for (int col = 0; col < 4; ++col) {
		for (int i = 0; i < 4; ++i) {
			result.f[col] += in.f[i] * mat.f[i*4 + col];
		}
	}
	return result;
}

// QUATERNIONS
math_inline quaternion qidentity() {
	quaternion result = {0.0f, 0.0f, 0.0f, 1.0f};
	return result;
}
math_inline quaternion qmul(quaternion q1, quaternion q2) {
	quaternion q;
	q.w = q1.w*q2.w - q1.x*q2.x - q1.y*q2.y - q1.z*q2.z;
	q.x = q1.w*q2.x + q1.x*q2.w + q1.y*q2.z - q1.z*q2.y;
	q.y = q1.w*q2.y - q1.x*q2.z + q1.y*q2.w + q1.z*q2.x;
	q.z = q1.w*q2.z + q1.x*q2.y - q1.y*q2.x + q1.z*q2.w;
	return q;
}
math_inline quaternion qdiv(quaternion q, float f) {
	quaternion result;
	result.x = q.x / f;
	result.y = q.y / f;
	result.z = q.z / f;
	result.w = q.w / f;
	return result;
}
math_inline float qdot(quaternion a, quaternion b) {
	return (a.x*b.x) + (a.y*b.y) + (a.z*b.z) + (a.w*b.w);
}
math_inline quaternion qinverse(quaternion q) {
	quaternion result;
	result.x = -q.x;
	result.y = -q.y;
	result.z = -q.z;
	result.w = q.w;
	return qdiv(result, qdot(q,q));
}
math_inline void qrotate(quaternion *q, vec3_t axis, float angle) {
	quaternion local;
	local.w = cosf(angle/2.0f);
	local.x = axis.x * sinf(angle/2.0f);
	local.y = axis.y * sinf(angle/2.0f);
	local.z = axis.z * sinf(angle/2.0f);
	
	*q = qmul(local, *q);
	*q = normalize4(*q); // todo: check if it needs this first
}
math_inline void qrotatevec3_create(vec3_t* v, quaternion q) {
	quaternion qp = {v->x, v->y, v->z, 0};
	quaternion temp = qmul(q, qp);
	q = qmul(temp, vec4_create(-q.x, -q.y, -q.z, q.w));
	*v = q.xyz;
}
math_inline quaternion qnlerp(quaternion a, quaternion b, float t) {
	quaternion result;
	result.x = lerp(a.x, b.x, t);
	result.y = lerp(a.y, b.y, t);
	result.z = lerp(a.z, b.z, t);
	result.w = lerp(a.w, b.w, t);
	return normalize4(result);
}
math_inline mat4 qmat4(quaternion q) {
	float len = sqrtf(q.x*q.x + q.y*q.y + q.z*q.z + q.w*q.w);
	float x = q.x / len;
	float y = q.y / len;
	float z = q.z / len;
	float w = q.w / len;

	/*mat4 result = {
		1 - 2*y*y - 2*z*z, 2*x*y - 2*w*z,     2*x*z + 2*w*y,     0,
		2*x*y + 2*w*z,     1 - 2*x*x - 2*z*z, 2*y*z + 2*w*x,     0,
		2*x*z - 2*w*y,     2*y*z - 2*w*x,     1 - 2*x*x - 2*y*y, 0,
		0, 0, 0, 1,
	};*/
	mat4 result;
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
math_inline float randf() {
	return (float)rand() / RAND_MAX;
}
math_inline float randfr(float min, float max) {
	return min + randf()*(max-min);
}
math_inline float rand2d(vec2_t st) {
    return fract((float)sin(dot2(st, vec2_create(12.9898f,78.233f)))*43758.5453123f);
}
math_inline float noise (vec2_t st) {
    vec2_t i = floor2(st);
    vec2_t f = fract2(st);
	
    // Four corners in 2D of a tile
    float a = rand2d(i);
    float b = rand2d(add2(i, vec2_create(1.0, 0.0)));
    float c = rand2d(add2(i, vec2_create(0.0, 1.0)));
    float d = rand2d(add2(i, vec2_create(1.0, 1.0)));
	
    //vec2_t u = vec2Mul(f, vec2Mul(f, (vec2Sub(vec2_create(3.0,3.0), vec2Mul(vec2_create(2.0,2.0), f)))));
	vec2_t u = f;
	//return a;
	
    //return mix(a, b, u.x) + (c - a)* u.y * (1.0 - u.x) + (d - b) * u.x * u.y;
	return mix(mix(a, b, u.x), mix(c, d, u.x), u.y);
}
math_inline float fbm (vec2_t st) {
    float value = 0.0;
    float amplitude = .5;
    float frequency = 0.;
    int o;

#define OCTAVES 6
    {for (o=0; o<OCTAVES; o++) {
        value += amplitude * noise(st);
        st = mul2(st, vec2_create(2,2));
        amplitude *= .5;
    }}

    return value;
	// return rand2d(vec2_create(-1.5, -1.0));
	// return noise(st);
}


//#endif

#endif
