// Copyright (C) 2014 - LGG EPFL

#include <iostream>
#include <sstream>

#include "common.h"
#include "heightmap.h"

#include "rendering_vshader.h"
#include "rendering_fshader.h"


/// Shader program.
GLuint renderingProgramID;

/// NxN triangle grid.
/// const is better than #define : type checked, optimized out anyway
const int N = 64;
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
    GLuint vertexBufferID;
    glGenBuffers(1, &vertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
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
    GLuint elementBufferID;
    glGenBuffers(1, &elementBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    /// Vertex attribute "position" points to the binded buffer.
    GLuint positionID = glGetAttribLocation(renderingProgramID, "position");
    glEnableVertexAttribArray(positionID);
    // vec3: 3 floats per vertex for the position attribute.
    glVertexAttribPointer(positionID, 3, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);

}


void init() {
//    perlin_noise();
//    exit(EXIT_SUCCESS);

    /// Vertex array.
    GLuint vertexArrayID;
    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);

    /// OpenGL parameters.
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_TEXTURE_2D);

    /// Generate the heightmap texture.
    /// Before shader compilation, as it uses its own shaders.
//    GLuint heightMapID = gen_test_heightmap();
    GLuint heightMapTexID = gen_heightmap();

    /// Set the screen framebuffer back as the rendering target and specify
    /// the transformation from normalized device coordinates to window coordinates.
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, windowWidth, windowHeight);

    /// Compile and install the rendering shaders.
    /// Triangle grid needs the programID to get the "position" attribute ID.
    renderingProgramID = compile_shaders(rendering_vshader, rendering_fshader);
    if(!renderingProgramID)
        exit(EXIT_FAILURE);
    glUseProgram(renderingProgramID);

    /// Bind the heightmap to texture 0.
    const GLuint heightMapTex = 0;
    glActiveTexture(GL_TEXTURE0+heightMapTex);
    glBindTexture(GL_TEXTURE_2D, heightMapTexID);
    GLuint uniformID = glGetUniformLocation(renderingProgramID, "heightMapTex");
    glUniform1i(uniformID, heightMapTex);

    /// Generate a flat and regular triangle grid.
    gen_triangle_grid();

    /// Define light properties and pass them to the shaders.
    vec3 light_pos(-1.0f, 1.0f, 2.0f);
    vec3 Id(1.0f, 1.0f, 1.0f);
    GLuint light_pos_id = glGetUniformLocation(renderingProgramID, "light_pos"); //Given in camera space
    GLuint Id_id = glGetUniformLocation(renderingProgramID, "Id");
    glUniform3fv(light_pos_id, ONE, light_pos.data());
    glUniform3fv(Id_id, ONE, Id.data());

    /// Define the material properties and pass them to the shaders.
    vec3 kd(0.5f, 0.5f, 0.5f);
    GLuint kd_id = glGetUniformLocation(renderingProgramID, "kd");
    glUniform3fv(kd_id, ONE, kd.data());

    /// Pass the size of the base grid to shader.
    GLuint N_id = glGetUniformLocation(renderingProgramID, "N");
    glUniform1f(N_id, N);

    /// Initialize the matrix stack.
    update_matrix_stack(mat4::Identity());

}


void display() {
    //To render only the boundary
    //comment it if you want to render full triangles
//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
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
