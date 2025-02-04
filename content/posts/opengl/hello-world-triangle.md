+++
date = '2025-02-04T23:30:00+03:00'
draft = false
title = 'Hello World Triangle'
+++

Before using OpenGL, we need to arrange a couple of things. Firstly, OpenGL only specifies graphics API. However, to use OpenGL you need something called *context* - a state machine with an OpenGL state in memory. Usually, this context is bound to a window. OpenGL does not specify the creation of windows and contexts associated with them. This means that it's different depending on the user platform! This is the last thing you need in a cross-platform application.

Fortunately, there are several *binding libraries* that manage the creation of a window and an associated context. I will be using [GLFW](https://glfw.org) since it is simple to use and tailored specifically for OpenGL.

After we created a new context, we now need some way to obtain OpenGL *methods* - functions that allow us to interact with the library. Unfortunately, most platforms support only old OpenGL versions by default (around 1.1 - 1.2). This means that if you want to load functions from newer versions of OpenGL you have to load function pointers from a driver software at a runtime. Again, OpenGL does not specify anything about it, so each platform uses a specific API to do that.

As in the previous case, there are libraries that can help us with this. They are called *loader libraries*. The most common one is [GLEW](https://glew.sourceforge.net).

To make dependency management easier, I use a package manager called [vcpkg](https://vcpkg.io) with the CMake building system. Assuming that vcpkg is already installed on your system and environment variables are configured, it's easy to configure our project:

```bash
mkdir triangle
cd triangle
vcpkg new --application
vcpkg add port glfw3 glew
```

CMakeLists.txt:

```txt
cmake_minimum_required(VERSION 3.5.0)
project(triangle VERSION 0.1.0 LANGUAGES C)

find_package(glfw3 CONFIG REQUIRED)
find_package(GLEW REQUIRED)

add_executable(triangle main.c)
target_link_libraries(triangle PRIVATE glfw GLEW::GLEW)
```

CMakePresets.json:

```json
{
    "version": 8,
    "configurePresets": [
        {
            "name": "vcpkg",
            "displayName": "Configure preset using toolchain file",
            "description": "Sets Ninja generator, build and install directory",
            "generator": "Ninja",
            "binaryDir": "${sourceDir}/out/build/${presetName}",
            "cacheVariables": {
                "CMAKE_BUILD_TYPE": "Debug",
                "CMAKE_TOOLCHAIN_FILE": "$env{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake",
                "CMAKE_INSTALL_PREFIX": "${sourceDir}/out/install/${presetName}"
            }
        }
    ]
}
```

main.c:

```c
#include <stdio.h>

int main(void) {
    printf("hello, world\n");
    return 0;
}
```

Finally, you can build your project:

```bash
cmake --preset=vcpkg
cmake --build out/build/vcpkg
```

This project is generated automatically. Feel free to modify anything you like.

Right now the program just prints out "hello, world", but if there are no errors, this is a good starting point. We can finally start getting into the actual programming.

Remove everything from the source file. Include the headers:

```c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
```

As always, define the `fileread` function to read the contents of a file:

```c
// read file
int fileread(char *filename, char **buffer) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        return 0;
    }
    fseek(f, 0, SEEK_END);
    int size = ftell(f);
    *buffer = malloc(size * sizeof(char));
    rewind(f);
    int ret = fread(*buffer, sizeof(char), size, f);
    fclose(f);
    *(strrchr(*buffer, '}') + 1) = 0;
    return ret;
}
```

Now, let's initialize a new window:

```c
// initialize a new window
int init_window(GLFWwindow **window) {
    // initialize glfw
    if (!glfwInit()) {
        fputs("failed to init glfw", stderr);
        return 0;
    }

    // create a new window
    *window = glfwCreateWindow(640, 480, "Hello, World", NULL, NULL);
    if (!window) {
        fputs("failed to create window", stderr);
        return 0;
    }

    // make context of the window current
    glfwMakeContextCurrent(*window);

    // initialize glew
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (err != GLEW_OK) {
        fprintf(stderr, "failed to init glew: %s", glewGetErrorString(err));
        return 0;
    }

    return 1;
}
```

Create the `main` function:

```c
int main(int argc, char **argv) {
    // window object
    GLFWwindow *window;
    if (!init_window(&window)) {
        fputs("failed to init glfw", stderr);
        return 1;
    }

    // render loop
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // terminate glfw
    glfwTerminate();
    return 0;
}
```

You should see an empty black window now. This is how you can create a new window with OpenGL.

Now that we have a window, we can finally use OpenGL to draw something.

There are a lot of important technical details that I'm not going to cover here. Other people write about this better than I would. Check [this resource](https://open.gl) for more details.

To be brief, the *graphics pipeline* covers all the steps needed to draw something.

Input vertex data about the thing that we want to draw are transformed by vertex shader so that the coordinates of the object are transformed into device coordinates. After that, vertices are assembled into shapes (most often into triangles). Geometry shader modifies those shapes according to a specified program. The result is then split into pixel-sized fragments. Finally, the fragment shader assigns a color to each fragment which is then shown on our screen.

To draw polygons, we need four things:

- Vertex buffer stores vertex data like coordinates and color
- Vertex array stores layout for vertex buffer
- Vertex shader is a program that performs transformations on vertices
- Fragment shader is a program that assigns a color to each fragment

I will bundle all data required to represent information about vertices together. Let's create a new structure:

```c
// add this after headers
typedef struct {
    float   *vertices;
    int     *elements;
    int vao, vbo, ebo;
    int vertices_size;
    int elements_size;
} vertices;
```

Here, `vertices` will store vertex data, and `elements` is an optional field that will store the order in which vertices should be processed.

Let's create a new function to handle vertex input. We assume that `vertices` (and optionally `elements`) are already set.

```c
// handle vertex input
void vertex_input(vertices *vertices) {
    // create vertex array
    glGenVertexArrays(1, &vertices->vao);
    glBindVertexArray(vertices->vao);

    // create vertex buffer
    glGenBuffers(1, &vertices->vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vertices->vbo);
    glBufferData(GL_ARRAY_BUFFER, vertices->vertices_size, vertices->vertices, GL_STATIC_DRAW);

    if (vertices->elements) {
        glGenBuffers(1, &vertices->ebo);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vertices->ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                        vertices->elements_size, vertices->elements, GL_STATIC_DRAW);
    }
}
```

OpenGL uses numbers to refer to vertex arrays and vertex buffers instead of pointers. We use `glGenVertexArrays` and `glGenBuffers` to create new arrays or buffers. Numbers that represent them are then written into provided pointers. Those numbers are stored in fields `vao`, `vbo`, and `ebo` of our `vertices` structure. We use `glBindVertexArray` and `glBindBuffer` to set a certain vertex array or vertex buffer as active. Most functions related to vertex arrays or buffers interact with the active array or buffer. Function `glBufferData` copies data from the specified pointer into a buffer.

I already mentioned *shaders* - programs that tell graphics cards what to do. Let's create vertex and fragment shaders. They are the simplest versions of such shaders:

vertex.glsl:

```glsl
#version 460 core   // use version 4.60

in vec2 position;   // first input is a vertex position
in vec3 color;      // second input is a vertex color

out vec3 v_color;   // we output vertex color

void main() {
    v_color = color;
    gl_Position = vec4(position, 0.0, 1.0);     // gl_Position is position of displayed vertex
}
```

fragment.glsl:

```glsl
#version 460 core

in  vec3 v_color;   // we accept a color
out vec4 color;     // and then display it

void main() {
    color = vec4(v_color, 1);
}
```

The first line in shaders specifies the OpenGL version that we'll be using. As with any program, shaders have *input* and *output* data which are specified with keywords `in` and `out` respectively. The entry point of shaders is in the `main` function. `gl_Position` is a special variable that stores the position of the displayed vertex. The fragment shader must produce an output value (here, `color`), which is then used to assign a color to a fragment.

Now that we have shader source code, we need to load shaders, compile them, and link them into a shader program. Here is a function that does that:

```c
// link shader program
int link_shader_program(char *vertex_filename, char *fragment_filename) {
    // create vertex shader
    char *vertex_source = NULL, *fragment_source = NULL;
    int size = fileread("vertex.glsl", &vertex_source);
    if (!size) {
        fputs("failed to read vertex source file\n", stderr);
        return 0;
    }
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    // load shader source
    glShaderSource(vertex_shader, 1, &vertex_source, NULL);
    // compile vertex shader
    glCompileShader(vertex_shader);
    // check compilation status
    GLuint status;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &status);
    // if compilation failed, display the log and return 0
    if (!status) {
        char buffer[512];
        glGetShaderInfoLog(vertex_shader, 512, NULL, buffer);
        fputs("failed to compile vertex shader", stderr);
        fputs(buffer, stderr);
        return 0;
    }

    // create fragment shader
    size = fileread("fragment.glsl", &fragment_source);
    if (!size) {
        fputs("failed to read fragment source file", stderr);
        return 0;
    }
    GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment_shader, 1, &fragment_source, NULL);
    glCompileShader(fragment_shader);
    glGetShaderiv(fragment_shader, GL_COMPILE_STATUS, &status);
    if (!status) {
        char buffer[512];
        glGetShaderInfoLog(fragment_shader, 512, NULL, buffer);
        fputs("failed to compile fragment shader", stderr);
        fputs(buffer, stderr);
        return 0;
    }

    // link shaders into program
    GLuint shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, fragment_shader);
    glLinkProgram(shader_program);
    glUseProgram(shader_program);

    // delete shaders
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return shader_program;
}
```

Now that we have a shader program, we can set *layout* for the vertex array. Remember input values from our vertex shader? Vertex data is stored in one interleaved buffer, so OpenGL needs a way to distinguish input values from this buffer. Layout specifies a way for our program to do that:

```c
// set layout for vertex array
void vertex_layout(int shader_program) {
    // set position input to be 2 component float vector with stride of 5 floats starting from position 0
    GLuint pos_attrib = glGetAttribLocation(shader_program, "position");
    glEnableVertexAttribArray(pos_attrib);
    glVertexAttribPointer(pos_attrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);

    // set color input to be 3 component float vector with stride of 5 floats starting from position of 2*float
    GLuint col_attrib = glGetAttribLocation(shader_program, "color");
    glEnableVertexAttribArray(col_attrib);
    glVertexAttribPointer(col_attrib, 3, GL_FLOAT, GL_FALSE,
                            5 * sizeof(float), (void*)(2 * sizeof(float))
    );
}
```

Don't forget to delete all vertex arrays and buffers:

```c
// delete vertex data
void vertex_delete(vertices *vertices) {
    glDeleteBuffers(1, &vertices->vbo);
    glDeleteVertexArrays(1, &vertices->vao);
    if (vertices->ebo) {
        glDeleteBuffers(1, &vertices->ebo);
    }
}
```

Finally, write the `main` function:

```c
int main(int argc, char **argv) {
    // window object
    GLFWwindow *window;

    if (!init_window(&window)) {
        fputs("failed to init glfw", stderr);
        return 1;
    }

    // vertex data; first two numbers are coordinates and
    // three numbers after that are color
    GLfloat vertices_array[] = {
        0.0f,  0.5f, 1.0f, 0.0f, 0.0f,  // Vertex 1: Red
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  // Vertex 2: Green
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f  // Vertex 3: Blue
    };

    // pack all vertex data together
    vertices vertices = {
        .vertices = vertices_array,
        .vertices_size = sizeof(vertices_array),
    };

    // handle vertex input
    vertex_input(&vertices);

    // link shader program
    GLuint shader_program = link_shader_program(
        "vertex.glsl", "fragment.glsl"
    );
    if (!shader_program) {
        fputs("failed to link shader program", stderr);
        return 1;
    }

    // set vertex layout
    vertex_layout(shader_program);

    // render loop
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // delete program and buffers
    glDeleteProgram(shader_program);
    vertex_delete(&vertices);

    // terminate glfw
    glfwTerminate();
    return 0;
}
```

Try to compile the program. You should see a window with a three-colored triangle. Check out the [source code](../triangle.c).