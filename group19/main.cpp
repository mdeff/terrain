// Copyright (C) 2014 - LGG EPFL

#include <iostream>
#include <sstream>

#include "common.h"
#include "terrain.h"
#include "skybox.h"

extern GLuint gen_heightmap();

// Rendering contexts.
Terrain terrain;
Skybox skybox;

/// Screen size.
const int windowWidth(1024);
const int windowHeight(768);

/// Matrices that have to be shared between rendering contexts.
static mat4 projection;
static mat4 modelview;


void update_matrix_stack(const mat4& model) {

    /// Define projection matrix (camera intrinsics)
    projection = Eigen::perspective(45.0f, 4.0f/3.0f, 0.1f, 100.0f);

    /// Define the view matrix (camera extrinsics)
    vec3 cam_look(0.1f, 0.1f, 0.5f);
    /// Camera is in the sky, looking down.
//    vec3 cam_pos(0.0f, 0.0f, 5.0f);
//    vec3 cam_up(0.0f, 1.0f, 0.0f);
    /// Camera is in a corner, looking down to the terrain.
    vec3 cam_pos(2.5f, -2.0f, 1.8f);
    //vec3 cam_pos(2.0f, -2.0f, 1.5f);
    //vec3 cam_pos(0.7f, -0.7f, 0.3f); // Close texture view.
    //vec3 cam_pos(0.8f, 1.2f, 2.0f);
    vec3 cam_up(0.0f, 0.0f, 1.0f);
    static mat4 view = Eigen::lookAt(cam_pos, cam_look, cam_up);

    /// Assemble the "Model View" matrix
    modelview = view * model;

}




void init() {
	
    /// OpenGL parameters.
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_TEXTURE_2D);
    //glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);


    /// Generate the heightmap texture.
    /// Before shader compilation, as it uses its own shaders.
//    GLuint heightMapID = gen_test_heightmap();
    GLuint heightMapTexID = gen_heightmap();

    /// Set the screen framebuffer back as the rendering target and specify
    /// the transformation from normalized device coordinates to window coordinates.
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, windowWidth, windowHeight);

    // Initialize the rendering contexts.
    terrain.init(heightMapTexID);
    skybox.init();

    /// Initialize the matrix stack.  	
	update_matrix_stack(mat4::Identity());

}


void display() {

    //To render only the boundary
    //comment it if you want to render full triangles
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
    // Draw terrain and skybox.
    terrain.draw(projection, modelview);
    skybox.draw(projection, modelview);

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
