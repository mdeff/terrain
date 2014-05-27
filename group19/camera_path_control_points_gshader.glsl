#version 330 core

layout(points) in;
layout(triangle_strip, max_vertices=24) out;

// Transformation matrices from model space to camera clip space.
uniform mat4 projection;
uniform mat4 view;

// Light source position in world space.
uniform vec3 lightPositionWorld;

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


const float s = 0.02;
const vec4 cubeVertices[8] = vec4[] (
    vec4(-s,-s,-s,0.0),
    vec4(-s, s,-s,0.0),
    vec4( s,-s,-s,0.0),
    vec4( s, s,-s,0.0),
    vec4(-s,-s, s,0.0),
    vec4(-s, s, s,0.0),
    vec4( s,-s, s,0.0),
    vec4( s, s, s,0.0)
);

const ivec4 cubeIndices[6] = ivec4[] (
    ivec4(0,1,2,3),
    ivec4(7,6,3,2),
    ivec4(7,5,6,4),
    ivec4(4,0,6,2),
    ivec4(1,0,5,4),
    ivec4(3,1,7,5)
);

const vec3 cubeNormals[6] = vec3[] (
    vec3( 0.0,  0.0, -1.0),
    vec3( 1.0,  0.0,  0.0),
    vec3( 0.0,  0.0,  1.0),
    vec3( 0.0, -1.0,  0.0),
    vec3(-1.0,  0.0,  0.0),
    vec3( 0.0,  1.0,  0.0)
);

// Light color.
const vec3 light = vec3(1,1,1);

// Specular parameter.
const float power = 60.0f;


void main() {

    // Pass the color from vertex to fragment shader.
    color_f = color_g[0];

    // Light and view directions : subtraction of 2 points gives vector.
    // Camera space --> camera position at origin --> subtraction by [0,0,0].
    vec4 vertexPosition3DWorld = gl_in[0].gl_Position;
    vec3 lightDirWorld = normalize(lightPositionWorld - vec3(vertexPosition3DWorld));
    vec3 viewDirCamera = normalize(vec3(view * vertexPosition3DWorld));

    // Compute the 8 cube corner coordinates.
    vec4 vertexPositionClip[8];
    for(int k=0; k<8; ++k) {
        vertexPositionClip[k] = projection * view * (gl_in[0].gl_Position + cubeVertices[k]);
    }

    // Each cube face needs 4 corners --> 24 vertices.
    for(int iFace=0; iFace<6; ++iFace) {

        // Cube face normal.
        vec3 normalWorld = cubeNormals[iFace];

        // Lightning components : cube faces have uniform color.
        // They are too small for specular lightning to make sense in fragment shader.
        vec3 ambient = light * 0.4f * color_g[0];
        vec3 diffuse = light * 0.3f * color_g[0] * max(dot(normalWorld,lightDirWorld),0.0);
        vec3 specular = light * 0.3f * color_g[0] * pow(max(dot(viewDirCamera,reflect(lightDirWorld,normalWorld)),0.0),power);

        // Assemble the light.
        color_f = ambient + diffuse + specular;

        // Emit the face 4 vertices : 2 triangles with triangle_strip.
        for(int iCorner=0; iCorner<4; ++iCorner) {
            gl_Position = vertexPositionClip[cubeIndices[iFace][iCorner]];
            EmitVertex();
        }

        // End the triangle strip : cube faces are independant --> for shading.
        EndPrimitive();

    }
}
