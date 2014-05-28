#version 330 core

// Pixel window-relative coordinates. (0,0) is lower-left corner.
layout(pixel_center_integer) in vec4 gl_FragCoord;

// Controller and camera views textures.
uniform sampler2DMS controllerViewTex, cameraViewTex;
//uniform sampler2D controllerViewTex, cameraViewTex;

// First output buffer is pixel color (mandatory output, gl_FragColor).
layout(location = 0) out vec3 color;


void main() {

    // Too much distortion for main screen.
//    vec2 UV = vec2(gl_FragCoord.x/1024.0, gl_FragCoord.y/768.0);
//    color = texture(controllerViewTex, UV).rgb;

    color  = texelFetch(controllerViewTex, ivec2(gl_FragCoord.xy), 0).rgb;
    color += texelFetch(controllerViewTex, ivec2(gl_FragCoord.xy), 1).rgb;
    color += texelFetch(controllerViewTex, ivec2(gl_FragCoord.xy), 2).rgb;
    color += texelFetch(controllerViewTex, ivec2(gl_FragCoord.xy), 3).rgb;
    color /= 4.0;

}
