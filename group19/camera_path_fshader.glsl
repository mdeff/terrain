#version 330 core

// First output buffer is pixel color (mandatory output, gl_FragColor).
layout(location = 0) out vec3 color;


void main() {

    // Camera path Bezier curve drawn in red.
    color = vec3(1.0, 0.0, 0.0);

}
