#include "glm.h"

// v = 0
void vec2_zero(vec2 v) {
	for (int i = 0; i < 2; i++) v[i] = 0;
}

// a · b
float vec2_dot(vec2 a, vec2 b) {
	float n = 0;
	for (int i = 0; i < 2; i++)
		n += a[i] * b[i];
	return n;
}

// ||v|| ** 2
float vec2_norm2(vec2 v) {
	return vec2_dot(v, v);
}

// ||v||
float vec2_norm(vec2 v) {
	return (float) sqrt(vec2_norm2(v));
}

// c = a + b
void vec2_add(vec2 a, vec2 b, vec2 c) {
	for (int i = 0; i < 2; i++) c[i] = a[i] + b[i];
}

// c = a - b
void vec2_sub(vec2 a, vec2 b, vec2 c) {
	for (int i = 0; i < 2; i++) c[i] = a[i] - b[i];
}

// v *= s
void vec2_scale(vec2 v, float s) {
	for (int i = 0; i < 2; i++) v[i] *= s;
}

// v /= ||v||
void vec2_normalize(vec2 v) {
	float n = vec2_norm(v);
	vec2_scale(v, 1/n);
}

// d = rot(v, th)
void vec2_rot(vec2 v, float th, vec2 d) {
	mat2 rot = {
		{(float) cos(th), (float) -sin(th)}, 
		{(float) sin(th), (float)  cos(th)}
	};
	mat2_mulv(rot, v, d);
}

// d = ref(v, th)
void vec2_ref(vec2 v, float th, vec2 d) {
	mat2 ref = {
		{(float) cos(2*th), (float)  sin(2*th)},
		{(float) sin(2*th), (float) -cos(2*th)}
	};
	mat2_mulv(ref, v, d);
}

// d = shear(v, s)
void vec2_shear(vec2 v, vec2 s, vec2 d) {
	mat2 shear = {
		{1 + s[0] * s[1], s[0]},
		{s[1], 				1 },
	};
	mat2_mulv(shear, v, d);
}

// m = 0
void mat2_zero(mat2 m) {
	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++)
			m[i][j] = 0;
}

// m = I
void mat2_identity(mat2 m) {
	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++)
			m[i][j] = (float) (i == j);
}

// c = a * b
void mat2_mul(mat2 a, mat2 b, mat2 c) {
	mat2_zero(c);
	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++)
			for (int k = 0; k < 2; k++)
				c[i][j] += a[i][k] * b[k][j];
}

// m = T(m)
void mat2_transpose(mat2 m) {
	swap(float, m[0][1], m[1][0]);
}

// d = m * v
void mat2_mulv(mat2 m, vec2 v, vec2 d) {
	int copy = (v == d);
	if (copy) d = malloc(sizeof(vec2));
	vec2_zero(d);
	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++)
			d[i] += m[i][j] * v[j];
	if (copy) { memcpy(v, d, sizeof(vec2)); free(d); }
}

// m *= v
void mat2_scale(mat2 m, int v) {
	for (int i = 0; i < 2; i++)
		for (int j = 0; j < 2; j++)
			m[i][j] *= v;
}

// D(m)
float mat2_det(mat2 m) {
	return m[0][0] * m[1][1] - m[0][1] * m[1][0];
}

// v = 0
void vec3_zero(vec3 v) {
	for (int i = 0; i < 3; i++) v[i] = 0;
}

// v = a · b
float vec3_dot(vec3 a, vec3 b) {
	float n = 0;
	for (int i = 0; i < 3; i++)
		n += a[i] * b[i];
	return n;
}

// ||v|| ** 2
float vec3_norm2(vec3 v) {
	return vec3_dot(v, v);
}

// ||v||
float vec3_norm(vec3 v) {
	return (float) sqrt(vec3_norm2(v));
}

// c = a + b
void vec3_add(vec3 a, vec3 b, vec3 c) {
	for (int i = 0; i < 3; i++) c[i] = a[i] + b[i];
}

// c = a - b
void vec3_sub(vec3 a, vec3 b, vec3 c) {
	for (int i = 0; i < 3; i++) c[i] = a[i] - b[i];
}

// v *= s
void vec3_scale(vec3 v, float s) {
	for (int i = 0; i < 3; i++) v[i] *= s;
}

void vec3_scale_as(vec3 v, float s, vec3 d) {
	for (int i = 0; i < 3; i++) d[i] = v[i] * s;
}

// v /= ||v||
void vec3_normalize(vec3 v) {
	float n = vec3_norm(v);
	vec3_scale(v, 1/n);
}

// d = rotx(v, th)
void vec3_rotx(vec3 v, float th, vec3 d) {
	mat3 rot = {
		{	1,		0,		0	},
		{	0, (float) cos(th), (float) -sin(th)},
		{	0, (float) sin(th), (float)  cos(th)},
	};
	mat3_mulv(rot, v, d);
}

// d = roty(v, th)
void vec3_roty(vec3 v, float th, vec3 d) {
	mat3 rot = {
		{	(float) cos(th),	0,	(float) sin(th)	},
		{		0,		1,		0	},
		{	(float) -sin(th),	0,	(float) cos(th)	},
	};
	mat3_mulv(rot, v, d);
}

// d = rotz(v, th)
void vec3_rotz(vec3 v, float th, vec3 d) {
	mat3 rot = {
		{	(float) cos(th),	(float) -sin(th),	0	},
		{	(float) sin(th),	(float)  cos(th),	0	},
		{		0,			0,		1	},
	};
	mat3_mulv(rot, v, d);
}

// d = shearx(v, s)
void vec3_shearx(vec3 v, vec3 s, vec3 d) {
	mat3 shear = {
		{1, s[1], s[2]},
		{0, 1, 0},
		{0, 0, 1},
	};
	mat3_mulv(shear, v, d);
}

// d = sheary(v, s)
void vec3_sheary(vec3 v, vec3 s, vec3 d) {
	mat3 shear = {
		{1, 0, 0},
		{s[0], 1, s[2]},
		{0, 0, 1},
	};
	mat3_mulv(shear, v, d);
}

// d = shearz(v, s)
void vec3_shearz(vec3 v, vec3 s, vec3 d) {
	mat3 shear = {
		{1, 0, 0},
		{0, 1, 0},
		{s[0], s[1], 1},
	};
	mat3_mulv(shear, v, d);
}

// c = a x b
void vec3_cross(vec3 a, vec3 b, vec3 c) {
	c[0] = a[1] * b[2] - a[2] * b[1];
	c[1] = a[2] * b[0] - a[0] * b[2];
	c[2] = a[0] * b[1] - a[1] * b[0];
}

// m = 0
void mat3_zero(mat3 m) {
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			m[i][j] = 0;
}

// m = I
void mat3_identity(mat3 m) {
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			m[i][j] = (float) (i == j);
}

// c = a * b
void mat3_mul(mat3 a, mat3 b, mat3 c) {
	mat3_zero(c);
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			for (int k = 0; k < 3; k++)
				c[i][j] += a[i][k] * b[k][j];
}

// m = T(m)
void mat3_transpose(mat3 m) {
	swap(float, m[0][1], m[1][0]);
	swap(float, m[0][2], m[2][0]);
	swap(float, m[1][2], m[2][1]);
}

// d = m * v
void mat3_mulv(mat3 m, vec3 v, vec3 d) {
	int copy = (v == d);
	if (copy) d = malloc(sizeof(vec3));
	vec3_zero(d);
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			d[i] += m[i][j] * v[j];
	if (copy) { memcpy(v, d, sizeof(vec3)); free(d); }
}

// m *= s
void mat3_scale(mat3 m, int s) {
	for (int i = 0; i < 3; i++)
		for (int j = 0; j < 3; j++)
			m[i][j] *= s;
}

// D(m)
float mat3_det(mat3 m) {
	return m[0][0] * m[1][1] * m[2][2] +
		m[0][1] * m[1][2] * m[2][0] +
		m[0][2] * m[1][0] * m[2][1] -
		m[0][2] * m[1][1] * m[2][0] -
		m[0][1] * m[1][0] * m[2][2] -
		m[0][0] * m[1][2] * m[2][1];
}

void vec4_zero(vec4 v) {
	for (int i = 0; i < 4; i++) v[i] = 0;
}

// v = a · b
float vec4_dot(vec4 a, vec4 b) {
	float n = 0;
	for (int i = 0; i < 4; i++)
		n += a[i] * b[i];
	return n;
}

// ||v|| ** 2
float vec4_norm2(vec4 v) {
	return vec4_dot(v, v);
}

// ||v||
float vec4_norm(vec4 v) {
	return (float) sqrt(vec4_norm2(v));
}

// c = a + b
void vec4_add(vec4 a, vec4 b, vec4 c) {
	for (int i = 0; i < 4; i++) c[i] = a[i] + b[i];
}

// c = a - b
void vec4_sub(vec4 a, vec4 b, vec4 c) {
	for (int i = 0; i < 4; i++) c[i] = a[i] - b[i];
}

// v *= s
void vec4_scale(vec4 v, float s) {
	for (int i = 0; i < 4; i++) v[i] *= s;
}

// v /= ||v||
void vec4_normalize(vec4 v) {
	float n = vec4_norm(v);
	vec4_scale(v, 1/n);
}

// d = rotx(v, th)
void vec4_rotx(vec4 v, float th, vec4 d) {
	mat4 rot = {
		{	1,		0,		0,		0	},
		{	0, (float) cos(th), (float) -sin(th),	0},
		{	0, (float) sin(th), (float)  cos(th),	0},
		{	0,		0,		0,		1	},
	};
	mat4_mulv(rot, v, d);
}

// d = roty(v, th)
void vec4_roty(vec4 v, float th, vec4 d) {
	mat4 rot = {
		{	(float) cos(th),	0,	(float) sin(th),	0	},
		{		0,		1,		0,		0	},
		{	(float) -sin(th),	0,	(float) cos(th),	0	},
		{		0,		0,		0,		1	},
	};
	mat4_mulv(rot, v, d);
}

// d = rotz(v, th)
void vec4_rotz(vec4 v, float th, vec4 d) {
	mat4 rot = {
		{	(float) cos(th),	(float) -sin(th),	0,		0	},
		{	(float) sin(th),	(float)  cos(th),	0,		0	},
		{		0,		0,		1,		0	},
		{		0,		0,		0,		1	},
	};
	mat4_mulv(rot, v, d);
}

// d = shearx(v, s)
void vec4_shearx(vec4 v, vec4 s, vec4 d) {
	mat4 shear = {
		{1,	s[1], s[2], 0},
		{0, 1, 0, 0},
		{0, 0, 1, 0},
		{0, 0, 0, 1},
	};
	mat4_mulv(shear, v, d);
}

// d = sheary(v, s)
void vec4_sheary(vec4 v, vec4 s, vec4 d) {
	mat4 shear = {
		{1, 0, 0, 0},
		{s[0], 1, s[2], 0},
		{0, 0, 1, 0},
		{0, 0, 0, 1}
	};
	mat4_mulv(shear, v, d);
}

// d = shearz(v, s)
void vec4_shearz(vec4 v, vec4 s, vec4 d) {
	mat4 shear = {
		{1, 0, 0, 0},
		{0, 1, 0, 0},
		{s[0], s[1], 1, 0},
		{0, 0, 0, 1},
	};
	mat4_mulv(shear, v, d);
}

// m = 0
void mat4_zero(mat4 m) {
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			m[i][j] = 0;
}

// m = I
void mat4_identity(mat4 m) {
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			m[i][j] = (float) (i == j);
}

// c = a * b
void mat4_mul(mat4 a, mat4 b, mat4 c) {
	mat4_zero(c);
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			for (int k = 0; k < 4; k++)
				c[i][j] += a[i][k] * b[k][j];
}

// m = T(m)
void mat4_transpose(mat4 m) {
	swap(float, m[0][1], m[1][0]);
	swap(float, m[0][2], m[2][0]);
	swap(float, m[1][2], m[2][1]);
	swap(float, m[0][3], m[3][0]);
	swap(float, m[1][3], m[3][1]);
	swap(float, m[2][3], m[3][2]);
}

// d = m * v
void mat4_mulv(mat4 m, vec4 v, vec4 d) {
	int copy = (v == d);
	if (copy) d = malloc(sizeof(vec4));
	vec4_zero(d);
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			d[i] += m[i][j] * v[j];
	if (copy) { memcpy(v, d, sizeof(vec4)); free(d); }
}

// m *= s
void mat4_scale(mat4 m, int s) {
	for (int i = 0; i < 4; i++)
		for (int j = 0; j < 4; j++)
			m[i][j] *= s;
}

float rad(float deg) { return deg * PI / 180; }

void rotate(mat4 m, float th, vec3 v) {
	vec3 col1 = {
		v[0] * v[0] * (1 - (float) cos(th)) + (float) cos(th),
		v[0] * v[1] * (1 - (float) cos(th)) + v[2] * (float) sin(th),
		v[0] * v[2] * (1 - (float) cos(th)) - v[1] * (float) sin(th),
	};
	vec3 col2 = {
		v[0] * v[1] * (1 - (float) cos(th)) - v[2] * (float) sin(th),
		v[1] * v[1] * (1 - (float) cos(th)) + (float) cos(th),
		v[1] * v[2] * (1 - (float) cos(th)) + v[0] * (float) sin(th),
	};
	vec3 col3 = {
		v[0] * v[2] * (1 - (float) cos(th)) + v[1] * (float) sin(th),
		v[1] * v[2] * (1 - (float) cos(th)) - v[0] * (float) sin(th),
		v[2] * v[2] * (1 - (float) cos(th)) + (float) cos(th),
	};
	mat4 rot = {0};
	mat4_identity(rot);
	memcpy(rot[0], col1, sizeof(vec3));
	memcpy(rot[1], col2, sizeof(vec3));
	memcpy(rot[2], col3, sizeof(vec3));
	mat4 res = {0};
	mat4_mul(m, rot, res);
	memcpy(m, res, sizeof(mat4));
}

void translate(mat4 m, vec3 v) {
	for (int i = 0; i < 3; i++) m[i][3] += v[i];
}

void lookat(vec3 eye, vec3 target, vec3 up, mat4 dest) {
	vec3 s = {0}, f = {0}, u = {0};
	vec3_sub(target, eye, f);
	vec3_normalize(f);
	vec3_normalize(up);
	vec3_cross(f, up, s);
	vec3_normalize(s);
	vec3_cross(s, f, u);
	vec3_scale(f, -1);
	memcpy(dest[0], s, sizeof(vec3));
	memcpy(dest[1], u, sizeof(vec3));
	memcpy(dest[2], f, sizeof(vec3));
	mat4_transpose(dest);
	dest[3][0] = -vec3_dot(s, eye);
	dest[3][1] = -vec3_dot(u, eye);
	dest[3][2] = -vec3_dot(f, eye);
}

void perspective(float fovy, float aspect, float znear, float zfar, mat4 dest) {
	float f = 1 / (float) tan(fovy / 2);
	mat4 m = {
		{f / aspect, 0, 0, 0},
		{0, f, 0, 0},
		{0, 0, (zfar + znear) / (znear - zfar), -1},
		{0, 0, (2 * zfar * znear) / (znear - zfar), 0},
	};
	memcpy(dest, m, sizeof(mat4));
}

int printf();

void vec3_print(vec3 v) {
	for (int i = 0; i < 3; i++)
		printf("%8.4f ", v[i]);
	printf("\n");
}

void mat4_print(mat4 m) {
	for (int i = 0; i < 4; i++) {
		for (int j = 0; j < 4; j++)
			printf("%8.4f ", m[i][j]);
		printf("\n");
	}
}