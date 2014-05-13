
#include "terrain.h"

#include <iostream>

#include <GL/glew.h>
#include <GL/glfw.h>
#include "opengp.h"

#include "terrain_vshader.h"
#include "terrain_fshader.h"

/// NxN triangle grid.
/// const is better than #define : type checked, optimized out anyway
const unsigned int N = 128;
const unsigned int nVertices = N*N;
unsigned int nIndices = (N-1)*(N-1)*6;


Terrain::Terrain(unsigned int width, unsigned int height) :
    RenderingContext(width, height) {
}


/// Generate the triangle grid vertices.
void Terrain::gen_triangle_grid() {

    /// Generate the vertices (line by line) : 16^2 = 256 vertices.
    vec2 vertices[nVertices];
    for(int y=0; y<N; y++) {
        for(int x=0; x<N; x++) {
            vertices[y*N+x] = vec2(float(2.0*x)/(N-1)-1, float(2.0*y)/(N-1)-1);
        }
    }

    /// Copy the vertices to GPU in a vertex buffer.
    glGenBuffers(1, &_vertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    /// Indices that form the triangles.
    /// Grid of 15x15 squares : 225 squares -> 450 triangles -> 1350 indices.
    unsigned int indices[nIndices];
    for(int y=0; y<N-1; y++) {
        for(int x=0; x<N-1; x++) {
            /// Upper left triangle of the square.
            indices[y*(N-1)*6+x*6+0] = y*N + x + 0;
            indices[y*(N-1)*6+x*6+1] = y*N + x + 1;
            indices[y*(N-1)*6+x*6+2] = y*N + x + N;
            /// Lower right triangle of the square.
            indices[y*(N-1)*6+x*6+3] = y*N + x + 1;
            indices[y*(N-1)*6+x*6+4] = y*N + x + N+1;
            indices[y*(N-1)*6+x*6+5] = y*N + x + N;
        }
    }

    /// Copy the indices to GPU in an index buffer.
    glGenBuffers(1, &_elementBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _elementBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

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


void Terrain::init(GLuint heightMapTexID) {

    /// Common initialization : vertex array and shader programs.
    RenderingContext::init(terrain_vshader, terrain_fshader);

    /// Bind the heightmap to texture 0.
    set_texture(0, heightMapTexID, "heightMapTex");

    /// Load material textures and bind them to textures 1 - 6.
    set_texture(1, -1, "sandTex");
    load_texture("../../textures/sand.tga");
    set_texture(2, -1, "iceMoutainTex");
    load_texture("../../textures/dordona_range.tga");
    set_texture(3, -1, "treeTex");
    load_texture("../../textures/forest.tga");
    set_texture(4, -1, "stoneTex");
    load_texture("../../textures/stone_2.tga");
    set_texture(5, -1, "waterTex");
    load_texture("../../textures/water.tga");
    set_texture(6, -1, "snowTex");
    load_texture("../../textures/snow.tga");

    /// Load the normal map for water lighting to texture 7.
    set_texture(7, -1, "waterNormalMap");
    load_texture("../../textures/water_normal_map_2.tga");

    /// Bind the shadowmap to texture 8.
    //set_texture(8, shadowMapTexID, "shadowMapTex");

    /// Generate a flat and regular triangle grid. Copy vertices to GPU.
    gen_triangle_grid();

    /// Vertex attribute "position" points to data from the currently binded array buffer.
//    glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferID);
//    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _elementBufferID);
    //GLuint positionID = glGetAttribLocation(_programID, "position");
    //glEnableVertexAttribArray(positionID);
    // vec2: 2 floats per vertex for the xy plane position attribute.
    //glVertexAttribPointer(positionID, 2, GL_FLOAT, GL_FALSE, 0, 0);

    /// Define light properties and pass them to the shaders.
    vec3 Ia(1.0f, 1.0f, 1.0f);
    vec3 Id(1.0f, 1.0f, 1.0f);
    vec3 Is(1.0f, 1.0f, 1.0f);
    GLuint _IaID = glGetUniformLocation(_programID, "Ia");
    GLuint _IdID = glGetUniformLocation(_programID, "Id");
    GLuint _IsID = glGetUniformLocation(_programID, "Is");
    glUniform3fv(_IaID, 1, Ia.data());
    glUniform3fv(_IdID, 1, Id.data());
    glUniform3fv(_IsID, 1, Is.data());

    /// Set uniform IDs.
    _modelviewID = glGetUniformLocation(_programID, "modelview");
    _projectionID = glGetUniformLocation(_programID, "projection");
    _lightOffsetMVPID = glGetUniformLocation(_programID, "lightOffsetMVP");
    _lightPositionModelID = glGetUniformLocation(_programID, "lightPositionModel");
    _timeID = glGetUniformLocation(_programID, "time");

    _vertexAttribID = glGetAttribLocation(_programID, "vertexPosition2DModel");
}


void Terrain::draw(mat4& projection, mat4& modelview, mat4& lightMVP, vec3& lightPositionModel) const {

    /// Common drawing.
    RenderingContext::draw();

    /// Vertex attribute "position" points to data from the currently binded array buffer.
    glBindBuffer(GL_ARRAY_BUFFER, _vertexBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _elementBufferID);
    glEnableVertexAttribArray(_vertexAttribID);
    // vec2: 2 floats per vertex for the vertexPosition2DModel attribute.
    glVertexAttribPointer(_vertexAttribID, 2, GL_FLOAT, GL_FALSE, 0, 0);

    /// Update the content of the uniforms.
    glUniformMatrix4fv(_modelviewID, 1, GL_FALSE, modelview.data());
    glUniformMatrix4fv(_projectionID, 1, GL_FALSE, projection.data());
    glUniform3fv(_lightPositionModelID, 1, lightPositionModel.data());

    /// Time value which animates water.
    // TODO: implement rollover ?
    static float time = 0;
    glUniform1f(_timeID, time++);

    /// Map from light-coordinates in (-1,1)x(-1,1) to texture
    /// coordinates in (0,1)x(0,1).
    mat4 biasMatrix;
    biasMatrix <<
            0.5f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.5f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.5f, 0.0f,
            0.5f, 0.5f, 0.5f, 1.0f;
    mat4 lightOffsetMVP = biasMatrix * lightMVP;
    glUniformMatrix4fv(_lightOffsetMVPID, 1, GL_FALSE, lightOffsetMVP.data());

    /// Clear the screen framebuffer.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /// Render the terrain from camera point of view to default framebuffer.
    glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, 0);


    glDisableVertexAttribArray(_vertexAttribID);

}


void Terrain::clean() {
    RenderingContext::clean();
}
