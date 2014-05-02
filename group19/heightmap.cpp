
// FIXME : should be a separated compilation unit, not a header.
// Multiple definition of functions like compile_shader() prevent this.
// No header guards !!

//#include "common.h"

#include <cstdlib>
#include <iostream>

#include <GL/glew.h>
#include "opengp.h"

#include "heightmap_vshader.h"
#include "heightmap_fshader.h"


/// Generate a simple height map texture for test purpose.
GLuint gen_test_heightmap() {

    /// Create and bind the texture.
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    /// Flat terrain.
    //GLfloat pixels[] = {1.0f};
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, 1, 1, 0, GL_RED, GL_FLOAT, pixels);

    /// Sort of "pyramid".
    // (0,0) corresponds to the lower left corner.
    // (0, 0) (.5, 0) (1, 0)
    // (0,.5) (.5,.5) (1,.5)
    // (0, 1) (.5, 1) (1, 1)
    GLfloat pixels[] = {
        0.0f, 0.5f, 0.0f,
        0.5f, 1.0f, 0.5f,
        0.0f, 0.5f, 0.0f,
    };
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, 3, 3, 0, GL_RED, GL_FLOAT, pixels);

    /// Simple filtering (needed).
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    /// Return the height map texture ID.
    return textureID;
}


GLuint gen_permutation_table(GLuint programID) {

    /// Pseudo-randomly generate the permutation table.
    const int size(256);
//    GLubyte permutationTable[size];
    GLfloat permutationTable[size];
    for(int k=0; k<size; ++k)
        permutationTable[k] = k;

    // Seed the pseudo-random generator for reproductability.
    std::srand(10);

    // Fisher-Yates / Knuth shuffle.
//    GLubyte tmp;
    GLfloat tmp;
    for(int k=size-1; k>0; --k) {
        // Random number with 0 <= rnd <= k.
        GLuint idx = int(float(k) * std::rand() / RAND_MAX);
        tmp = permutationTable[k];
        permutationTable[k] = permutationTable[idx];
        permutationTable[idx] = tmp;
    }

    // Print the permutation table.
    //for(int k=0; k<size; ++k)
    //    cout << permutationTable[k] << " ";

    /// Bind the permutation table to texture 0.
    const GLuint permTableTex = 0;
    glActiveTexture(GL_TEXTURE0+permTableTex);
    GLuint uniformID = glGetUniformLocation(programID, "permTableTex");
    glUniform1i(uniformID, permTableTex);
    GLuint permTableTexID;
    glGenTextures(1, &permTableTexID);
    glBindTexture(GL_TEXTURE_1D, permTableTexID);
    //glBindSampler(0, linearFiltering);
    // Filled image, one color component, unclamped 32 bits float.
    // GL_R8UI or GL_R32I does not work on my machine.
    //glTexImage1D(GL_TEXTURE_1D, 0, GL_R8UI, size, 0, GL_RED, GL_UNSIGNED_BYTE, permutationTable);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_R32F, size, 0, GL_RED, GL_FLOAT, permutationTable);

    /// Set the texture addressing to wrap (or repeat) mode, so we don't have to
    /// worry about extending the table to avoid indexing past the end of the array.
    /// Only in s direction, 1D texture.
    /// No wraping for integer indexing (works however on normalized coordinates).
    //glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);

    /// We do not want any interpolation of our random integers.
    //glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    return permTableTexID;

}


GLuint gen_gradient_vectors(GLuint programID) {

    /// Gradients for 2D noise.
    //static GLbyte gradients[nVectors*dim] = {
    static GLfloat gradients[] = {
        1.0f,  1.0f,
       -1.0f,  1.0f,
        1.0f, -1.0f,
       -1.0f, -1.0f,
        0.0f,  1.0f,
        0.0f, -1.0f,
        1.0f,  0.0f,
       -1.0f,  0.0f,
    };

    /// Bind the gradient vectors to texture 1.
    const GLuint gradVectTex = 1;
    glActiveTexture(GL_TEXTURE0+gradVectTex);
    GLuint uniformID = glGetUniformLocation(programID, "gradVectTex");
    glUniform1i(uniformID, gradVectTex);
    GLuint gradVectTexID;
    glGenTextures(1, &gradVectTexID);
    glBindTexture(GL_TEXTURE_1D, gradVectTexID);
    // Filled image, two color components, unclamped 32 bits float.
    // GL_RG8I does not work on my machine.
    //glTexImage1D(GL_TEXTURE_1D, 0, GL_RG8I, nVectors, 0, GL_RG, GL_BYTE, gradients);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RG32F, 8, 0, GL_RG, GL_FLOAT, gradients);

    /// Set the texture addressing to wrap (or repeat) mode, so we don't have to
    /// worry about extending the table to avoid indexing past the end of the array.
    /// Only in s direction, 1D texture.
    /// No wraping for integer indexing (works however on normalized coordinates).
    //glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_WRAP_S, GL_REPEAT);

    /// We do not want any interpolation of our vectors.
    //glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_1D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    return gradVectTexID;
}


/// Generate the heightmap texture.
GLuint gen_heightmap() {

    /// Height map texture size.
    const int texWidth(1024);
    const int texHeight(1024);

    /// Compile and install the heightmap shaders.
    GLuint programID = opengp::compile_shaders(heightmap_vshader, heightmap_fshader);
    if(!programID)
        exit(EXIT_FAILURE);
    glUseProgram(programID);

    /// Create a framebuffer (container for textures, and an optional depth buffer).
    /// The height map will be rendered to this FBO instead of the screen.
    /// Specify the transformation from normalized device coordinates to texture coordinates.
    GLuint frameBufferID;
    glGenFramebuffers(1, &frameBufferID);
    glBindFramebuffer(GL_FRAMEBUFFER, frameBufferID);
    glViewport(0, 0, texWidth, texHeight);

    /// Create the two input textures.
    GLuint permTableTexID = gen_permutation_table(programID);
    GLuint gradVectTexID = gen_gradient_vectors(programID);

    /// Create the texture which will contain the color output
    /// (the actuall height map) of our shader.
    GLuint heightMapTexID;
    glGenTextures(1, &heightMapTexID);
    glBindTexture(GL_TEXTURE_2D, heightMapTexID);
    // Empty image (no data), one color component, unclamped 32 bits float.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, texWidth, texHeight, 0, GL_RED, GL_FLOAT, 0);
    // Simple filtering (needed).
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    // Filtering
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    //// Nice trilinear filtering.
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    //glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//    glGenerateMipmap(GL_TEXTURE_2D);

    /// Configure the framebuffer : heightmapTexture become the
    /// fragment shader first output buffer.
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, heightMapTexID, 0);
//    GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0};
//    glDrawBuffers(1, drawBuffers);

    /// Check that our framebuffer is complete.
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Heightmap framebuffer not complete." << std::endl;
        exit(EXIT_FAILURE);
    }

    /// Fullscreen quad : fragment shader is executed on evey pixel of the texture.
    const GLfloat vertices[] = {
        -1.0f, -1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f, 0.0f,
         1.0f, -1.0f, 0.0f,
         1.0f,  1.0f, 0.0f,
    };
    GLuint vertexBufferID;
    glGenBuffers(1, &vertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    /// Vertex attribute "position" points to the binded buffer.
    GLuint positionID = glGetAttribLocation(programID, "position");
    glEnableVertexAttribArray(positionID);
    // vec3: 3 floats per vertex for the position attribute.
    glVertexAttribPointer(positionID, 3, GL_FLOAT, GL_FALSE, 0, 0);

    /// Render the 2 triangles (6 vertices).
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices)/3);

    /// Clean up the now useless objects.
    glDisableVertexAttribArray(positionID);
    glDeleteBuffers(1, &vertexBufferID);
    glDeleteTextures(1, &gradVectTexID);
    glDeleteTextures(1, &permTableTexID);
    glDeleteFramebuffers(1, &frameBufferID);
    glDeleteProgram(programID);

    /// Return the height map texture ID.
    return heightMapTexID;
}
