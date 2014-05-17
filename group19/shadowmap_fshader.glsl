#version 330 core

// There is no output buffer because of glDrawBuffer(GL_NONE).
// shadowMapTexture is attached as a GL_DEPTH_ATTACHMENT.
// The hardware will thus copy the depth of each pixel (Z coordinate in clip
// space, which is the distance to the light / camera) to the texture.
void main();
void main(){}

// To test the output on default framebuffer (screen).
// r=2 --> * 10.0f - 8.8f
// r=3 --> * 35.0f - 33.2f
// layout(location = 0) out vec3 color;
// void main() {color = vec3(gl_FragCoord.z * 35.0f - 33.2f);}
