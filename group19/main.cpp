// Copyright (C) 2014 - LGG EPFL

#include <iostream>
#include <sstream>

#include "common.h"
#include "terrain.h"
#include "skybox.h"
#include "shadowmap.h"
#include "watermap.h"

extern GLuint gen_heightmap();

/// Screen size.
const int windowWidth(1024);
const int windowHeight(768);

// Rendering contexts.
Terrain terrain(windowWidth, windowHeight);
Skybox skybox(windowWidth, windowHeight);
Shadowmap shadowmap(1024, 1024);  ///< Shadow map texture size.
//Watermap watermap(windowWidth, windowHeight);


/// Matrices that have to be shared between rendering contexts.
static mat4 projection;
static mat4 modelview;


void update_matrix_stack(const mat4& model) {

    /// Define projection matrix (camera intrinsics)
    projection = Eigen::perspective(45.0f, 4.0f/4.0f, 0.1f, 100.0f);

    /// Define the view matrix (camera extrinsics)
    vec3 cam_look(-0.3f, 0.1f, 0.5f);
    /// Camera is in the sky, looking down.
//    vec3 cam_pos(0.0f, 0.0f, 5.0f);
//    vec3 cam_up(0.0f, 1.0f, 0.0f);
    /// Camera is in a corner, looking down to the terrain.
    vec3 cam_pos(0.9f, -0.8f, 1.0f);
    //vec3 cam_pos(2.0f, -2.0f, 2.5f);
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
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
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
	//watermap.init(heightMapTexID, windowWidth, windowHeight);

    // Terrain and Shadowmap contexts share the same vertices.
    GLuint vertexBufferID, elementBufferID;
    terrain.get_buffer_IDs(vertexBufferID, elementBufferID);
    shadowmap.set_buffer_IDs(vertexBufferID, elementBufferID);
    shadowmap.init(heightMapTexID, terrain.get_vertexarray_ID());

    /// Initialize the matrix stack.  	
	update_matrix_stack(mat4::Identity());
}

void display() {

	//to measure FPS
	static double lastTime = glfwGetTime();
	static int nbFrames = 0;
    // Measure speed
    double currentTime = glfwGetTime();
    nbFrames++;
    if ( currentTime - lastTime >= 1.0 ){ // compute and display amount of frame every sc
        printf("%f frames/s\n", double(nbFrames));
        nbFrames = 0;
        lastTime += 1.0;
    }

    //To render only the boundary
    //comment it if you want to render full triangles
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    shadowmap.draw(projection, modelview);

    // Draw terrain and skybox.
    terrain.draw(projection, modelview);
    skybox.draw(projection, modelview);
	
	//watermap.draw(projection, modelview);

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
