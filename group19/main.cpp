// Copyright (C) 2014 - LGG EPFL

#include <iostream>
#include <sstream>
#include "common.h"
#include <OpenGP/Surface_mesh.h>

#include "LightDepth_vshader.h"
#include "LightDepth_fshader.h"

#include "ShadowMap_vshader.h"
#include "ShadowMap_fshader.h"

#include "cube.h"

using namespace std;
using namespace opengp;

///Model-view-projection
mat4 model;
mat4 view;
mat4 projection;

/// Mesh
unsigned int n_indices;
GLuint vertexbuffer;
GLuint colorbuffer;
GLuint uvbuffer;

///globals for shadow mapping
GLuint shadowMapProgramID;
GLuint lightDepthProgramID;
GLuint lightMatrixID;

GLuint texture;
GLuint textureID;

GLuint FramebufferName;
GLuint MVPID;
GLuint lightOffsetMVPID;
GLuint ShadowMapID;
GLuint depthTexture;
mat4 lightMVP;


GLuint loadTGA(const char * imagepath){
    // Create one OpenGL texture
    GLuint textureID;
    glGenTextures(ONE, &textureID);

    // "Bind" the newly created texture : all future texture functions will modify this texture
    glBindTexture(GL_TEXTURE_2D, textureID);

    // Read the file, call glTexImage2D with the right parameters
    glfwLoadTexture2D(imagepath, 0);

    // Nice trilinear filtering.
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D);

    // Return the ID of the texture we just created
    return textureID;
}

void update_matrix_stack(const mat4& _model){
    model = mat4::Identity();

    projection = Eigen::perspective(45.0f, 4.0f/3.0f, 0.1f, 10.0f);

    vec3 cam_pos(-2.0f, 2.0f, -2.0f);
    vec3 cam_look(0.0f, 0.0f, 0.0f);
    vec3 cam_up(0.0f, 1.0f, 0.0f);
    view = Eigen::lookAt(cam_pos, cam_look, cam_up);
}

void bindMesh( const char* objFileName ){

    Surface_mesh mesh;
    mesh.read(objFileName);
    mesh.triangulate();
    cout << "num vertices: " << mesh.n_vertices() << " num faces: " << mesh.n_faces() << endl;

    std::vector<vec3> vertices;
    std::vector<vec2> uvs;
    std::vector<vec3> colors;
    
    for(Surface_mesh::Face_iterator fit = mesh.faces_begin(); fit != mesh.faces_end(); ++fit) {
        unsigned int n = mesh.valence(*fit);
        Surface_mesh::Vertex_around_face_circulator vit = mesh.vertices(*fit);
        for(unsigned int v = 0; v < n; ++v) {
            
            vertices.push_back(mesh.position(*vit));
            uvs.push_back(vec2(0.,0.));
            colors.push_back(vec3(1.0,1.0,1.0));

            ++vit;
        }
    }

    n_indices = vertices.size();

    glGenBuffers(ONE, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(vec3), &vertices[0], GL_STATIC_DRAW);

    glGenBuffers(ONE, &colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(vec3), &colors[0], GL_STATIC_DRAW);

    glGenBuffers(ONE, &uvbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, uvbuffer);
    glBufferData(GL_ARRAY_BUFFER, uvs.size() * sizeof(vec2), &uvs[0], GL_STATIC_DRAW);
}

void bindCube() {
    n_indices = nCubeVertices;

    glGenBuffers(ONE, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, nCubeVertices * sizeof(vec3), &cubeVertices[0], GL_STATIC_DRAW);

    glGenBuffers(ONE, &colorbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glBufferData(GL_ARRAY_BUFFER, nCubeColors * sizeof(vec3), &cubeColors[0], GL_STATIC_DRAW);

    ///>>>>>>>>>> TODO >>>>>>>>>>>
    /// TODO: Practical 6.
    /// 1) Create uvbuffer and bind the cubeUVs to the uvbuffer.
    ///<<<<<<<<<< TODO <<<<<<<<<<<

    texture = loadTGA("rubik.tga");
}

void init() {

    GLuint VertexArrayID;
    glGenVertexArrays(ONE, &VertexArrayID);
    glBindVertexArray(VertexArrayID);
    
    //bindMesh("tangle_cube.obj");
    bindCube();

    lightDepthProgramID = compile_shaders( LightDepth_vshader, LightDepth_fshader );
    if(!lightDepthProgramID) exit(EXIT_FAILURE);
    
    lightMatrixID = glGetUniformLocation(lightDepthProgramID, "lightMVP");
    
    // The framebuffer, which regroups 0, 1, or more textures, and 0 or 1 depth buffer.
    FramebufferName = 0;
    glGenFramebuffers(1, &FramebufferName);
    glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
    
    // Depth texture. Slower than a depth buffer, but you can sample it later in your shader
    glGenTextures(ONE, &depthTexture);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0,GL_DEPTH_COMPONENT32, 1024, 1024, 0,GL_DEPTH_COMPONENT, GL_FLOAT, 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR); 
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_FUNC, GL_LEQUAL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_REF_TO_TEXTURE);
    
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthTexture, 0);
    
    // No color output in the bound framebuffer, only depth.
    glDrawBuffer(GL_NONE);
    
    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE){
        std::cerr << "Framebuffer not complete." <<std::endl;
    }
    
    shadowMapProgramID = compile_shaders( ShadowMap_vshader, ShadowMap_fshader );
    if(!shadowMapProgramID) exit(EXIT_FAILURE);
    
    MVPID = glGetUniformLocation(shadowMapProgramID, "MVP");
    lightOffsetMVPID = glGetUniformLocation(shadowMapProgramID, "lightOffsetMVP");
    ShadowMapID = glGetUniformLocation(shadowMapProgramID, "shadowMap");
    textureID = glGetUniformLocation(shadowMapProgramID, "tex");

    update_matrix_stack(mat4::Identity());

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_MULTISAMPLE);
    
}

void displayLightDepth( float t){
    
    glBindFramebuffer(GL_FRAMEBUFFER, FramebufferName);
    glViewport(0,0,1024,1024);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(lightDepthProgramID);
    
    // Compute and send the MVP matrix from the light's point of view for spot light :
    float fieldOfView = 45.0f;
    float aspectRatio = 1.f;
    float nearPlane = 0.1f;
    float farPlane  = 10.f;
    vec3 lightPosition(3.0*sin(t),3.0,3.0*cos(t));
    vec3 lightAt(0.0,0.0,0.0);
    vec3 lightUp(0.0,1.0,0.0);
    
    ///>>>>>>>>>> TODO >>>>>>>>>>>
    /// TODO: Practical 6.
    /// 2) Assembly the lightMVP matrix for a spotlight source.
    ///<<<<<<<<<< TODO <<<<<<<<<<<
    lightMVP = mat4::Identity();
    glUniformMatrix4fv(lightMatrixID, ONE, GL_FALSE,lightMVP.data());
    
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer( 0, 3,GL_FLOAT,DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);
    
    glDrawArrays(GL_TRIANGLES,0,n_indices);
    
    glDisableVertexAttribArray(0);
}

void displayWithShadows( float t ){
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0,0,1024,768);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shadowMapProgramID);
    
    mat4 MVP = projection * view * model;
    ///>>>>>>>>>> TODO >>>>>>>>>>>
    /// TODO: Practical 6.
    /// 2) Assemble the offsetMatrix that maps from light-coordinates in (-1,1)x(-1,1) to texture coordinates in (0,1)x(0,1)
    ///<<<<<<<<<< TODO <<<<<<<<<<<
    mat4 offsetMatrix = mat4::Identity();
    
    mat4 lightOffsetMVP = offsetMatrix*lightMVP;
    
    glUniformMatrix4fv(MVPID, 1, GL_FALSE, MVP.data());
    glUniformMatrix4fv(lightOffsetMVPID, 1, GL_FALSE, lightOffsetMVP.data());
    glUniform1f(glGetUniformLocation(shadowMapProgramID,"t"),t);
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, depthTexture);
    glUniform1i(ShadowMapID, 0);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture);
    glUniform1i(textureID, 1);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glVertexAttribPointer(0,3,GL_FLOAT,DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, colorbuffer);
    glVertexAttribPointer(1,3,GL_FLOAT,DONT_NORMALIZE, ZERO_STRIDE, ZERO_BUFFER_OFFSET);

    ///>>>>>>>>>> TODO >>>>>>>>>>>
    /// TODO: Practical 6.
    /// 1) Enable the uvbuffer as a third vertex attribute array. Don't forget to disable it after the call to drawArrays
    ///<<<<<<<<<< TODO <<<<<<<<<<<
    
    glDrawArrays(GL_TRIANGLES,0,n_indices);
    
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}

void display() {
    static float t=0.;
    displayLightDepth(t);
    displayWithShadows(t);
    t += 0.01;
}

int main(int, char**) {
    glfwInitWindowSize(1024, 768);
    glfwCreateWindow("Practical 6");
    glfwDisplayFunc(display);
    init();
    glfwTrackball(update_matrix_stack);
    glfwMainLoop();
    return EXIT_SUCCESS;    
}
