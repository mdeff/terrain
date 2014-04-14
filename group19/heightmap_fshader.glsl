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
vec2 fade_new(vec2 t) {
    return t * t * t * ( t * (t*6-15) + 10 );
}

// Original Perlin interpolation function.
// Less expensive to evaluate but results in discontinuous second derivatives.
vec2 fade_orig(vec2 t) {
    return t * t * (3 - 2 * t);
}

// Look-up in the permutation table at index idx [0,255].
int perm(int idx) {
    //return texture(permTableTex, x/256.0).r;
    int idx_mod = int(mod(idx, 255));
    return int(texelFetch(permTableTex, idx_mod, 0).r);
}

// Look-up in the gradient vectors table at index idx [0,3].
float grad(int idx, vec2 position) {
    //int idx_mod = int(mod(idx, 4)); // Without zeros in gradients.
    int idx_mod = int(mod(idx, 8)); // With    zeros in gradients.
    vec2 gradient = texelFetch(gradVectTex, idx_mod, 0).rg;
    return dot(gradient, position);
}

// Linear interpolation.
float lerp(in float a, in float b, in float t) {
    return a + t*(b-a);
}

// Perlin noise function.
float perlin_noise(vec2 position) {

    // Find which square contains the pixel.
    ivec2 square = ivec2(floor(position));

    // Find relative position (displacement) in this square [0,1].
    vec2 disp = position - vec2(square);

    // Observe the displacement [0,1].
    //return disp.x;
    //return disp.y;

    // Generate a pseudo-random value for current square using an hash function.
    int rnd = perm( perm(square.x) + square.y );

    // Verify that rnd is a random number in [0,255].
    //return rnd / 255.0f;

    // Noise at current position.
    float noise = grad(rnd, disp.xy);
    //return noise;

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

// Fractal Brownian motion : sum_k l^(-k*H) * f(l^k * x).
float fBm(vec2 position, float H, float lacunarity, int octaves) {

    float height = 0.0f;

    // Loop will be unrolled by the compiler (GPU driver).
    for (int k=0; k<octaves; k++) {
        height   += perlin_noise(position) * pow(lacunarity, -H*k);
        position *= lacunarity;
    }

    return height;

}

// Multifractal : fractal system which has a different fractal dimension in different regions.
float multifractal(vec2 position, float H, float lacunarity, float octaves, float offset) {

	
	float weight = (perlin_noise(position) + offset) * pow(lacunarity, 0)/3.0f;
	float signal = 0.0f;
    float height =  weight;

    // Loop will be unrolled by the compiler (GPU driver).
    for (int k=0; k<octaves; k++) {
		if ( weight > 1.0f ) weight = 1.0f;
		
		signal = (perlin_noise(position) + offset) * pow(lacunarity, -H*k);

		height += weight * signal;

		weight *= signal;

        position *= lacunarity;
    }
	return height;
}

void main() {

    // Perlin noise.
    //height = 0.2f * perlin_noise(2.0f * position2.xy);

    // Fractal Brownian motion.
    //height = fBm(position2.xy, 1.1f, 10.0f, 10) / 2.0f;

    // Multifractal.
	// Multifractal(vec2 position, float H, float lacunarity, float octaves, float offset) {
    height = (multifractal(position2.xy, 0.25f, 4.0f, 5, 0.75f) / 4.0f)-0.15f;

    // Ground floor (lake).
    if (height < 0.0f)
        height = 0.0f;

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
