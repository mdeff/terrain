#version 330 core

// There is no output buffer because of glDrawBuffer(GL_NONE).
// shadowMapTexture is attached as a GL_DEPTH_ATTACHMENT.
// The hardware will thus copy the depth of each pixel (Z coordinate in clip
// space), which is the distance to the light (camera) to the texture.

void main();
