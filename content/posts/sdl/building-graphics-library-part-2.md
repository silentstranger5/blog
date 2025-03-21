+++
date = '2025-03-17T07:49:33+03:00'
draft = false
title = 'Building Graphics Library Part 2'
+++

In previous part, we learned how to draw lines, rectangles, triangles and some polygons.
There are some polygons, however, that can't be easily triangulated.
Because of that, we need to use another algorithm to draw them.

There are many algorithms that you can use here.
I'm going to use perhaps a rather intuitive and simple one called [Scanline Rendering](https://en.wikipedia.org/wiki/Scanline_rendering).

The core idea of the algorithm is simple.
We iterate the polygon row by row, from top to bottom, and then examine each edge of the polygon.
For each edge that intersects with the row, we write down the intersection point.
After that, we sort those points in ascending order.
Finally, we iterate through alternate pairs of points and draw lines between them.

Here is a picture that will make things a bit more clear.

![polygon](../polygon.png)

You may ask: how are we going to determine where the row intersects with the edge?
You can use the line interpolation equation from the previous post to answer this question.

```txt
x = x0  + (x1 - x0) * (y  - y0) / (y1 - y0)     for y0 <= y <= y1
```

Take a look at the function.

```c
void polygon(render_t *r, int m, float *v, int n) {
    if (m == LINE) {
        ...
        return;
    }
    // ymin and ymax are the polygon vertical borders
	float ymin = v[0], ymax = v[1];
    // xi stores x intersections
    float *xi = malloc(n * sizeof(float));
    // get polygon vertical borders
    for (int i = 0; i < n; i++) {
        float y = v[2 * i + 1];
        if (y < ymin) ymin = y;
        if (y > ymax) ymax = y;
    }
    // iterate y from top to bottom
    for (float y = ymin; y < ymax; y++) {
        // xn is a number of intersections
        int xn = 0;
        // iterate through polygon edges
        for (int i = 0; i < n; i++) {
            // get edge vertices
            float x0 = v[2 * i + 0];
            float y0 = v[2 * i + 1];
            float x1 = v[2 * ((i+1)%n) + 0];
            float y1 = v[2 * ((i+1)%n) + 1];
            // get edge vertical borders
            float ymin = (y0 < y1) ? y0 : y1;
            float ymax = (y0 > y1) ? y0 : y1;
            // if edge is not horizontal and y is between edge borders
            if ((y1 - y0) != 0 && ymin < y && y <= ymax) {
                // add the new x intercept
                xi[xn++] = x0 + (x1 - x0) * (y - y0) / (y1 - y0);
            }
        }
        // sort intercepts by increasing order
        qsort(xi, xn, sizeof(float), cmp);
        // iterate through alternate pairs of interception points
        for (int i = 0; i < xn; i += 2) {
            // draw line between points
            line(r, xi[i], y, xi[i + 1], y);
        }
    }
    free(xi);
}
```

Since I use the standard library `qsort` function, here is a comparison function.

```c
int cmp(const void *x, const void *y) {
        return (int) (*(float *) x - *(float *) y);
}
```

Note that we deliberately use asymmetric edge borders (`ymin < y <= ymax`).
This allows to gracefully handle polygon vertices.
Also, note that we ignore edges with a slope of zero since we divide by slope.

Try to draw a triangle with outlined and filled drawing mode.
Verify that it works correctly in both cases.

This algorithm will work for more general cases of polygons.

While I wrote this article, I stumbled upon another interesting thing that's worth discusssion.

So far we only considered things that are either described by a simple equation (like ellipse) or contain only straight lines (like polygons).
However, there is another thing that can be found anywhere in computer graphics: curves.
The font that you read from your display right now is probably described with them.
More precisely, computer graphics uses a particular form of them called *Bezier curves*.

Before we dive further, let's take a look at what bezier curve is.
Bezier curve is described with arbitrary amount of *control points* that define a shape of a curve.
For two points, it is a line computed using linear interpolation. More specifically, the formula looks like this:

```txt
B(t) = P0 + t(P1 - P0) = (1 - t)P0 + tP1    0 <= t <= 1
```

From the computational standpoint, points P0 and P1 are generally split into coordinates x and y and then the formula above is applied to each coordinate.

Take a look at the formula and think about what it describes. 
We start from the point P0 and then move along the difference (P1 - P0) multiplied by some t. 
This t varies between 0 and 1. When t is 0, we are at P0, and when it is 1, we are at P1.
This value is called *interpolant* and it describes where we are located between the points.

Bezier curves of higher order expand this idea to more points. Take a look at the picture below.

![bezier](../bezier.gif)

We have three points that describe the curve. Those points can be connected with two lines.
Let's call them A and B. They are the grey lines between points.
Then, each of those lines is interpolated with some value of t (interpolant).
This interpolation yields another point for each line. Let's call those points Q1 and Q2.
Those are green points on each line. We can then connect those points with another, third line.
Let's call this line C. This is a green line. Finally, interpolate this line with the value of t.
You get another point. Let's call it R. It is a black point on the picture.
The bezier curve can be described as a curve B that is given by points Rt for 0 <= t <= 1.
You can add as much points as you want to get more complex curves.

Bezier curves are a nice and compact way to describe almost any curve.
They are extensively used in computer graphics, animation and many other fields.
Bezier curves are described using recursive linear interpolation which yields a huge and complicated polynomial formula.
Computation of bezier curve with polynomial equation is severly unoptimal.
There is a nice recursive algorithm with numerical stability and optimal performance.
It is called *De casteljau algorithm*

As I noted above, bezier curves are described with points, and each of them has more than one coordinate.
Therefore, control points are usually split into coordinates, and those coordinates are then used for computations.

Here is a bezier curve code:

```c
// draw bezier curve
void bezier(render_t *r, float *p, int n) {
    // xv contains x coordinates of points
    float *xv = malloc(n * sizeof(float));
    // yv contains y coordinates of points
    float *yv = malloc(n * sizeof(float));
    // copy point coordinates into vectors
    for (int i = 0; i < n; i++) {
        xv[i] = p[2 * i + 0];
        yv[i] = p[2 * i + 1];
    }
    // compute bezier curve for t
    // notice the iteration step; it should be at most this small
    for (float t = 0; t < 1; t += 0.001f) {
        // compute x from xv and y from yv using De Casteljau algorithm
        float x = de_casteljau(xv, t, n);
        float y = de_casteljau(yv, t, n);
        point(r, x, y);
    }
    free(xv);
    free(yv);
}
```

As I mentioned above, points are split into coordinates which are then used to compute the curve coordinate values.

Now let's take a look at De Casteljau algorithm.

```c
float de_casteljau(float *k, float t, int n) {
    // return value
    float v;
    // beta holds values used for computation
    float *beta = malloc(n * sizeof(float));
    // we start by copying values from k into beta
    memcpy(beta, k, n * sizeof(float));
    // we start from one!
    for (int i = 1; i < n; i++)
        for (int j = 0; j < n - i; j++)
            // new value is computed as linear interpolation of values
            // from the previous iteration
            beta[j] = beta[j] * (1 - t) + beta[j + 1] * t;
    // return value is beta[0]
    v = beta[0];
    free(beta);
    return v;
}
```

Note that we have to copy values from k. Otherwise, we'll end up overwriting values at xv and yv which is certainly not what we want.

Try to use this code to draw a curve like this:

```c
bezier(renderer, (float []){50, 200, 100, 50, 200, 200}, 3);
```

You should see a smooth curve rendered on the screen.

Finally, let's tackle a wireframe mesh rendering. Take a look at the picture below.

![projection](../projection.png)

O is an origin, your eye or a camera sensor.
OL is a *line of sight* which is a line directed where you (or your camera) are looking.
CL' is a *canvas* which is a plane where image is projected.
Think about it as a painter canvas (or your display).
Note that OL' is defined to be 1. P is a point that is being projected onto the canvas.
P' is a projection of point P onto the canvas. We only consider two dimensions now for simplicity. 
More specifically, we consider YZ plane where O is an origin, OL lies on the Z axis, and
PL lies on the Y axis. 
It is as if we look from the side at you (or your camera) looking at something.

Now comes the pure geometry. For those who are not fans of math, bear with me.
It's good to know how any optical system works.

Triangles PLO and P'L'O are similar because they are right triangles that have an angle POL in common.
This means that all corresponding angles are equal and all corresponding sides are proportional.
More specifically,

```txt
PL / P'L' = OL / OL'
```

Recall that OL' is defined to be 1. Then, we have:

```txt
PL / P'L' = OL
```

Notice that PL is a y-coordinate of point P, P'L' is a y-coordinate of projected point P'.
OL is z-coordinate of point P. With this in mind, we have:

```txt
P.y / P'.y = P.z
P'.y * P.z = P.y
P'.y = P.y / P.z
```

In other words, in order to obtain a height of the projected point P' onto the canvas, we need to divide the height of the point P to its depth.

You can consider plane XZ, which looks as if we would be looking from above. 
Try to apply the same reasoning. You should arrive at the similar conclusion.

With this, we have:

```txt
P'.x = P.x / P.z
P'.y = P.y / P.z
```

In other words, in order to obtain coordinates of projected point on the canvas, we need to 
divide respective coordinates of the original point to its depth.
This is called *z-divide* and it is perhaps one of the most important equations in computer graphics.

Let's say that our canvas is a square whose center lies on the line of sight.
Then for both axes, we have a range of possible coordinates [-n, n].
n can be any number. The higher it is, the larger is the captured area.
However, we need to convert those coordinates to something else. 
For example, let's say that you need to convert those coordinates to a position on your display.
Before we do that, we need to convert our range into another range [0, 1].
This is called *normalized device coordinates* and sometimes is used directly in some platforms (like OpenGL).
The math is not very hard here:

```txt
x' = (x + n) / 2 * n
```

where x is original coordinate, n is canvas minimal value and x' is a normalized coordinate.

Given normalized coordinate, it is sufficient to scale it to the size of the new surface to get the new coordinates. 
For example, you can multiply it to the width of your window w.

Before we proceed, a word of caution. When I revised the algorithm, I stumbled upon some problems.
As it turns out, the problems were related to coordinate systems.
I use SDL in my tutorial, but you may use this tutorial for any graphical platform.
If that's the case, it is sufficient for you to redefine symbols like `point` or `line` and maybe slightly rewrite function prototypes.
Depending on your platform you may also need to encapsulate data into primitives like `vec2`.
Nevertheless, I try to keep code readable so that you won't have much problem applying it to other platforms.
Depending on your platform, *your coordinate system may vary!* Different platforms use different coordinate conventions.
This is especially problematic in 3D, but this is an issue in 2D as well.

You see, in some systems, origin is located at the *bottom left corner*.
It is the case for normalized device coordinates and platforms that support them like OpenGL.
This system seem to make sense because this is how we learned about cartesian coordinate system in school.
There are many other systems, however, that have origin set to *top left corner*.
The reasons for it are mostly historical: in old display devices, image was being projected onto the screen with a ray of light that "scanned" the image row by row, from top to bottom.
Hence, the first place that the ray hitted the screen was top left corner, which is why this place was chosen as an origin.
This old technology is long gone, however in many cases image data is still transmitted and processed in the same order: from top to bottom, from left to right. 
This system can be found in SDL as well. It means that in our platform, y axis points down, not up.
This means that we have to flip y-axis so that resulting image would be displayed correctly.
Since we have a range of [0, 1], it is sufficient to subtract our y-coordinate from 1 to get a new coordinate where old point at 0 (bottom) becomes 1 (top) and vice versa.

Now we know enough to render wireframe mesh. Let's take a look at the code.

```c
// render wireframe mesh
// r - renderer
// v - vertices (list of point coordinates)
// f - faces (list of mesh triangles)
// nv - number of vertices
// nf - number of faces
// ch - canvas height
// wh - window height
void mesh(render_t *r, float *v, int *f, int nv, int nf, float ch, float wh) {
    // proj will store projected normalized points
	float *proj = malloc(2 * nv * sizeof(float));
	for (int i = 0; i < nv; i++) {
        // perform z-divide on the point coordinates
		float x = v[3 * i + 0] / v[3 * i + 2];
		float y = v[3 * i + 1] / v[3 * i + 2];
        // normalize coordinates
		proj[2 * i + 0] =	  (x + ch) / (2 * ch);
		proj[2 * i + 1] = 1 - (y + ch) / (2 * ch); // flip y-axis
	}
    // t will store triangle coordinates in the screen space
    float t[6] = {0};
    // iterate through polygon faces
	for (int i = 0; i < nf; i++) {
        // iterate through vertices of a triangle
		for (int j = 0; j < 3; j++) {
            // copy coordinates from proj with
            // index obtained from a polygon face
			memcpy(&t[2 * j], &proj[2 * f[3 * i + j]], 
				2 * sizeof(float));
            // scale triangle coordinates with window height
			for (int k = 0; k < 2; k++)
				t[2 * j + k] *= wh;
		}
        // render a triangle
		triangle(r, LINE, t);
	}
	free(proj);
}
```

Note that we specify mesh using triangles. This means that any polygonal face must be triangulated (for example using a triangulation procedure specified above).

Now we have enough information to try and render some three-dimenional object onto our screen.

```c
// c = a + b
void vec3_add(vec3 a, vec3 b, vec3 c) {
	for (int i = 0; i < 3; i++) c[i] = a[i] + b[i];
}

void cube() {
	vec3 vertices[] = {
		{-0.5f, -0.5f, -0.5f},
		{-0.5f,  0.5f, -0.5f},
		{ 0.5f,  0.5f, -0.5f},
		{ 0.5f, -0.5f, -0.5f},
		{-0.5f, -0.5f,  0.5f},
		{-0.5f,  0.5f,  0.5f},
		{ 0.5f,  0.5f,  0.5f},
		{ 0.5f, -0.5f,  0.5f},
	};
	ivec3 faces[] = {
		{0, 1, 2}, {0, 2, 3},
		{4, 5, 1}, {4, 1, 0},
		{3, 2, 6}, {3, 6, 7},
		{7, 6, 5}, {7, 5, 4},
		{6, 2, 1}, {6, 1, 5},
		{3, 7, 4}, {3, 4, 0},
	};
	int nv = sizeof(vertices) / sizeof(vec3);
	int nf = sizeof(faces) / sizeof(ivec3);
    for (int i = 0; i < nv; i++) {
        vec3_add(vertices[i], (vec3){0, 0, 2});
	}
	color(renderer, 255, 255, 255, 255);
	mesh(renderer, (float *) vertices, (int *) faces, nv, nf, 0.5f, 255);
}
```

Before you use this function, expand the primitive types so that your compiler won't get confused. Add this to the header file.

```c
typedef  vec3 float[3];
typedef ivec3 int[3];
```

Try to render a cube using this procedure. It probably won't be very clear to you that it's a cube because it contains all triangle edges rather than just the cube edges.
However, the cube siluette should still be easily noticable.

Now, to add a little spice to this we can actually rotate the cube (hopefully then it will be more clear that it's actually a cube). Add this to the header file:

```c
typedef mat3 vec3[3];
```

Then, add this to the source code.

```c
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

// d = roty(v, th)
void vec3_roty(vec3 v, float th, vec3 d) {
	mat3 rot = {
		{	(float) cos(th),	0,	(float) sin(th)	},
		{		0,		1,		0	},
		{	(float) -sin(th),	0,	(float) cos(th)	},
	};
	mat3_mulv(rot, v, d);
}
```

Finally, you can add this to the `cube` function:

```c
void cube() {
    ...
    int nf = sizeof(faces) / sizeof(ivec3);
    static float angle = 0;
	for (int i = 0; i < nv; i++) {
        vec3_add(vertices[i], (vec3){0, 0, 2});
		vec3_roty(vertices[i], angle += 0.02f, vertices[i]);
	}
	color(renderer, 255, 255, 255, 255);
	...
}
```

Try to launch the program. You should see a cube rotating clockwise.

For those who are curious, I made a small and simple library that contains functions to work with computer graphics. 
You can check the [source code](../glm.c) and [header file](../glm.h).
If you want to use it, do not forget to remove type definitions (`vec3`, `ivec3`, `mat3`) from the header file, and to remove all functions operating on them from your source code.
For those who are more curious how it works, all you need to know is just two words: *linear algebra*.
This ~~magic~~ science allows you to perform linear transformations on objects and space and transform coordinate systems between each other.
If you want to learn it, you can start from [3Blue1Brown introductory course](https://www.youtube.com/watch?v=fNk_zzaMoSs&list=PLZHQObOWTQDPD3MizzM2xVFitgF8hE_ab).
This series is heavily focused around *meaning* and *application* of mathematical formalisms accompanied by good *visualizations*.
