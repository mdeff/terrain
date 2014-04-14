#version 330 core

// First and second textures.
// Defined by glActiveTexture and passed by glUniform1i.
uniform sampler1D permTableTex;
uniform sampler1D gradVectTex;

in vec3 position2;

// First output buffer.
// Which is DrawBuffers[1] = GL_COLOR_ATTACHMENT0
// Which is attached to heightmapTexture.
layout(location = 0) out float height;


// Perlin's new interpolation function (Hermite polynomial of degree 5).
// Results in a C2 continuous noise function.
vec2 fade_new(in vec2 t) {
    return t * t * t * ( t * (t*6-15) + 10 );
}

// Original Perlin interpolation function.
// Less expensive to evaluate but results in discontinuous second derivatives.
vec2 fade_orig(in vec2 t) {
    return t * t * (3 - 2 * t);
}

// Look-up in the permutation table at index idx [0,255].
int perm(in int idx) {
    //return texture(permTableTex, x/256.0).r;
    int idx_mod255 = int(mod(idx, 255));
    return int(texelFetch(permTableTex, idx_mod255, 0).r);
}

// Look-up in the gradient vectors table at index idx [0,3].
float grad(in int idx, in vec2 position) {
    int idx_mod4 = int(mod(idx, 4));
    vec2 gradient = texelFetch(gradVectTex, idx_mod4, 0).rg;
    return dot(gradient, position);
}

// Linear interpolation.
float lerp(in float a, in float b, in float t) {
    return a + t*(b-a);
}

// Perlin noise function.
float perlin_noise(in vec2 position) {

    // Find which square contains the pixel.
    ivec2 square = ivec2(floor(position));

    // Find relative position (displacement) in this square [0,1].
    vec2 disp = position - vec2(square);

    // Observe the displacement [0,1].
    //height = disp.x;
    //height = disp.y;

    // Generate a pseudo-random value for current square using an hash function.
    int rnd = perm( perm(square.x) + square.y );

    // Verify that rnd is a random number in [0,255].
    //height = rnd / 255.0f;

    // Noise at current position.
    float noise = grad(rnd, disp.xy);
    //height = noise;

    // Noise at the 3 neightboring positions.
    rnd = perm( perm(square.x+1) + square.y+0 );
    float noise_x  = grad(rnd, disp.xy -vec2(1,0));
    rnd = perm( perm(square.x+0) + square.y+1 );
    float noise_y  = grad(rnd, disp.xy -vec2(0,1));
    rnd = perm( perm(square.x+1) + square.y+1 );
    float noise_xy = grad(rnd, disp.xy -vec2(1,1));

    // Fade curve.
    vec2 f = fade_new(disp.xy);

    // Average over the four neighbor pixels.
    float avg1 = lerp(noise,   noise_x,  f.x);
    float avg2 = lerp(noise_y, noise_xy, f.x);
    return lerp(avg1, avg2, f.y);

}

// Fractal Brownian motion.
float fBm(vec2 position) {

    float height = 0.0f;

    const int N = 5;
    for (int k=1; k<=N; k++) {
        height += 0.2f/k * perlin_noise(1.0f * k * position + 1.0f);
    }

    return height;

}

void main() {

    // Fractal Brownian motion.
    height = fBm(position2.xy);

}

void test() {

    // gl_FragCoord.x
    // position2.x
//    height = 0.5;


 //   height = -textureSize(permTableTex, 0).x;// / 2.0;

//    height = -float(texture(permTableTex, 0).r) / 128.0;
//    height = -texture(permTableTex, 0/255).r / 91.0f;
//    height = -texture(permTableTex, 1/255).r / 65.0f;

//    height = -textureOffset(permTableTex, 0, 0).r / 91.0f;
//    height = -textureOffset(permTableTex, 0, 1).r / 65.0f;
    //height = -textureOffset(permTableTex, 0, 10).r / 225.0f;

    height = -texelFetch(permTableTex, 0, 0).r / 91.0f;
    height = -texelFetch(permTableTex, 1, 0).r / 65.0f;
    height = -texelFetch(permTableTex, 10, 0).r / 225.0f;
    height = -texelFetch(permTableTex, 255, 0).r / 144.0f;
//    height = -texelFetch(permTableTex, 256, 0).r / 91.0f;

//    height = -texelFetch(gradVectTex, 0, 0).r;
//    height = -texelFetch(gradVectTex, 0, 0).g;
//    height = +texelFetch(gradVectTex, 1, 0).r;
//    height = -texelFetch(gradVectTex, 1, 0).g;

//    height = -texture(permTableTex, vec2(0,0)).r / 128.0f;
//    height = -texture2D(permTableTex, vec2(0,0)).r / 128.0f;
    //height = 0.5;

//    height = texelFetch(permTableTex, 0).r;
    // World (triangle grid) coordinates are [-1,1].
    // Texture (height map) coordinates are [0,1].
//    float s = (position2.x+1)/2;
//    height = texture(permTableTex, s).r / 255 + 0.5;

}
