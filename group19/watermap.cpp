
#include "common.h"
#include "watermap.h"
#include "terrain.h"
#include "skybox.h"
#include "shadowmap.h"

#include <iostream>

#include <GL/glew.h>
#include <GL/glfw.h>
#include "opengp.h"

#include "watermap_vshader.h"
#include "watermap_fshader.h"

const unsigned int nVertices = 6;

Watermap::Watermap(unsigned int width, unsigned int height) :
    RenderingContext(width, height) {
}

void RenderRefractionAndDepth(GLuint heightMapTexID,unsigned int windowWidth, unsigned int windowHeight)
{
	glViewport(0,0, windowWidth, windowHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	//normal pointing along negative y
	double plane[4] = {0.0, -1.0, 0.0, 0.0};
	glEnable(GL_CLIP_PLANE0);
	glClipPlane(GL_CLIP_PLANE0, plane);

	//regenerate everything flipped arroung plane  -- not working 
	shadowmap.init(heightMapTexID);
	terrain.init(heightMapTexID);
	skybox.init();

	glDisable(GL_CLIP_PLANE0);
	glPopMatrix();

	//render color buffer to textureGLuint texturereflectionID;
	GLuint refractionID;
	glGenTextures(2, &refractionID);
	glBindTexture(GL_TEXTURE_2D, refractionID);
	glCopyTexSubImage2D(GL_TEXTURE_2D,0,0,0,0,0, windowWidth, windowHeight);

	//render depth to texture
	GLuint depthID;
	glGenTextures(3, &depthID);
	glBindTexture(GL_TEXTURE_2D, depthID);
	glCopyTexImage2D(GL_TEXTURE_2D,0,GL_DEPTH_COMPONENT, 0,0,  windowWidth, windowHeight, 0);
}

//Render Scene scaled by(1.0, -1.0, 1.0) to texture

void RenderReflection(GLuint heightMapTexID,unsigned int windowWidth, unsigned int windowHeight)
{
	glViewport(0,0, windowWidth, windowHeight);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();

	glPushMatrix();
	glTranslatef(0.0f, 0.0f, 0.0f);
	glScalef(1.0, -1.0, 1.0);
	double plane[4] = {0.0, 1.0, 0.0, 0.0}; //water at y=0
	glEnable(GL_CLIP_PLANE0);
	glClipPlane(GL_CLIP_PLANE0, plane);

	//regenerate everything flipped arroung plane -- not working 
	shadowmap.init(heightMapTexID);
	terrain.init(heightMapTexID);
	skybox.init();

	glDisable(GL_CLIP_PLANE0);
	glPopMatrix();

	//render reflection to texture
	GLuint texturereflectionID;
	glGenTextures(1, &texturereflectionID);
	glBindTexture(GL_TEXTURE_2D, texturereflectionID);
	//glCopyTexSubImage2D copies the frame buffer
	//to the bound texture
	glCopyTexSubImage2D(GL_TEXTURE_2D,0,0,0,0,0,windowWidth, windowHeight);
}


void Watermap::init(GLuint heightMapTexID,unsigned int width, unsigned int height) {
	const float size = 1.0f;
	
	const GLfloat vertices[] = {
        -size,-size, 0,
		-size, size, 0,
		 size, size, 0,
		 size, size, 0,
		-size, size, 0,
		 size,-size, 0};

    // Common initialization.
    RenderingContext::init(watermap_vshader, watermap_fshader);

    /// Render to the screen : FBO 0;
    _frameBufferID = 0;

    const int waterTex = 9;
    GLuint uniformID = glGetUniformLocation(_programID, "waterTex");
    glUniform1i(uniformID, waterTex);

	// probably need to generate all texture fliped
	/// Load and bind the Skybox cube texture.
    
	RenderRefractionAndDepth(heightMapTexID,width,height);
	RenderReflection(heightMapTexID,width,height);

	/*
    glGenTextures(1, &_WatermapTexID);
    glBindTexture(GL_TEXTURE_2D, _WatermapTexID);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32, _width, _height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
	*/

   /// Copy the vertices in a vertex buffer.
    GLuint vertexBufferID;
    glGenBuffers(1, &vertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), &vertices, GL_STATIC_DRAW);

    /// Vertex attribute "position" points to the binded buffer.
    GLuint positionID = glGetAttribLocation(_programID, "position");
    glEnableVertexAttribArray(positionID);
    // vec3: 3 floats per vertex for the position attribute.
    glVertexAttribPointer(positionID, 3, GL_FLOAT, GL_FALSE, 0, 0);

    /// Set uniform IDs.
    _modelviewID = glGetUniformLocation(_programID, "modelview");
    _projectionID = glGetUniformLocation(_programID, "projection");
}


void Watermap::draw(mat4& projection, mat4& modelview) const {

    /// Common drawing.
    RenderingContext::draw();

    /// Update the content of the uniforms.
    glUniformMatrix4fv(_modelviewID, 1, GL_FALSE, modelview.data());
    glUniformMatrix4fv(_projectionID, 1, GL_FALSE, projection.data());

    /// Render the water sufrace.
    glDrawArrays(GL_TRIANGLES, 0, nVertices);

}


void Watermap::clean() {
    RenderingContext::clean();
}
