
#include "terrain.h"

#include <iostream>

#include <GL/glew.h>
#include <GL/glfw.h>
#include "opengp.h"

#include "terrain_vshader.h"
#include "terrain_fshader.h"

/// NxN triangle grid.
/// const is better than #define : type checked, optimized out anyway
const int N = 128;
const int nVertices = N*N;
const int nIndices = (N-1)*(N-1)*6;


Terrain::Terrain(unsigned int width, unsigned int height) :
    RenderingContext(width, height) {
}


/// Generate the triangle grid vertices.
void Terrain::gen_triangle_grid() {

    /// Generate the vertices (line by line) : 16^2 = 256 vertices.
    vec3 vertices[nVertices];
    for(int y=0; y<N; y++) {
        for(int x=0; x<N; x++) {
            vertices[y*N+x] = vec3(float(2.0*x)/(N-1)-1, float(2.0*y)/(N-1)-1, 0);
        }
    }

    /// Copy the vertices in a vertex buffer.
    GLuint vertexBufferID;
    glGenBuffers(1, &vertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
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

    /// Copy the indices in an index buffer.
    GLuint elementBufferID;
    glGenBuffers(1, &elementBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    /// Vertex attribute "position" points to the binded buffer.
    GLuint positionID = glGetAttribLocation(_programID, "position");
    glEnableVertexAttribArray(positionID);
    // vec3: 3 floats per vertex for the position attribute.
    glVertexAttribPointer(positionID, 3, GL_FLOAT, GL_FALSE, 0, 0);

}


GLuint Terrain::loadTexture(const char * imagepath) {

    // Start at 1.
    static int slotNum = 0;
    slotNum++;

    // Create one OpenGL texture
    GLuint textureID;
    glGenTextures(1, &textureID);

    glActiveTexture(GL_TEXTURE0 + slotNum);
    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Read the file, call glTexImage2D with the right parameters
    if (glfwLoadTexture2D(imagepath, 0)){
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

    // Return the ID of the texture we just created
    return slotNum;
}


void Terrain::init(GLuint heightMapTexID) {

    // Common initialization.
    RenderingContext::init(terrain_vshader, terrain_fshader);

    /// Render to the screen : FBO 0;
    _frameBufferID = 0;

    /// Bind the heightmap to texture 0.
    const GLuint heightMapTex = 0;
    glActiveTexture(GL_TEXTURE0+heightMapTex);
    glBindTexture(GL_TEXTURE_2D, heightMapTexID);
    GLuint uniformID = glGetUniformLocation(_programID, "heightMapTex");
    glUniform1i(uniformID, heightMapTex);

    // Load textures and bind them to textures 1 - 6.
    int slotNum = loadTexture("../../textures/sand.tga");
    uniformID = glGetUniformLocation(_programID, "sandTex");
    glUniform1i(uniformID, slotNum);
    slotNum = loadTexture("../../textures/dordona_range.tga");
    uniformID = glGetUniformLocation(_programID, "iceMoutainTex");
    glUniform1i(uniformID, slotNum);
    slotNum = loadTexture("../../textures/forest.tga");
    uniformID = glGetUniformLocation(_programID, "treeTex");
    glUniform1i(uniformID, slotNum);
    slotNum = loadTexture("../../textures/stone_2.tga");
    uniformID = glGetUniformLocation(_programID, "stoneTex");
    glUniform1i(uniformID, slotNum);
    slotNum = loadTexture("../../textures/water.tga");
    uniformID = glGetUniformLocation(_programID, "waterTex");
    glUniform1i(uniformID, slotNum);
    slotNum = loadTexture("../../textures/snow.tga");
    uniformID = glGetUniformLocation(_programID, "snowTex");
    glUniform1i(uniformID, slotNum);

    /// Generate a flat and regular triangle grid.
    gen_triangle_grid();

    /// Define light properties and pass them to the shaders.
    vec3 light_dir_tmp(1.0f,0.5f,1.0f);
    vec3 Ia(1.0f, 1.0f, 1.0f);
    vec3 Id(1.0f, 1.0f, 1.0f);
    vec3 Is(1.0f, 1.0f, 1.0f);
    GLuint light_pos_id = glGetUniformLocation(_programID, "light_dir_tmp"); //Given in camera space
    GLuint Ia_id = glGetUniformLocation(_programID, "Ia");
    GLuint Id_id = glGetUniformLocation(_programID, "Id");
    GLuint Is_id = glGetUniformLocation(_programID, "Is");
    glUniform3fv(light_pos_id, 1, light_dir_tmp.data());
    glUniform3fv(Ia_id, 1, Ia.data());
    glUniform3fv(Id_id, 1, Id.data());
    glUniform3fv(Is_id, 1, Is.data());

    /// Define the material properties and pass them to the shaders
    vec3 ka(0.65f, 0.7f, 0.65f);
    vec3 kd(0.35f, 0.25f, 0.35f);
    vec3 ks(0.35f, 0.25f, 0.35f);
    float p = 60.0f;
    GLuint ka_id = glGetUniformLocation(_programID, "ka");
    GLuint kd_id = glGetUniformLocation(_programID, "kd");
    GLuint ks_id = glGetUniformLocation(_programID, "ks");
    GLuint p_id = glGetUniformLocation(_programID, "p");
    glUniform3fv(ka_id, 1, ka.data());
    glUniform3fv(kd_id, 1, kd.data());
    glUniform3fv(ks_id, 1, ks.data());
    glUniform1f(p_id, p);

    GLuint N_id = glGetUniformLocation(_programID, "N");
    glUniform1d(N_id,N);



    /// Set uniform IDs.
    _modelviewID = glGetUniformLocation(_programID, "modelview");
    _projectionID = glGetUniformLocation(_programID, "projection");

    _timeID = glGetUniformLocation(_programID, "time");
}


void Terrain::draw(mat4& projection, mat4& modelview) const {

    /// Common drawing.
    RenderingContext::draw();

    /// Update the content of the uniforms.
    glUniformMatrix4fv(_modelviewID, 1, GL_FALSE, modelview.data());
    glUniformMatrix4fv(_projectionID, 1, GL_FALSE, projection.data());

    /// Time value which animates water.
    static float time = 0;
    glUniform1f(_timeID, time++);

    /// Clear the screen framebuffer.
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /// Render the terrain from the camera point of view.
    glDrawElements(GL_TRIANGLES, nIndices, GL_UNSIGNED_INT, 0);

}


void Terrain::clean() {
    RenderingContext::clean();
}
