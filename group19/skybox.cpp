#include "skybox.h"

#include <GL/glew.h>
#include "opengp.h"

#include "skybox_vshader.h"
#include "skybox_fshader.h"

const unsigned int nVertices = 36;

void Skybox::init() {

    const float size = 5.0f;
    const GLfloat skybox_vertices[] ={
        -size,-size,-size, // triangle 1 : begin
        -size,-size, size,
        -size, size, size, // triangle 1 : end
        size, size,-size, // triangle 2 : begin
        -size,-size,-size,
        -size, size,-size, // triangle 2 : end
        size,-size, size,
        -size,-size,-size,
        size,-size,-size,
        size, size,-size,
        size,-size,-size,
        -size,-size,-size,
        -size,-size,-size,
        -size, size, size,
        -size, size,-size,
        size,-size, size,
        -size,-size, size,
        -size,-size,-size,
        -size, size, size,
        -size,-size, size,
        size,-size, size,
        size, size, size,
        size,-size,-size,
        size, size,-size,
        size,-size,-size,
        size, size, size,
        size,-size, size,
        size, size, size,
        size, size,-size,
        -size, size,-size,
        size, size, size,
        -size, size,-size,
        -size, size, size,
        size, size, size,
        -size, size, size,
        size,-size, size
    };


    // Common initialization.
    RenderingContext::init(skybox_vshader, skybox_fshader);


    /*char *CubeMapTextureFiles[6] = {"../../skybox/right.jpg", "../../skybox/left.jpg", "../../skybox/top.jpg",
                                                "../../skybox/bottom.jpg", "../../skybox/front.jpg", "../../skybox/back.jpg"};
*/


    /// Vertex Buffer
    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skybox_vertices), &skybox_vertices, GL_STATIC_DRAW);


    GLuint positionID = glGetAttribLocation(_programID, "position");
    glEnableVertexAttribArray(positionID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    // vec3: 3 floats per vertex for the position attribute.
    glVertexAttribPointer(positionID, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

    //update_matrix_stack(mat4::Identity());

    /// Set uniform IDs.
    _modelviewID = glGetUniformLocation(_programID, "modelview");
    _projectionID = glGetUniformLocation(_programID, "projection");

}


void Skybox::draw(mat4& projection, mat4& modelview) const {

    // Common drawing.
    RenderingContext::draw();

    //--- Update the content of the uniforms (texture IDs, matrices, ...)
    glUniformMatrix4fv(_modelviewID, 1, GL_FALSE, modelview.data());
    glUniformMatrix4fv(_projectionID, 1, GL_FALSE, projection.data());

    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    //glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
     glDrawArrays(GL_TRIANGLES, 0, nVertices);

    //make sure the VAO is not changed from the outside
    //glBindVertexArray(0);
}
