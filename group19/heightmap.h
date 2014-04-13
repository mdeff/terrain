
// FIXME : should be a separated compilation unit, not a header.
// Multiple definition of functions like compile_shader() prevent this.
// No header guards !!

#include "heightmap_vshader.h"
#include "heightmap_fshader.h"


/// Generate a simple height map texture for test purpose.
GLuint gen_test_heightmap() {

    /// Create and bind the texture.
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);

    /// Flat terrain.
    //float pixels[] = {1.0f};
    //glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, 1, 1, 0, GL_RED, GL_FLOAT, pixels);

    /// Sort of "pyramid".
    // (0,0) corresponds to the lower left corner.
    // (0, 0) (.5, 0) (1, 0)
    // (0,.5) (.5,.5) (1,.5)
    // (0, 1) (.5, 1) (1, 1)
    float pixels[] = {
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


/// Generate the heightmap texture.
GLuint gen_heightmap() {

    /// Height map texture size.
    const int texWidth(1024);
    const int texHeight(1024);

    /// Compile and install the heightmap shaders.
    GLuint programID = compile_shaders(heightmap_vshader, heightmap_fshader);
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

    /// Create the texture which will contain the color output
    /// (the actuall height map) of our shader.
    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    // Empty image (no data), one color component, 32 bits floating point format.
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, texWidth, texHeight, 0, GL_RED, GL_FLOAT, 0);
    // Simple filtering (needed).
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    // Filtering
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    // Nice trilinear filtering.
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
//    glGenerateMipmap(GL_TEXTURE_2D);

    /// Configure the framebuffer : heightmapTexture become the
    /// fragment shader first output buffer.
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, textureID, 0);
//    GLenum drawBuffers[] = {GL_COLOR_ATTACHMENT0};
//    glDrawBuffers(1, drawBuffers);

    /// Check that our framebuffer is complete.
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        std::cerr << "Heightmap framebuffer not complete." <<std::endl;
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
    glVertexAttribPointer(positionID, 3, GL_FLOAT, DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);

    /// Render the 2 triangles (6 vertices).
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices)/3);

    /// Clean up the now useless objects.
    glDisableVertexAttribArray(positionID);
    glDeleteBuffers(1, &vertexBufferID);
    glDeleteFramebuffers(1, &frameBufferID);
    glDeleteProgram(programID);

    /// Return the height map texture ID.
    return textureID;
}
