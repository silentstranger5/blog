#include <stdio.h>
#include <stdlib.h>
#include <glad/gl.h>
#include <GLFW/glfw3.h>

typedef struct {
    float   *vertices;
    int     *elements;
    int vao, vbo, ebo;
    int vertices_size;
    int elements_size;
} vertices;

int fileread(char *filename, char **buffer) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        return 0;
    }
    fseek(f, 0, SEEK_END);
    int size = ftell(f);
    *buffer = calloc(size, sizeof(char));
    rewind(f);
    if (!fread(*buffer, sizeof(char), size, f)) {
        fclose(f);
        return 0;
    }
    fclose(f);
    return size;
}

int init_glfw(GLFWwindow **window) {
    // init glfw
    if (!glfwInit()) {
        fputs("failed to init glfw", stderr);
        return 0;
    }

    // create new window
    *window = glfwCreateWindow(640, 480, "Hello, World", NULL, NULL);
    if (!window) {
        fputs("failed to create window", stderr);
        return 0;
    }

    // make context of window current
    glfwMakeContextCurrent(*window);
    if (!gladLoadGL(glfwGetProcAddress)) {
        fputs("failed to load gl", stderr);
        return 0;
    }

    return 1;
}

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

// link shader program
int link_shader_program(char *vertex_filename, char *fragment_filename) {
    // create vertex shader
    char *vertex_source = NULL, *fragment_source = NULL;
    int size = fileread("vertex.glsl", &vertex_source);
    if (!size) {
        fputs("failed to read vertex source file", stderr);
        return 0;
    }
    GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex_shader, 1, &vertex_source, NULL);
    glCompileShader(vertex_shader);
    GLuint status;
    glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &status);
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
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    return shader_program;
}

// set layout for vertex array
void vertex_layout(int shader_program) {
    GLuint pos_attrib = glGetAttribLocation(shader_program, "position");
    glEnableVertexAttribArray(pos_attrib);
    glVertexAttribPointer(pos_attrib, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), 0);

    GLuint col_attrib = glGetAttribLocation(shader_program, "color");
    glEnableVertexAttribArray(col_attrib);
    glVertexAttribPointer(col_attrib, 3, GL_FLOAT, GL_FALSE,
                            5 * sizeof(float), (void*)(2 * sizeof(float))
    );
}

// delete vertex data
void vertex_delete(vertices *vertices) {
    glDeleteBuffers(1, &vertices->vbo);
    glDeleteBuffers(1, &vertices->ebo);
    glDeleteVertexArrays(1, &vertices->vao);
}

int main(int argc, char **argv) {
    // window object
    GLFWwindow *window;

    if (!init_glfw(&window)) {
        fputs("failed to init glfw", stderr);
        return 1;
    }
    
    GLfloat vertices_array[] = {
        0.0f,  0.5f, 1.0f, 0.0f, 0.0f,  // Vertex 1: Red
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  // Vertex 2: Green
        -0.5f, -0.5f, 0.0f, 0.0f, 1.0f  // Vertex 3: Blue
    };

    vertices vertices = {
        .vertices = vertices_array,
        .vertices_size = sizeof(vertices_array),
    };

    vertex_input(&vertices);

    GLuint shader_program = link_shader_program(
        "vertex.glsl", "fragment.glsl"
    );
    if (!shader_program) {
        fputs("failed to link shader program", stderr);
        return 1;
    }

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
