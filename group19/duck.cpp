#include "duck.h"
#include "vertices.h"

#include <GL/glew.h>
#include <GL/glfw.h>
#include "opengp.h"

#include <Eigen/Geometry>

#include "duck_vshader.h"
#include "duck_fshader.h"


RenderedDuck::RenderedDuck(unsigned int width, unsigned int height):
	RenderingContext(width, height) {
}


void RenderedDuck::init(Vertices* vertices) {

	/// Common initialization.
    preinit(vertices, duck_vshader, duck_fshader, NULL, "vertexPosition3DWorld", "normal_mv");

    /// Light properties.
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
	_transID = glGetUniformLocation(_programID, "translation");
	_lightPositionWorldID = glGetUniformLocation(_programID, "lightPositionWorld");

	_aniMat = glGetUniformLocation(_programID, "animation");
}




void RenderedDuck::draw(const mat4& projection, const mat4 views[], const vec3& lightPositionWorld) {

	 /// Common drawing.
    predraw();

    /// Update the content of the uniforms.
    glUniformMatrix4fv( _projectionID, 1, GL_FALSE, projection.data());
	glUniform3fv(_lightPositionWorldID, 1, lightPositionWorld.data());



    /// Do not clear the framebuffers : done by Terrain.
    /// Otherwise already drawn pixels will be cleared.

    /// Flip the terrain by multiplying the Z coordinate by -1 in world space.
    //mat4 flip = mat4::Identity();
    //flip(2,2) = -1.0f;
    //mat4 viewFlip = view * flip;

    /// Render the terrain from camera point of view to the reflection FBO.
 /*   glUniformMatrix4fv(_viewID, 1, GL_FALSE, viewFlip.data());
    _vertices->draw();*/
  

	/* Translation matrix */
	mat4 trans_mat;
	trans_mat << 1,0,0, -0.55,
				 0,1,0, 0.15,
				 0,0,1, 0,
				 0,0,0,1;
	glUniformMatrix4fv(_transID, 1, GL_FALSE, trans_mat.data());
		

	/* For Animation: Move circular around a fixed point */
	//float radius = 
	static float angle = 0;
	mat4 ani_mat;
	


    /// Render from camera point of view to 'normal' FBOs.
    glUniformMatrix4fv(_viewID, 1, GL_FALSE, views[0].data());
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferIDs["controllerView"]);
    _vertices->draw();
    glUniformMatrix4fv(_viewID, 1, GL_FALSE, views[1].data());
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferIDs["cameraView"]);
    _vertices->draw();


    /// Flip the world by multiplying the Z coordinate by -1 in world space.
    mat4 flip = mat4::Identity();
    flip(2,2) = -1.0f;
    mat4 viewFlip[] = {views[0]*flip, views[1]*flip};

    /// Render from flipped camera point of view to 'reflection' FBOs.
    glUniformMatrix4fv(_viewID, 1, GL_FALSE, viewFlip[0].data());
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferIDs["controllerViewReflected"]);
    _vertices->draw();
    glUniformMatrix4fv(_viewID, 1, GL_FALSE, viewFlip[1].data());
    glBindFramebuffer(GL_FRAMEBUFFER, framebufferIDs["cameraViewReflected"]);
    _vertices->draw();


}
