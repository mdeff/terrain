#version 330 core

layout(points) in;
//layout(points, max_vertices=1) out;
layout(triangle_strip, max_vertices=24) out;

// Transformation matrices from model space to camera clip space.
uniform mat4 projection;
uniform mat4 view;

// Vertex position.
in gl_PerVertex {
  vec4 gl_Position;
} gl_in[];

// Vertex position (mandatory output).
out gl_PerVertex {
  vec4 gl_Position;
};

// Pixel color for fragment shader.
in vec3 color_g[];
out vec3 color_f;


const float s = 0.01;
const vec4 cubeVertices[8] =
    vec4[](
    vec4(-s,-s,-s,0.0),
    vec4(-s, s,-s,0.0),
    vec4( s,-s,-s,0.0),
    vec4( s, s,-s,0.0),
    vec4(-s,-s, s,0.0),
    vec4(-s, s, s,0.0),
    vec4( s,-s, s,0.0),
    vec4( s, s, s,0.0)
    );

const ivec4 cubeIndices[6]  =
    ivec4[] (
    ivec4(0,1,2,3),
    ivec4(7,6,3,2),
    ivec4(7,5,6,4),
    ivec4(4,0,6,2),
    ivec4(1,0,5,4),
    ivec4(3,1,7,5)
    );


void main() {

    // View matrix transforms from world space to camera space.
    // Projection matrix transforms from camera space to clip space (homogeneous space).
//    vec4 vertexPositionCamera = view * gl_in[0].gl_Position;
//    gl_Position = projection * vertexPositionCamera;

//    gl_PointSize = 5.0;
    color_f = color_g[0];

//    EmitVertex();
//    EndPrimitive();


    // Compute cube corner coordinates (screen space).
    vec4 coord[8];
    for(int i=0; i<8; ++i)
        coord[i] = projection * view * (gl_in[0].gl_Position + cubeVertices[i]);

    // Each cube face needs 4 corners --> 24 vertices.
    for(int iFace=0; iFace<6; ++iFace) {
        for(int iCorner=0; iCorner<4; ++iCorner) {
            gl_Position = coord[cubeIndices[iFace][iCorner]];
            EmitVertex();
        }
    }

    EndPrimitive();

}
