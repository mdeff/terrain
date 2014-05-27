
#include "heightmap.h"
#include "vertices.h"

#include <cstdlib>
#include <iostream>

#include <GL/glew.h>
#include <GL/glfw.h>
#include "opengp.h"

#include "heightmap_vshader.h"
#include "heightmap_fshader.h"


Heightmap::Heightmap(unsigned int width, unsigned int height) :
    RenderingContext(width, height) {
}


GLuint Heightmap::init(Vertices* vertices) {

    /// Common initialization.
    preinit(vertices, heightmap_vshader, heightmap_fshader, NULL, "vertexPosition2D", -1);

    /// Create and bind the permutation table to texture 0.
    GLuint permTableTexID = gen_permutation_table();
    set_texture(0, permTableTexID, "permTableTex", GL_TEXTURE_1D);

    /// Create and bind the gradient vectors to texture 1.
    GLuint gradVectTexID = gen_gradient_vectors();
    set_texture(1, gradVectTexID, "gradVectTex", GL_TEXTURE_1D);

    /// Create and bind the texture which will contain the
    /// color output (the actual height map) of our shader.
    GLuint heightMapTexID;
    glGenTextures(1, &heightMapTexID);
    glBindTexture(GL_TEXTURE_2D, heightMapTexID);

    /// Empty image (no data), one color component, unclamped 32 bits float.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, _width, _height, 0, GL_RED, GL_FLOAT, 0);

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
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, heightMapTexID, 0);
    GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0};
    glDrawBuffers(1, drawBuffers);

    /// Check that our framebuffer object (FBO) is complete.
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Heightmap framebuffer not complete." << std::endl;
        exit(EXIT_FAILURE);
    }

    /// Return the heightmap texture ID (for the terrain).
    return heightMapTexID;

}


void Heightmap::draw() const {

    /// Common drawing.
    predraw();

    /// Update the content of the uniforms.

    /// Clear the FBO.
    glClear(GL_COLOR_BUFFER_BIT);

    /// Render the height map to FBO.
    _vertices->draw();

}



GLuint Heightmap::test() const {

    /// Create and bind the texture.
    GLuint heightMapTexID;
    glGenTextures(1, &heightMapTexID);
    glBindTexture(GL_TEXTURE_2D, heightMapTexID);

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
    return heightMapTexID;

}


GLuint Heightmap::gen_permutation_table() const {

    /// Pseudo-randomly generate the permutation table.
    const int size(256);
    // GLubyte permutationTable[size];
    GLfloat permutationTable[size];
    for(int k=0; k<size; ++k)
        permutationTable[k] = k;

    /// Seed the pseudo-random generator for reproductability.
    std::srand(10);

    /// Fisher-Yates / Knuth shuffle.
    // GLubyte tmp;
    for(int k=size-1; k>0; --k) {
        // Random number with 0 <= rnd <= k.
        GLuint idx = int(float(k) * std::rand() / RAND_MAX);
        GLfloat tmp = permutationTable[k];
        permutationTable[k] = permutationTable[idx];
        permutationTable[idx] = tmp;
    }

    /// Print the permutation table.
    // for(int k=0; k<size; ++k)
    //     cout << permutationTable[k] << " ";

    /// Create the texture.
    GLuint permTableTexID;
    glGenTextures(1, &permTableTexID);
    glBindTexture(GL_TEXTURE_1D, permTableTexID);

    /// Copy the permutation table to GPU.
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


GLuint Heightmap::gen_gradient_vectors() const {

    /// Gradients for 2D noise.
    // const GLbyte gradients[nVectors*dim] = {
    const GLfloat gradients[] = {
        1.0f,  1.0f,
       -1.0f,  1.0f,
        1.0f, -1.0f,
       -1.0f, -1.0f,
        0.0f,  1.0f,
        0.0f, -1.0f,
        1.0f,  0.0f,
       -1.0f,  0.0f,
    };
    const int nGradients = sizeof(gradients) / sizeof(GLfloat) / 2;

    /// Create the texture.
    GLuint gradVectTexID;
    glGenTextures(1, &gradVectTexID);
    glBindTexture(GL_TEXTURE_1D, gradVectTexID);

    /// Copy the gradient vectors to GPU.
    // Filled image, two color components, unclamped 32 bits float.
    // GL_RG8I does not work on my machine.
    //glTexImage1D(GL_TEXTURE_1D, 0, GL_RG8I, nVectors, 0, GL_RG, GL_BYTE, gradients);
    glTexImage1D(GL_TEXTURE_1D, 0, GL_RG32F, nGradients, 0, GL_RG, GL_FLOAT, gradients);

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
