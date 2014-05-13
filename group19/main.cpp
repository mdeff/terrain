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

/// Textures (heightmap and shadowmap) sizes.
const int textureWidth(1024);
const int textureHeight(1024);

/// Instanciate the rendering contexts.
Terrain terrain(windowWidth, windowHeight);
Skybox skybox(windowWidth, windowHeight);
Shadowmap shadowmap(textureWidth, textureHeight);

/// Matrices that have to be shared between rendering contexts.
static mat4 projection;
static mat4 modelview;
static mat4 lightMVP;
static vec3 lightPositionModel;


void update_matrix_stack(const mat4& model) {

    /// Camera projection.
    // The horizontal Field of View, in degrees : the amount of "zoom". Think "camera lens". Usually between 90° (extra wide) and 30° (quite zoomed in)
    float fieldOfView = 45.0f;
    // Aspect Ratio. Depends on the size of your window. Notice that 4/3 == 800/600 == 1280/960
    float aspectRatio = float(windowWidth)/float(windowHeight);
    // Near clipping plane. Keep as little as possible, or you'll get precision issues.
    float nearPlane = 0.1f;
    // Far clipping plane. Keep as big as possible.
    float farPlane  = 100.f;

    /// Define projection matrix (camera intrinsics)
    // Projection matrix : 45° Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
    projection = Eigen::perspective(fieldOfView, aspectRatio, nearPlane, farPlane);


    /// Define the view matrix (camera extrinsics) (position in world space).
    /// Camera is in the sky, looking down.
    vec3 cam_pos(0.0f, -3.0f, 4.0f);
    vec3 cam_up(0.0f, 1.0f, 0.0f);
    vec3 cam_look(0.0f, 0.0f, 0.0f);
    /// Camera is in a corner, looking down to the terrain.
    //vec3 cam_pos(2.0f, -2.0f, 2.5f);
    //vec3 cam_pos(0.7f, -0.7f, 0.3f); // Close texture view.
    //vec3 cam_pos(0.8f, 1.2f, 2.0f);
    /// View from center.
//    vec3 cam_pos(0.9f, -0.8f, 1.0f);
//    vec3 cam_up(0.0f, 0.0f, 1.0f);
//    vec3 cam_look(-0.3f, 0.1f, 0.5f);
    mat4 view = Eigen::lookAt(cam_pos, cam_look, cam_up);

    /// Assemble the "Model View" matrix
    modelview = view * model;



    /// Spot light projection.
    aspectRatio = float(textureWidth)/float(textureHeight);
    mat4 lightProjection = Eigen::perspective(fieldOfView, aspectRatio, nearPlane, farPlane);

    /// Light source position (model coordinates).
    lightPositionModel = vec3(1.0, 1.0, 10.0);
    vec3 lightAt(0.0,0.0,0.0);
    vec3 lightUp(0.0,1.0,0.0);
    mat4 lightView = Eigen::lookAt(lightPositionModel, lightAt, lightUp);

    /// Assemble the lightMVP matrix for a spotlight source.
    lightMVP = lightProjection * lightView * model;

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
//    GLuint heightMapTexID = gen_test_heightmap();
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
    // TODO : generate vertices out of any context ?
    GLuint vertexBufferID, elementBufferID;
    terrain.get_buffer_IDs(vertexBufferID, elementBufferID);
    shadowmap.set_buffer_IDs(vertexBufferID, elementBufferID);

    // Initialize the rendering context.
    shadowmap.init(heightMapTexID, terrain.get_vertexarray_ID());

    // The shadow map texture is used by the two rendering contexts.
    // Texture 8 in Terrain and texture 1 in Shadowmap.
    GLuint textureID = shadowmap.get_texture_ID(1);
    terrain.set_texture_ID(8, textureID);

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

    shadowmap.draw(projection, modelview, lightMVP);

    // Draw terrain and skybox.
    terrain.draw(projection, modelview, lightMVP, lightPositionModel);
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
