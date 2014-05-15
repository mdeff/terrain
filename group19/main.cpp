// Copyright (C) 2014 - LGG EPFL

#include <iostream>
#include <sstream>

#include "common.h"
#include "terrain.h"
#include "shadowmap.h"
#include "skybox.h"
#include "vertices.h"
#include "vertices_grid.h"
#include "vertices_skybox.h"

/// No need to put only this declaration in a separate header.
extern GLuint gen_heightmap();
//extern GLuint gen_test_heightmap();

/// Screen size.
const int windowWidth(1024);
const int windowHeight(768);

/// Textures (heightmap and shadowmap) sizes.
const int textureWidth(1024);
const int textureHeight(768);

/// Instanciate the rendering contexts.
Terrain terrain(windowWidth, windowHeight);
Skybox skybox(windowWidth, windowHeight);
Shadowmap shadowmap(textureWidth, textureHeight);

/// Instanciate the vertices.
Vertices* verticesGrid = new VerticesGrid();
Vertices* verticesSkybox = new VerticesSkybox();

/// Matrices that have to be shared between functions.
static mat4 cameraModelview;
static mat4 lightMVP;
static vec3 lightPositionModel;

/// Projection parameters.
// Horizontal field of view in degrees : amount of "zoom" ("camera lens").
// Usually between 90° (extra wide) and 30° (quite zoomed in).
const float fieldOfView = 45.0f;
// Aspect ratio depends on the window size (for example 4/3 or 1).
const float aspectRatio = float(windowWidth) / float(windowHeight);
// Near clipping plane. Keep as little as possible (precision issues).
const float nearPlane = 0.1f;
// Far clipping plane. Keep as big as possible (usually 10.0f or 100.0f).
const float farPlane  = 100.0f;

/// Camera projection matrix (camera intrinsics).
const mat4 cameraProjection = Eigen::perspective(fieldOfView, aspectRatio, nearPlane, farPlane);

/// Spot light projection matrix.
const mat4 lightProjection = Eigen::perspective(fieldOfView, float(textureWidth)/float(textureHeight), nearPlane, farPlane);


void update_matrix_stack(const mat4& model) {

    /// View matrix (camera extrinsics) (position in world space).
    /// Camera is in the sky, looking down.
    //vec3 camPos(0.0f, -3.0f, 4.0f);
    vec3 camLookAt(-0.3f, 0.1f, 0.5f);
    vec3 camUp(0.0f, 0.0f, 1.0f);
    /// Camera is right on top, comparison with light position.

    //camPos = vec3(0.0, 0.0, 5.0);
    //camLookAt = vec3(0.0, 0.0, 0.0);
    //camUp = vec3(1.0, 0.0, 0.0);

//    camPos = vec3(0.0, 0.0, 5.0);
//    camLookAt = vec3(0.0, 0.0, 0.0);
//    camUp = vec3(1.0, 0.0, 0.0);

    /// Camera is in a corner, looking down to the terrain.
    //vec3 camPos(2.0f, -2.0f, 2.5f);
    vec3 camPos(0.9f, -0.8f,3.0f); // Close texture view.
    //vec3 camPos(0.8f, 1.2f, 2.0f);
    /// View from center.
//    vec3 camPos(0.9f, -0.8f, 1.0f);
//    vec3 camLookAt(-0.3f, 0.1f, 0.5f);
//    vec3 camUp(0.0f, 0.0f, 1.0f);
    mat4 view = Eigen::lookAt(camPos, camLookAt, camUp);

    /// Assemble the "Model View" matrix.
    cameraModelview = view * model;

}


/// Key press callback.
void GLFWCALL keyboard_callback(int key, int action) {

    /// Distance from center (0,0,0) to sun.
    const float r = 2.0f;

    if(action == GLFW_PRESS) {

        //std::cout << "Pressed key : " << key << std::endl;

        /// 49 corressponds to 1 (keyboard top), 57 to 9.
        if(key >= 49 && key <= 57) {

            /// Angle from 0° (key 1) to 90° (key 9).
            float theta = M_PI / 16.0f * float(key-49);
//            float theta = M_PI/4.0f + M_PI / 16.0f * float(key-49);

            /// Position from sunrise (-r,0,0) to noon (0,0,r).
            lightPositionModel = vec3(-std::cos(theta)*r, 0.0, std::sin(theta)*r);

            /// Light source position (model coordinates).
            const vec3 lightLookAt(0.0, 0.0, 0.0);
            const vec3 lightUp(0.0, 1.0, 0.0);
            mat4 lightView = Eigen::lookAt(lightPositionModel, lightLookAt, lightUp);

            /// Moving light source position (model coordinates).
        //    static float t = 0.0f;
        //    t += 0.01f;
        //    lightPositionModel = vec3(3.0*std::sin(t), 3.0, 3.0*std::cos(t));
        //    vec3 lightLookAt(0.0, 0.0, 0.0);
        //    vec3 lightUp(0.0, 1.0, 0.0);
        //    mat4 lightView = Eigen::lookAt(lightPositionModel, lightLookAt, lightUp);

            /// Assemble the lightMVP matrix for a spotlight source.
            lightMVP = lightProjection * lightView;



            /// Assemble the "Model View" matrix.
//            cameraModelview = lightView;
        }
    }
}


void init() {
	
    /// OpenGL parameters.
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    //glEnable(GL_CULL_FACE);

    /// Generate the heightmap texture.
    //GLuint heightMapTexID = gen_test_heightmap();
    GLuint heightMapTexID = gen_heightmap();

    /// Set the screen framebuffer back as the rendering target and specify
    /// the transformation from normalized device coordinates to window coordinates.
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, windowWidth, windowHeight);

    /// Generate the vertices.
    verticesGrid->generate();
    verticesSkybox->generate();

    /// Initialize the rendering contexts.
    GLuint shadowMapTexID = shadowmap.init(verticesGrid, heightMapTexID);
    terrain.init(verticesGrid, heightMapTexID, shadowMapTexID);
    skybox.init(verticesSkybox);

    std::cerr << heightMapTexID << " " << shadowMapTexID << std::endl;

    /// Initialize the matrix stack.  	
	update_matrix_stack(mat4::Identity());

    /// Initialize the light position.
    keyboard_callback(49, GLFW_PRESS);

}


void display() {

    /// Measure and print FPS (every second).
	static double lastTime = glfwGetTime();
    static int nbFrames = 0;
    double currentTime = glfwGetTime();
    nbFrames++;
    if(currentTime - lastTime >= 1.0) {
        std::cout << nbFrames << " FPS" << std::endl;
        nbFrames = 0;
        lastTime += 1.0;
    }

    /// Uncomment to render only the boundary (not full triangles).
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    /// Render shadowmap, terrain and skybox.
    shadowmap.draw(lightMVP);
    terrain.draw(cameraProjection, cameraModelview, lightMVP, lightPositionModel);
    skybox.draw(cameraProjection, cameraModelview);
//    shadowmap.draw(lightMVP);

}


int main(int, char**) {
    glfwInitWindowSize(windowWidth, windowHeight);
    glfwCreateWindow("Project - Group 19");	
    glfwDisplayFunc(display);
    init();
    glfwTrackball(update_matrix_stack);
    glfwSetKeyCallback(keyboard_callback);
    glfwMainLoop();
    return EXIT_SUCCESS;    
}
