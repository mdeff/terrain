// Copyright (C) 2014 - LGG EPFL

#include <iostream>
#include <sstream>
#include "common.h"

#include "rendering_vshader.h"
#include "rendering_fshader.h"
#include "heightmap_vshader.h"
#include "heightmap_fshader.h"


/// Shader program.
GLuint renderingProgramID;

/// NxN triangle grid.
/// const is better than #define : type checked, optimized out anyway
const int N = 16;
const int nVertices = N*N;
const int nIndices = (N-1)*(N-1)*6;

/// Screen size.
const int windowWidth(1024);
const int windowHeight(768);


void update_matrix_stack(const mat4& model) {

    /// Define projection matrix (camera intrinsics)
    static mat4 projection = Eigen::perspective(45.0f, 4.0f/3.0f, 0.1f, 10.0f);
    GLuint projectionID = glGetUniformLocation(renderingProgramID, "projection");
    glUniformMatrix4fv(projectionID, ONE, DONT_TRANSPOSE, projection.data());

    /// Define the view matrix (camera extrinsics)
    vec3 cam_look(0.0f, 0.0f, 0.0f);
    /// Camera is in the sky, looking down.
//    vec3 cam_pos(0.0f, 0.0f, 5.0f);
//    vec3 cam_up(0.0f, 1.0f, 0.0f);
    /// Camera is in a corner, looking down to the terrain.
    vec3 cam_pos(3.0f, -3.0f, 1.0f);
//    vec3 cam_pos(2.0f, -2.0f, 1.5f);
    vec3 cam_up(0.0f, 0.0f, 1.0f);
    static mat4 view = Eigen::lookAt(cam_pos, cam_look, cam_up);

    /// Assemble the "Model View" matrix
    static mat4 modelview;
    modelview = view * model;
    GLuint modelviewID = glGetUniformLocation(renderingProgramID, "modelview");
    glUniformMatrix4fv(modelviewID, ONE, DONT_TRANSPOSE, modelview.data());

}


/// Generate a simple height map texture for test purpose.
GLuint gen_test_height_map() {

    /// Create and bind the texture.
    GLuint heightmapTexture;
    glGenTextures(ONE, &heightmapTexture);
    glBindTexture(GL_TEXTURE_2D, heightmapTexture);

    /// Flat terrain.
    //float pixels[] = {1.0f};
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, 1, 1, 0, GL_RED, GL_FLOAT, pixels);

    /// Sort of "pyramid".
    // (0,0) corresponds to the lower left corner.
    // (0, 0) (.5, 0) (1, 0)
    // (0,.5) (.5,.5) (1,.5)
    // (0, 1) (.5, 1) (1, 1)
    float pixels[] = {
        0.0f, 0.5f, 0.0f,
        0.5f, 1.0f, 0.5f,
        0.0f, 0.5f, 0.0f
    };
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, 3, 3, 0, GL_RED, GL_FLOAT, pixels);

    /// Simple filtering (it is needed).
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    /// Return the height map texture ID.
    return heightmapTexture;
}


/// Generate the triangle grid vertices.
void gen_triangle_grid() {

    /// Generate the vertices (line by line) : 16^2 = 256 vertices.
    vec3 vertices[nVertices];
    for(int y=0; y<N; y++) {
        for(int x=0; x<N; x++) {
            vertices[y*N+x] = vec3(float(2*x)/(N-1)-1, float(2*y)/(N-1)-1, 0);
        }
    }

    /// Copy the vertices in a vertex buffer.
    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    /// Indices that form the triangles.
    /// Grid of 15x15 squares : 225 squares -> 450 triangles -> 1350 indices.
    unsigned int indices[nIndices];
    for(int y=0; y<N-1; y++) {
        for(int x=0; x<N-1; x++) {
            /// Upper left triangle of the square.
            indices[y*(N-1)*6+x*6+0] = y*N + x + 0;
            indices[y*(N-1)*6+x*6+1] = y*N + x + 1;
            indices[y*(N-1)*6+x*6+2] = y*N + x + N;
            /// Lower right triangle of the square.
            indices[y*(N-1)*6+x*6+3] = y*N + x + 1;
            indices[y*(N-1)*6+x*6+4] = y*N + x + N+1;
            indices[y*(N-1)*6+x*6+5] = y*N + x + N;
        }
    }

    /// Copy the indices in an index buffer.
    GLuint elementbuffer;
    glGenBuffers(1, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    /// Vertex attribute "position" points to the binded buffer.
    GLuint positionID = glGetAttribLocation(renderingProgramID, "position");
    glEnableVertexAttribArray(positionID);
    // vec3: 3 floats per vertex for the position attribute.
    glVertexAttribPointer(positionID, 3, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);

}


void init() {

    /// Vertex array.
    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    /// Generate the height map texture.
    /// Before shader compilation, as it uses its own shaders.
    GLuint heightMap = gen_test_height_map();
//    GLuint heightMap = gen_height_map();

    /// Compile the rendering shaders.
    /// Triangle grid needs the programID to get the "position" attribute.
    renderingProgramID = compile_shaders(rendering_vshader, rendering_fshader);
    if(!renderingProgramID)
        exit(EXIT_FAILURE);
    glUseProgram(renderingProgramID);

    /// Bind the heightmap to texture 0.
    //glEnable(GL_TEXTURE_2D);
    const GLuint heightMapTex = 0;
    glActiveTexture(GL_TEXTURE0+heightMapTex);
    glBindTexture(GL_TEXTURE_2D, heightMap);
    GLuint heightMapTexID = glGetUniformLocation(renderingProgramID, "heightMapTex");
    glUniform1i(heightMapTexID, heightMapTex);

    /// Generate a flat and regular triangle grid.
    gen_triangle_grid();

    /// Initialize the matrix stack.
    update_matrix_stack(mat4::Identity());

    /// OpenGL parameters.
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);

}


void display() {
    //To render only the boundary
    //comment it if you want to render full triangles
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, ZERO_BUFFER_OFFSET);
}


int main(int, char**) {
    glfwInitWindowSize(windowWidth, windowHeight);
    glfwCreateWindow("Project - Group 19");
    glfwDisplayFunc(display);
    init();
    glfwTrackball(update_matrix_stack);
    glfwMainLoop();
    return EXIT_SUCCESS;    
}
