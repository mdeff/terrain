
#include "skybox.h"
#include "vertices.h"

#include <iostream>
#include <stdio.h>

#include <GL/glew.h>
#include "opengp.h"

#include "skybox_vshader.h"
#include "skybox_fshader.h"


Skybox::Skybox(unsigned int width, unsigned int height) :
    RenderingContext(width, height) {
}


void Skybox::init(Vertices* vertices) {

    /// Common initialization.
    RenderingContext::init(vertices, skybox_vshader, skybox_fshader, "vertexPosition3DWorld", 0);

    /// Bind the Skybox cube map to texture 0.
    set_texture(0, -1, "skyboxTex", GL_TEXTURE_CUBE_MAP);
    loadCubeTexture();

    /// Set uniform IDs.
    _viewID = glGetUniformLocation(_programID, "view");
    _projectionID = glGetUniformLocation(_programID, "projection");

}


void Skybox::draw(const mat4& projection, const mat4& view) const {

    /// Common drawing.
    RenderingContext::draw();

    /// Update the content of the uniforms.
    glProgramUniformMatrix4fv(_programID, _viewID, 1, GL_FALSE, view.data());
    glProgramUniformMatrix4fv(_programID, _projectionID, 1, GL_FALSE, projection.data());

    /// Do not clear the default framebuffer (screen) : done by Terrain.
    /// Otherwise already drawn pixels will be cleared.

    /// Render the skybox from camera point of view to default framebuffer.
    _vertices->draw();

}


void Skybox::loadCubeTexture() const {

    // hardcode the size of image for now
    int width = 1024, height = 1024, channel = 3;
    int imgSize = width*height*channel;

    /// Allocate data for each pixel buffer.
    /// Too much data to be allocated on the stack --> allocate on the heap.
    unsigned char* left   = new unsigned char[imgSize];
    unsigned char* right  = new unsigned char[imgSize];
    unsigned char* back   = new unsigned char[imgSize];
    unsigned char* front  = new unsigned char[imgSize];
    unsigned char* top    = new unsigned char[imgSize];
    unsigned char* bottom = new unsigned char[imgSize];

    /// Load each an image for each face of the cube.
    if (!loadBMP("../../skybox/left.bmp", left))
        exit(EXIT_FAILURE);
    if (!loadBMP("../../skybox/right.bmp", right))
        exit(EXIT_FAILURE);
    if (!loadBMP("../../skybox/back.bmp", back))
        exit(EXIT_FAILURE);
    if (!loadBMP("../../skybox/front.bmp", front))
        exit(EXIT_FAILURE);
    if (!loadBMP("../../skybox/top.bmp", top))
        exit(EXIT_FAILURE);
    if (!loadBMP("../../skybox/bottom.bmp", bottom))
        exit(EXIT_FAILURE);

    /// Set the filtering.
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    /// Enable textures.
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, left);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, right);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, back);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, front);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, top);
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, bottom);

    /// Deallocate heap data.
    delete left, right, back, front, top, bottom;

}


int Skybox::loadBMP(const char* imagepath, unsigned char* data) const {

    // Data read from the header of the BMP file.
    unsigned char header[54];
    unsigned int dataPos;
    unsigned int imageSize;
    unsigned int width, height;

    // Open the file
    FILE * file = fopen(imagepath, "rb");
    if(!file) {
        std::cerr << imagepath << " could not be opened. Are you in the right directory ? Don't forget to read the FAQ !" << std::endl;
        return 0;
    }

    // Read the header, i.e. the 54 first bytes.

    // If less than 54 bytes are read, problem.
    if(fread(header, 1, 54, file) != 54 ){
        std::cerr << "Not a correct BMP file" << std::endl;
        return 0;
    }
    // A BMP file always begins with "BM".
    if(header[0]!='B' || header[1]!='M') {
        std::cerr << "Not a correct BMP file" << std::endl;
        return 0;
    }

    // Make sure this is a 24bpp file.
    if(*(int*)&(header[0x1E]) != 0) {
        std::cerr << "Not a correct BMP file" << std::endl;
        return 0;
    }
    if(*(int*)&(header[0x1C]) != 24) {
        std::cerr << "Not a correct BMP file" << std::endl;
        return 0;
    }

    // Read the information about the image
    dataPos    = *(int*)&(header[0x0A]);
    imageSize  = *(int*)&(header[0x22]);
    width      = *(int*)&(header[0x12]);
    height     = *(int*)&(header[0x16]);

    // For debugging only.
    //std::cout << "Image size : width = " << width << ", height = " << height << std::endl;

    // Some BMP files are misformatted, guess missing information.
    if(imageSize == 0)
        imageSize = width*height*3; // 3 : one byte for each Red, Green and Blue component
    if(dataPos == 0)
        dataPos = 54; // The BMP header is done that way

    // Read the actual data from the file into the buffer.
    fread(data, 1, imageSize, file);

    // Need to swap the order of channel since the order here is BGR not RGB.
    for(int i=0; i<imageSize; i=i+3) {
        unsigned char tmp = data[i];
        data[i] = data[i+2];
        data[i+2] = tmp;
    }

    // Everything is in memory now, the file wan be closed.
    fclose(file);

    // Return 1 if the file is read successfully
    return 1;
}
