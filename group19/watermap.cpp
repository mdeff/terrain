#include "watermap.h"
#include "vertices.h"



#include <cstdlib>
#include <iostream>

#include <GL/glew.h>
#include <GL/glfw.h>
#include "opengp.h"

#include "watermap_vshader.h"
#include "watermap_fshader.h"

Watermap::Watermap(unsigned int width, unsigned int height) :
    RenderingContext(width, height){
	 //initialization the reflection class as well
	
}

	
void Watermap::init(Vertices* vertices , GLuint reflectionID) {

  	 /// Common initialization.
    RenderingContext::init(vertices, watermap_vshader, watermap_fshader, NULL, "vertexPosition2DWorld", 0);

	//bind the reflection tex to texture 0
	set_texture(0, reflectionID, "reflectionTex", GL_TEXTURE_2D);
	
	/* Load texture for water surface */
	set_texture(1, -1, "waterNormalMap", GL_TEXTURE_2D);
    load_texture("../../textures/water_normal_map_2.tga");
  
    set_texture(2, -1, "riverSurfaceMap", GL_TEXTURE_2D);
    load_texture("../../textures/water_2.tga");


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
    _lightPositionWorldID = glGetUniformLocation(_programID, "lightPositionWorld");
    _timeID = glGetUniformLocation(_programID, "time");
    _lightViewProjectionID = glGetUniformLocation(_programID, "lightViewProjection");
}


void Watermap::draw(const mat4& projection, const mat4& view,
                   const mat4& lightViewProjection, const vec3& lightPositionWorld) const {
	 // Enable blending
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	
    /// Common drawing. 
    RenderingContext::draw();

    /// Update the content of the uniforms.
    glUniformMatrix4fv(_viewID, 1, GL_FALSE, view.data());
    glUniformMatrix4fv( _projectionID, 1, GL_FALSE, projection.data());
    glUniformMatrix4fv( _lightViewProjectionID, 1, GL_FALSE, lightViewProjection.data());
    glUniform3fv( _lightPositionWorldID, 1, lightPositionWorld.data());
  
	/// Time value which animates water
    static float time = 0;
    glUniform1f(_timeID, int(time++)%5000);

    /// Must not clear the buffer since it will delete the pre-drawn terrain
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /// Render the terrain from camera point of view to default framebuffer.
    _vertices->draw();


	glDisable(GL_BLEND);
}


GLuint Watermap::load_texture(const char * imagepath) const {

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
