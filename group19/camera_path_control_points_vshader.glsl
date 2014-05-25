#version 330 core

// Selected control point.
uniform int selectedControlPoint;

// Vertices 3D position in world space.
in vec3 vertexPosition3DWorld;

// Index of the current vertex defines the index in the position texture.
in int gl_VertexID;

// Vertex position (mandatory output).
out gl_PerVertex {
    vec4 gl_Position;
};

// Pixel color for fragment shader.
out vec3 color_g;


void main() {

    // Pass the vertex position to the geometry shader.
    gl_Position = vec4(vertexPosition3DWorld, 1.0);

    // Selected control point appears in a different color.
    if(gl_VertexID == selectedControlPoint)
        color_g = vec3(1,0,0);
    else
        color_g = vec3(0,1,0);

}
