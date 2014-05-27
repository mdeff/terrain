#version 330 core

// Pixel window-relative coordinates. (0,0) is lower-left corner.
layout(pixel_center_integer) in vec4 gl_FragCoord;

// Controller and camera views textures.
uniform sampler2DMS mainviewTex, previewTex;
//uniform sampler2D controllerViewTex, cameraViewTex;

// First output buffer is pixel color (mandatory output, gl_FragColor).
layout(location = 0) out vec3 color;


vec3 MSAA_resolve(sampler2DMS texture, ivec2 coord) {
    const int samples = 4;
    vec3 res;
    res = texelFetch(texture, coord, 0).rgb;
    for(int k=1; k<samples; ++k) {
        res += texelFetch(texture, coord, k).rgb;
    }
    return(res / float(samples));
}


void main() {

    const int previewShrink = 3;
    const ivec2 previewPos = ivec2(1024-341-10-10, 10+5);
    const int borderWidth = 5;
    const vec3 borderColor = vec3(0.0, 0.0, 0.0);

    ivec2 windowSize = textureSize(mainviewTex);
    ivec2 previewSize =  windowSize / previewShrink;

    bool xPreview = gl_FragCoord.x >= previewPos.x && gl_FragCoord.x < previewPos.x + previewSize.x;
    bool yPreview = gl_FragCoord.y >= previewPos.y && gl_FragCoord.y < previewPos.y + previewSize.y;

    bool xBorder = gl_FragCoord.x >= previewPos.x - borderWidth && gl_FragCoord.x < previewPos.x + previewSize.x + borderWidth;
    bool yBorder = gl_FragCoord.y >= previewPos.y - borderWidth && gl_FragCoord.y < previewPos.y + previewSize.y + borderWidth;

    if(xBorder && yBorder) {

        if(xPreview && yPreview) {
            color = MSAA_resolve(previewTex, ivec2(gl_FragCoord.xy-previewPos)*previewShrink);
        } else {
            color = borderColor;
        }

    } else {
        color = MSAA_resolve(mainviewTex, ivec2(gl_FragCoord.xy));
    }

}
