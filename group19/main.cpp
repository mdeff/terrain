// Copyright (C) 2014 - LGG EPFL

#include <iostream>
#include <sstream>
#include <map>
#include <string>

#include "common.h"
#include "rendering_simple.h"
#include "post_processing.h"
#include "heightmap.h"
#include "shadowmap.h"
#include "water.h"
#include "skybox.h"
#include "particles_control.h"
#include "particles_render.h"
#include "terrain.h"
#include "camera_control.h"
#include "camera_path_controls.h"
#include "vertices.h"
#include "vertices_quad.h"
#include "vertices_grid.h"
#include "vertices_skybox.h"
#include "vertices_camera_path.h"
#include "vertices_camera_pictorial.h"


/// Number of parallel views.
GLuint Nviews(2);

/// OpenGL object references (global by definition, part of the OpenGL context).
std::map<std::string, unsigned int> framebufferIDs;
std::map<std::string, unsigned int> textureIDs;
std::map<std::string, unsigned int> viewIDs;

/// Screen (and rendering framebuffers) size.
const unsigned int windowWidth(1024);
const unsigned int windowHeight(768);

/// Textures (heightmap and shadowmap) sizes.
const unsigned int textureWidth(1024);
const unsigned int textureHeight(1024);

/// Unified width and height ?
//const unsigned int width(1024);
//const unsigned int height(1024);

/// Number of particles on the side. That makes nParticlesSide^3 particles.
const unsigned int nParticlesSide(20);

/// This is the sole rendering context that renders directly to the screen.
PostProcessing postProcessing(windowWidth, windowHeight);

/// Instanciate the rendering contexts that render to the screen.
Skybox skybox(windowWidth, windowHeight);
Terrain terrain(windowWidth, windowHeight);
RenderingSimple cameraPictorial(windowWidth, windowHeight);
RenderingSimple cameraPath(windowWidth, windowHeight);
CameraPathControls cameraPathControls(windowWidth, windowHeight);
ParticlesRender particlesRender(windowWidth, windowHeight, nParticlesSide);

/// Instanciate the rendering contexts that render to FBO.
Shadowmap shadowmap(textureWidth, textureHeight);
ParticlesControl particlesControl(nParticlesSide);

Water water(windowWidth, windowHeight);

/// Camera position controller.
CameraControl cameraControl;

/// Instanciate the vertices.
Vertices* verticesQuad = new VerticesQuad();
Vertices* verticesGrid = new VerticesGrid();
Vertices* verticesSkybox = new VerticesSkybox();
VerticesCameraPath* verticesCameraPath = new VerticesCameraPath();
VerticesCameraPath* verticesCameraPathControls = new VerticesCameraPath();
VerticesCameraPictorial* verticesCameraPictorial = new VerticesCameraPictorial();

/// Matrices that have to be shared between functions.
static mat4 lightViewProjection;
static vec3 lightPositionWorld;

//flip the camera for reflection effect
static mat4 flippedcameraView;



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


/// Key press callback.
void GLFWCALL keyboard_callback(int key, int action) {

    /// Distance from center (0,0,0) to sun.
    const float r = 3.0f;
	std::cout << "Pressed key : " << key << std::endl;

    if(action == GLFW_PRESS) {

       // std::cout << "Pressed key : " << key << std::endl;

        /// 49 corressponds to 1, 57 to 9 (keyboard top keys).
        if(key >= 49 && key <= 57) {

            /// Angle from 0° (key 1) to 90° (key 9).
            float theta = M_PI / 8.0f * float(key-49);

            /// Position from sunrise (-r,0,0) to noon (0,0,r).
            lightPositionWorld = vec3(-std::cos(theta)*r, 0.0, std::sin(theta)*r);

            /// Light source position (model coordinates).
            const vec3 lightLookAt(0.0, 0.0, 0.0);
            const vec3 lightUp(0.0, 1.0, 0.0);
            mat4 lightView = Eigen::lookAt(lightPositionWorld, lightLookAt, lightUp);

            /// Assemble the lightViewProjection matrix for a spotlight source.
            lightViewProjection = lightProjection * lightView;
        }
    }
    cameraControl.handleCameraControls(key, action);
}


void gen_rendering_framebuffers(GLuint framebufferIDs[], GLuint colorTexIDs[], unsigned int N) {

    /// Each FBO will have an attached texture for rendering and a renderbuffer
    /// depth buffer.

    /// Number of samples for multi-sampling.
    const unsigned int samples = 4;

    /// Generate framebuffers and renderbuffers.
    GLuint* colorBufIDs = new GLuint[N];
    GLuint* depthBufIDs = new GLuint[N];
    glGenRenderbuffers(N, colorBufIDs);
    glGenTextures(N, colorTexIDs);
    glGenRenderbuffers(N, depthBufIDs);

    for(int k=0; k<N; ++k) {

        glBindFramebuffer(GL_FRAMEBUFFER, framebufferIDs[k]);

        /// Multi-sample color buffer.
//        glBindRenderbuffer(GL_RENDERBUFFER, colorBufIDs[k]);
//        glRenderbufferStorageMultisampleEXT(GL_RENDERBUFFER_EXT, samples, GL_RGBA8, windowWidth, windowHeight);
//        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, colorBufIDs[k]);
//        GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0};
//        glDrawBuffers(1, drawBuffers);

        /// Multi-sample color texture (will be sampled by post-processing shader).
        glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, colorTexIDs[k]);
        glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, samples, GL_RGBA8, windowWidth, windowHeight, GL_FALSE);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, colorTexIDs[k], 0);
        GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0};
        glDrawBuffers(1, drawBuffers);

//        glBindTexture(GL_TEXTURE_2D, colorTexIDs[k]);
//        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, windowWidth, windowHeight, 0, GL_RGBA, GL_FLOAT, NULL);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorTexIDs[k], 0);
//        GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0};
//        glDrawBuffers(1, drawBuffers);

        /// Multi-sample depth buffer (will never be sampled, more efficient).
        glBindRenderbuffer(GL_RENDERBUFFER, depthBufIDs[k]);
        glRenderbufferStorageMultisample(GL_RENDERBUFFER, samples, GL_DEPTH_COMPONENT, windowWidth, windowHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBufIDs[k]);

        /// Create and attach a depth buffer for the FBO.
//        glBindRenderbuffer(GL_RENDERBUFFER, depthBufIDs[k]);
//        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, windowWidth, windowHeight);
//        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBufIDs[k]);

        /// Check that FBO is complete.
        if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
            std::cerr << "Rendering framebuffer " << k << " not complete." << std::endl;
            exit(EXIT_FAILURE);
        }
    }

    /// Free heap memory.
    delete[] colorBufIDs, depthBufIDs;

}


void init() {
	
    /// OpenGL parameters.
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_TEXTURE_2D);
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
	glEnable(GL_CLIP_DISTANCE0);
    //glEnable(GL_CULL_FACE);

    /// Generate the vertices.
    verticesQuad->generate();
    verticesGrid->generate();
    verticesSkybox->generate();
    verticesCameraPath->generate();
    verticesCameraPathControls->generate();
    verticesCameraPictorial->generate();

    /// Generate the heightmap texture.
    Heightmap heightmap(textureWidth, textureHeight);
    GLuint heightMapTexID = heightmap.init(verticesQuad);
	heightmap.draw();
    heightmap.clean();
//    verticesQuad->clean();
//    delete verticesQuad;

    /// Generate OpenGL global objects.
    GLuint framebufferIDs_tmp[2*Nviews];
    glGenFramebuffers(2*Nviews, framebufferIDs_tmp);
    framebufferIDs["controllerView"]  = framebufferIDs_tmp[0];
    framebufferIDs["cameraView"]      = framebufferIDs_tmp[1];
    framebufferIDs["waterReflection"] = framebufferIDs_tmp[2];


    /// Two rendering framebuffers and two view matrices.
    /// 1) Overall view for control purpose.
    /// 2) Camera actual view.
    /// No more direct drawing to the default framebuffer. All drawings go
    /// to textures and Display arranges the textures on screen.
    GLuint renderedTexIDs[Nviews];
    gen_rendering_framebuffers(framebufferIDs_tmp, renderedTexIDs, Nviews);
    postProcessing.init(verticesQuad, renderedTexIDs);

    /// Initialize the rendering contexts.
    GLuint shadowMapTexID = shadowmap.init(verticesGrid, heightMapTexID);
    GLuint flippedTerrainTexID = terrain.init(verticesGrid, heightMapTexID, shadowMapTexID);
    skybox.init(verticesSkybox);

    // Grid or quad : interpolation ?
    water.init(verticesGrid, flippedTerrainTexID);
//    water.init(verticesQuad, flippedTerrainTexID);

    /// Pass the particles position textures from control to render.
    GLuint particlePosTexID[2];
    particlesControl.init(verticesQuad, particlePosTexID);
    particlesRender.init(particlePosTexID);

    /// CameraPath is a rendering object.
    /// Camera is able to change the rendered vertices.
    cameraControl.init(verticesCameraPath, verticesCameraPathControls, heightMapTexID);
    cameraPictorial.init(verticesCameraPictorial);
    cameraPath.init(verticesCameraPath);
    cameraPathControls.init(verticesCameraPathControls);

    /// Initialize the light position.
    keyboard_callback(50, GLFW_PRESS);

}


void display() {

    /// Measure and print FPS (every 10 seconds).
	static double lastTime = glfwGetTime();
    static int nbFrames = 0;
    double currentTime = glfwGetTime();
    nbFrames++;
    if(currentTime - lastTime >= 10.0) {
        std::cout << float(nbFrames)/10.0f << " FPS" << std::endl;
        nbFrames = 0;
        lastTime = currentTime;
    }

    /// Time elapsed between two frames (for constant movement speed).
    static double lastFrameTime = glfwGetTime();
    float deltaT = float(currentTime - lastFrameTime);
    lastFrameTime = currentTime;

    /// Control the camera position.
    /// Should come before rendering as it updates the view transformation matrix.
    mat4 views[Nviews], cameraPictorialModel;
    int selectedControlPoint;
    cameraControl.updateCameraPosition(views, cameraPictorialModel, selectedControlPoint);

    /// Generate the shadowmap.
    /// Should come before rendering as it updates the light transformation matrices.
    shadowmap.draw(lightViewProjection);

    /// Uncomment to render only the boundaries (not full triangles).
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    /// Render opaque primitives on screen.
    /// These are also rendered in a texture for water reflection.
    terrain.draw(cameraProjection, views, lightViewProjection, lightPositionWorld);
    skybox.draw(cameraProjection, views);

    /// Render opaque primitives on screen.
    cameraPictorial.draw(cameraProjection, views, cameraPictorialModel, vec3(1,1,0));
    cameraPath.draw(cameraProjection, views, mat4::Identity(), vec3(0,1,0));
    cameraPathControls.draw(cameraProjection, views, lightPositionWorld, selectedControlPoint, deltaT);

    water.draw(cameraProjection, views, lightViewProjection, lightPositionWorld);

    /// Render the translucent primitives last. Otherwise opaque objects that
    /// may be visible behind get discarded by the depth test.
    /// First control particle positions, then render them on screen.
    particlesControl.draw(deltaT);
    particlesRender.draw(cameraProjection, views);


    /// Perform anti-aliasing, assemble the views and fill the real window.
    postProcessing.draw();

}


/// Wrapper (do not accept class method type).
void trackball(const mat4& model) {
    cameraControl.trackball(model);
}


int main(int, char**) {
    glfwInitWindowSize(windowWidth, windowHeight);
    glfwCreateWindow("EPFL - Computer Graphics - Project - Group 19");
    glfwDisplayFunc(display);
    init();
    glfwTrackball(trackball);
    glfwSetKeyCallback(keyboard_callback);
    glfwMainLoop();
    return EXIT_SUCCESS;    
}
