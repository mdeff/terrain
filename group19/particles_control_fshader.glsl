#version 330 core

// Time in seconds between two executions (frames).

uniform float deltaT;

// Particules last position and velocity textures.
uniform sampler1D particlePosTex;
uniform sampler1D particleVelTex;

// Pixel window-relative coordinates. (0,0) is lower-left corner.
// The window is 1 pixel height. Positions are stored along x.
layout(pixel_center_integer) in vec4 gl_FragCoord;

// Output buffer location = index in the "drawBuffers" array.
layout(location = 0) out vec3 particlePos;
layout(location = 1) out vec3 particleVel;


// Earth acceleration.
const vec3 acc = vec3(0, 0, -0.03);


// Pseudo-random generator, using high precision.
highp float rand(vec2 co) {
    highp float a  = 12.9898;
    highp float b  = 78.233;
    highp float c  = 43758.5453;
    highp float dt = dot(co.xy, vec2(a,b));
    highp float sn = mod(dt, 3.14);
    return fract(sin(sn) * c);
}


void main() {

    // Retrieve particle position and velocity.
    vec3 lastVel = texelFetch(particleVelTex, int(gl_FragCoord.x), 0).rgb;
    vec3 lastPos = texelFetch(particlePosTex, int(gl_FragCoord.x), 0).rgb;

    // Random acceleration vector.
    float rx = (0.5-rand(lastPos.xy)) / 100.0;
    float ry = (0.5-rand(lastPos.xz)) / 100.0;
    float rz = (0.5-rand(lastPos.yz)) /  50.0;
    vec3 r = vec3(rx, ry, rz);

    // Compute new position and velocity.
    vec3 vel = lastVel + deltaT * (acc + r);
    vec3 pos = lastPos + deltaT * lastVel;

    // Initialize particles that start above skybox.
    // Goal : particles ar initialized the same way than reseted.
    if(pos.z > 5.0 && pos.z < 6.0) {
        pos.z = 2.0;
        vel = vec3(0,0,0);
    }

    // Reset particles that fall under see level.
    if(pos.z < 0.0) {
        pos.z = 2.0;
        vel = vec3(0,0,0);
    }

    // Make sure that the particule aggregate does not disperse in space.
    if(pos.x < -1.2 || pos.x > 1.2)
        pos.x = -pos.x;
    if(pos.y < -1.2 || pos.y > 1.2)
        pos.y = -pos.y;

    // Update particle position and velocity.
    particlePos = pos;
    particleVel = vel;

}
