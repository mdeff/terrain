#version 330 core

uniform vec3 Id;
uniform vec3 kd;

// First texture. Defined by glActiveTexture and passed by glUniform1i.
uniform sampler2D heightMapTex;

// Environmental texture
uniform sampler2D sandTex;
uniform sampler2D iceMoutainTex;
uniform sampler2D treeTex;
uniform sampler2D stoneTex;
uniform sampler2D waterTex;
uniform sampler2D snowTex;

// Position (world coordinates) after heightmap displacement.
in vec3 displaced;

//light direction
in vec3 light_dir;
//normal vector
in vec3 normal_mv;

// First output buffer is pixel color.
layout(location = 0) out vec3 color;

float mapped_function(float _alpha)
{
	
	if (_alpha <= 0.5)
		return (_alpha*_alpha);
	else 
		return sqrt(_alpha);
}


void main() {

	// Normalize the vectors.
    vec3 L = normalize(light_dir);
    vec3 N = normalize(normal_mv);

	// Compute the diffuse color component.
    vec3 diffuse = Id * kd * max(dot(N,L),0.0);

    // Color dependent on the elevation (similar to texture mapping).
    vec3 mapped;

    const float ground = 0.01f;   
	const float sandMax = 0.015f;
	const float forestMin = 0.02f;   
	const float forestMax = 0.12f;  
    const float snowMin= 0.135f;
    

	if(displaced.z < ground) {
		mapped = texture2D(waterTex, displaced.xy).rgb;
	} else if (displaced.z < sandMax) {
		mapped = texture2D(sandTex, displaced.xy).rgb;
	} else if (displaced.z < forestMin) {  //mix between sand, rock and forest		
		vec3 sandComp = texture2D(sandTex, 60.0*displaced.xy).rgb;
		vec3 forestComp = texture2D(treeTex, 10.0*displaced.xy).rgb;
		vec3 stoneComp = texture2D(stoneTex, 10.0*displaced.xy).rgb;
		float alpha = mapped_function(N.z);
		mapped = (1-4.0*alpha)*forestComp + 2.5*alpha*sandComp + 1.5*alpha*stoneComp;
	} else if (displaced.z  < forestMax) {  //mix between forest and rock
		//mapped = texture2D(treeTex, 10.0*displaced.xy).rgb;
		float alpha = mapped_function(N.z); //clamp(0,1, N.z*N.z);
		mapped = mix(texture2D(treeTex, 10.0*displaced.xy).rgb, texture2D(stoneTex, 10.0*displaced.xy).rgb, alpha);
	} else if (displaced.z < snowMin) { //mix between forest, rock and snow
		vec3 snowComp = texture2D(snowTex, 30.0*displaced.xy).rgb;
		vec3 forestComp = texture2D(treeTex, 10.0*displaced.xy).rgb;
		vec3 stoneComp = texture2D(stoneTex, 10.0*displaced.xy).rgb;
		float alpha = mapped_function(N.z);
		mapped = (1-4.0*alpha)*forestComp + 2.5*alpha*snowComp + 1.5*alpha*stoneComp;
	} else {
		mapped = texture2D(snowTex, displaced.xy).rgb;
	} 

  

    // Assemble the colors.
    color = mapped + diffuse;


}
