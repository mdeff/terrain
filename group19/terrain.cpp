
#include "terrain.h"
#include "vertices.h"

#include <iostream>

#include <GL/glew.h>
#include <GL/glfw.h>
#include "opengp.h"

#include "terrain_vshader.h"
#include "terrain_fshader.h"


Terrain::Terrain(unsigned int windowWidth, unsigned int windowHeight, unsigned int textureWidth, unsigned int textureHeight) :
    RenderingContext(windowWidth, windowHeight),
    _textureWidth(textureWidth),
    _textureHeight(textureHeight) {
}


GLuint Terrain::init(Vertices* vertices, GLuint heightMapTexID, GLuint shadowMapTexID) {

    /// Common initialization.
    /// Render to FBO by default.
    RenderingContext::init(vertices, terrain_vshader, terrain_fshader, NULL, "vertexPosition2DWorld", -1);

    /// Bind the heightmap and shadowmap to textures 0 and 1.
    set_texture(0, heightMapTexID, "heightMapTex", GL_TEXTURE_2D);
    set_texture(1, shadowMapTexID, "shadowMapTex", GL_TEXTURE_2D);

    /// Load material textures and bind them to textures 2 - 7.
    set_texture(2, -1, "sandTex", GL_TEXTURE_2D);
    load_texture("../../textures/sand.tga");
    set_texture(3, -1, "iceMoutainTex", GL_TEXTURE_2D);
    load_texture("../../textures/dordona_range.tga");
    set_texture(4, -1, "treeTex", GL_TEXTURE_2D);
    load_texture("../../textures/Mossy_Rock.tga");
    set_texture(5, -1, "stoneTex", GL_TEXTURE_2D);
    load_texture("../../textures/Fault_Zone.tga");
    set_texture(6, -1, "underWaterTex", GL_TEXTURE_2D);
    load_texture("../../textures/under_water.tga");
    set_texture(7, -1, "snowTex", GL_TEXTURE_2D);
	load_texture("../../textures/snow.tga");
  
    /// Define light properties and pass them to the shaders.
    vec3 Ia(1.0f, 1.0f, 1.0f);
    vec3 Id(1.0f, 1.0f, 1.0f);
    vec3 Is(1.0f, 1.0f, 1.0f);
    GLuint _IaID = glGetUniformLocation(_programID, "Ia");
    GLuint _IdID = glGetUniformLocation(_programID, "Id");
    GLuint _IsID = glGetUniformLocation(_programID, "Is");
    glUniform3fv( _IaID, 1, Ia.data());
    glUniform3fv( _IdID, 1, Id.data());
    glUniform3fv( _IsID, 1, Is.data());

    /// Set uniform IDs.
    _viewID = glGetUniformLocation(_programID, "view");
    _projectionID = glGetUniformLocation(_programID, "projection");
    _lightViewProjectionID = glGetUniformLocation(_programID, "lightViewProjection");
    _lightPositionWorldID = glGetUniformLocation(_programID, "lightPositionWorld");
    _clipID = glGetUniformLocation(_programID, "clip");

    /// The terrain will be rendered a second time from a flipped camera
    /// point of view to a texture which is attached to a FBO.
    GLuint flippedTerrainTexID;
    glGenTextures(1, &flippedTerrainTexID);
    glBindTexture(GL_TEXTURE_2D, flippedTerrainTexID);

    /// Empty image (no data), three color components, clamped [0,1] 32 bits float.
    /// Same size as the screen : no need to change the view port, same projection matrix.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_FLOAT, 0);

    /// Clamp texture coordinates to the [0,1] range. It is wrapped by default
    /// (GL_REPEAT), which creates artifacts at the terrain borders.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    /// Simple filtering (needed).
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // Filtering
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    //// Nice trilinear filtering.
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//    glGenerateMipmap(GL_TEXTURE_2D);

    /// Attach the created texture to the first color attachment point.
    /// The texture becomes the fragment shader first output buffer.
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, flippedTerrainTexID, 0);
    GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, drawBuffers);

    /// Create a depth buffer for the FBO.
    GLuint depthRenderbufferID;
    glGenRenderbuffers(1, &depthRenderbufferID);
    glBindRenderbuffer(GL_RENDERBUFFER, depthRenderbufferID);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, _width, _height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthRenderbufferID);

    /// Check that our framebuffer object (FBO) is complete.
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Flipped terrain framebuffer not complete." << std::endl;
        exit(EXIT_FAILURE);
    }

    /// Return the flipped terrain texture ID (for the water).
    return flippedTerrainTexID;

}


void Terrain::draw(const mat4& projection, const mat4& view,
                   const mat4& lightViewProjection, const vec3& lightPositionWorld) const {

    /// This could have been used instead of the clip uniform. But some drivers ignore it.
    glEnable(GL_CLIP_DISTANCE0);

    /// Common drawing. 
    RenderingContext::draw();

    /// Flip the terrain by multiplying the Z coordinate by -1 in world space.
    mat4 flip = mat4::Identity();
    flip(2,2) = -1.0f;
    mat4 viewFlip = view * flip;

    /// Update the content of the uniforms.
    glUniformMatrix4fv(_projectionID, 1, GL_FALSE, projection.data());
    glUniformMatrix4fv(_lightViewProjectionID, 1, GL_FALSE, lightViewProjection.data());
    glUniform3fv(_lightPositionWorldID, 1, lightPositionWorld.data());

    /// Clear the texture binded to FBO.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /// Render the terrain from camera point of view to default framebuffer.
    glUniform1f(_clipID, 1.0);
    glUniformMatrix4fv(_viewID, 1, GL_FALSE, viewFlip.data());
    _vertices->draw();

    /// Render the terrain from camera point of view to default framebuffer.
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glUniform1f(_clipID, 0.0);
    glUniformMatrix4fv(_viewID, 1, GL_FALSE, view.data());
    _vertices->draw();

    glDisable(GL_CLIP_DISTANCE0);

}


GLuint Terrain::load_texture(const char * imagepath) const {

    // Read the file, call glTexImage2D with the right parameters
    if (glfwLoadTexture2D(imagepath, 0)) {
        // Nice trilinear filtering.
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Cannot load texture file : " << imagepath << std::endl;
        exit(EXIT_FAILURE);
    }

}
