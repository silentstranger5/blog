#ifndef GLM

#define GLM

#include <math.h>
#include <stdlib.h>
#include <string.h>

typedef int  ivec2[2];
typedef int  ivec3[3];
typedef float vec2[2];
typedef float vec3[3];
typedef float vec4[4];
typedef vec2 mat2[2];
typedef vec3 mat3[3];
typedef vec4 mat4[4];

#define PI 3.14159265F
#define swap(t, x, y) { t z = x; x = y; y = z; }

void vec2_zero(vec2 v);
float vec2_dot(vec2 a, vec2 b);
float vec2_norm2(vec2 v);
float vec2_norm(vec2 v);
void vec2_add(vec2 a, vec2 b, vec2 c);
void vec2_sub(vec2 a, vec2 b, vec2 c);
void vec2_scale(vec2 v, float s);
void vec2_normalize(vec2 v);
void vec2_rot(vec2 v, float th, vec2 d);
void vec2_ref(vec2 v, float th, vec2 d);
void vec2_shear(vec2 v, vec2 s, vec2 d);

void mat2_zero(mat2 m);
void mat2_identity(mat2 m);
void mat2_mul(mat2 a, mat2 b, mat2 c);
void mat2_transpose(mat2 m);
void mat2_mulv(mat2 m, vec2 v, vec2 d);
void mat2_scale(mat2 m, int v);
float mat2_det(mat2 m);

void vec3_zero(vec3 v);
float vec3_dot(vec3 a, vec3 b);
float vec3_norm2(vec3 v);
float vec3_norm(vec3 v);
void vec3_add(vec3 a, vec3 b, vec3 c);
void vec3_sub(vec3 a, vec3 b, vec3 c);
void vec3_scale(vec3 v, float s);
void vec3_scale_as(vec3 v, float s, vec3 d);
void vec3_normalize(vec3 v);
void vec3_rotx(vec3 v, float th, vec3 d);
void vec3_roty(vec3 v, float th, vec3 d);
void vec3_rotz(vec3 v, float th, vec3 d);
void vec3_shearx(vec3 v, vec3 s, vec3 d);
void vec3_sheary(vec3 v, vec3 s, vec3 d);
void vec3_shearz(vec3 v, vec3 s, vec3 d);
void vec3_cross(vec3 a, vec3 b, vec3 c);
void vec3_print(vec3 v);

void mat3_zero(mat3 m);
void mat3_identity(mat3 m);
void mat3_mul(mat3 a, mat3 b, mat3 c);
void mat3_transpose(mat3 m) ;
void mat3_mulv(mat3 m, vec3 v, vec3 d);
void mat3_scale(mat3 m, int v);
float mat3_det(mat3 m);

void vec4_zero(vec4 v);
float vec4_dot(vec4 a, vec4 b);
float vec4_norm2(vec4 v);
float vec4_norm(vec4 v);
void vec4_add(vec4 a, vec4 b, vec4 c);
void vec4_sub(vec4 a, vec4 b, vec4 c);
void vec4_scale(vec4 v, float s);
void vec4_normalize(vec4 v);
void vec4_rotx(vec4 v, float th, vec4 d);
void vec4_roty(vec4 v, float th, vec4 d);
void vec4_rotz(vec4 v, float th, vec4 d);
void vec4_shearx(vec4 v, vec4 s, vec4 d);
void vec4_sheary(vec4 v, vec4 s, vec4 d);
void vec4_shearz(vec4 v, vec4 s, vec4 d);

void mat4_zero(mat4 m);
void mat4_identity(mat4 m);
void mat4_mul(mat4 a, mat4 b, mat4 c);
void mat4_transpose(mat4 m) ;
void mat4_mulv(mat4 m, vec4 v, vec4 d);
void mat4_scale(mat4 m, int v);
void mat4_print(mat4 m);

float rad(float deg);
void rotate(mat4 m, float th, vec3 v);
void translate(mat4 m, vec3 v);
void lookat(vec3 eye, vec3 target, vec3 up, mat4 dest);
void perspective(float fovy, float aspect, float near, float far, mat4 dest);

#endif