#version 330 core

// Coordinate of the fragment within a point.
in vec2 gl_PointCoord;

// First output buffer is pixel color. Fourth component is alpha.
layout(location = 0) out vec4 color;


void main() {

    // Distance from particle center in [0,0.5]
    float r = distance(gl_PointCoord, vec2(0.5,0.5));

    // Discard fragments that are outside of a circle : sprite corners.
    if(r > 0.5)
        discard;

    // Fragments that are inside get transparency gradient (alpha = 1 is fully opaque).
    color = vec4(1, 1, 1, 0.5-r);

}
