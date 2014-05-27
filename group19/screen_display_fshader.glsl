#version 330 core

// Pixel window-relative coordinates. (0.5,0.5) is lower-left corner.
in vec4 gl_FragCoord;

// Controller and camera views textures.
uniform sampler2D controllerViewTex, cameraViewTex;

// First output buffer is pixel color (mandatory output, gl_FragColor).
layout(location = 0) out vec3 color;


void main() {

    vec2 UV = vec2(gl_FragCoord.x/1024.0, gl_FragCoord.y/768.0);
    color = texture(controllerViewTex, UV).rgb;

}
