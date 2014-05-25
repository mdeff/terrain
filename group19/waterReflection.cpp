#include "vertices.h"
#include "waterReflection.h"

#include <cstdlib>
#include <iostream>

#include <GL/glew.h>
#include <GL/glfw.h>
#include "opengp.h"

#include "watereflection_vshader.h"
#include "watereflection_fshader.h"



WaterReflection::WaterReflection(unsigned int width, unsigned int height) :
    RenderingContext(width, height) {
}

GLuint WaterReflection::init(Vertices* vertices, GLuint heightMapTexID) {

	/// Common initialization.
    RenderingContext::init(vertices, watereflection_vshader, watereflection_fshader, NULL, "vertexPosition2DWorld", -1);

	/// Bind the heightmap to texture 0.
    set_texture(0, heightMapTexID, "heightMapTex", GL_TEXTURE_2D);

	/// Load material textures and bind them to textures 2 - 7.
    set_texture(2, -1, "sandTex", GL_TEXTURE_2D);
    load_texture("../../textures/sand.tga");
    set_texture(3, -1, "iceMoutainTex", GL_TEXTURE_2D);
    load_texture("../../textures/dordona_range.tga");
    set_texture(4, -1, "treeTex", GL_TEXTURE_2D);
    load_texture("../../textures/forest.tga");
    set_texture(5, -1, "stoneTex", GL_TEXTURE_2D);
    load_texture("../../textures/stone_2.tga");
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



	//  Create and bind to texture 1 the texture which will contain the
    // scene viewed from reflected camera
	GLuint reflectionID;
    glGenTextures(1, &reflectionID);
    glBindTexture(GL_TEXTURE_2D, reflectionID);

	//Empty image (no data), three color components, unclamped 32 bits float.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, _width, _height, 0, GL_RGB, GL_FLOAT, 0);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    //Simple filtering (needed).
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    //Attach the created texture to the depth attachment point.
    //Hardware will copy pixel depth to the texture.
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, reflectionID, 0);

    GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(sizeof(drawBuffers)/sizeof(GLenum), drawBuffers);

    //Check that our framebuffer is complete.
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Reflection texture framebuffer not complete." << std::endl;
        exit(EXIT_FAILURE);
    }
	
 
	 /// Set uniform IDs.
    _viewID = glGetUniformLocation(_programID, "view");
    _projectionID = glGetUniformLocation(_programID, "projection");
    _lightOffsetMVPID = glGetUniformLocation(_programID, "lightOffsetMVP");
    _lightPositionWorldID = glGetUniformLocation(_programID, "lightPositionWorld");
    
    /// Return the reflection texture ID
    return reflectionID;
}

void WaterReflection::draw(const mat4& projection, const mat4& view,
                   const mat4& lightMVP, const vec3& lightPositionWorld) const {

	
    /// Common drawing.
    RenderingContext::draw();

	//flip the camera
	//float groundHeight = -0.018f;
	float clipEqn[4] = { 0.0, 0.0, 1.0, -GROUND_HEIGHT};
    
    //glEnable(GL_CLIP_DISTANCE0);
	GLuint _clipPlaneID = glGetUniformLocation(_programID, "clipPlane");
    glUniform4fv(_clipPlaneID, 1, &clipEqn[0]);

    
    /// Update the content of the uniforms.
    glUniformMatrix4fv( _viewID, 1, GL_FALSE, view.data());
    glUniformMatrix4fv( _projectionID, 1, GL_FALSE, projection.data());
    glUniformMatrix4fv( _lightMVPID, 1, GL_FALSE, lightMVP.data());
    glUniform3fv( _lightPositionWorldID, 1, lightPositionWorld.data());
  
	
    /// Map from light-coordinates in (-1,-1)x(1,1) to texture
    /// coordinates in (0,0)x(1,1).
    mat4 offsetMatrix;
    offsetMatrix <<
            0.5f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.5f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.5f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f;
    mat4 lightOffsetMVP = offsetMatrix * lightMVP;
    glUniformMatrix4fv( _lightOffsetMVPID, 1, GL_FALSE, lightMVP.data());

    /// Must not clear the buffer since it will delete the pre-drawn terrain
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /// Render the terrain from camera point of view to default framebuffer.
    _vertices->draw();

	//glDisable(GL_CLIP_DISTANCE0);

}

GLuint WaterReflection::load_texture(const char * imagepath) const {

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
