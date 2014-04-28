#version 330 core

uniform vec3 Ia, Id, Is;
uniform vec3 ka, kd, ks;
uniform float p;



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
// Position (camera coordinates) after heightmap displacement.
in vec3 displaced_mv;

//light direction
in vec3 light_dir;
//view direction
in vec3 view_dir;



// First output buffer is pixel color.
layout(location = 0) out vec3 color;


void main() {

	//first calculate the normal vector using finite difference
	vec3 X = dFdx(displaced);
	vec3 Y = dFdy(displaced);
	
	//normal vector in world coordinate
	vec3 N= normalize(cross(X,Y));	

	// Normalize the vectors.	
    vec3 L = normalize(light_dir);
	vec3 V = normalize(view_dir);

	// Compute the diffuse color component.
    vec3 diffuse = Id * kd * max(dot(N,L),0.0);

	// Compute the specular color component
	vec3 specular = Is * ks * pow(max(dot(V,reflect(L,N)),0.0),p);

    // Color dependent on the elevation (similar to texture mapping).
    vec3 mapped;

    const float ground = 0.01f;   
	const float sandMax = 0.015f;
	const float forestMin = 0.025f;   
	const float forestMax = 0.15f;  
    const float snowMin= 0.175f;
    
	float slope = smoothstep(0.45, 0.85 , N.z);
	
	if(displaced.z < ground) {
		mapped = texture2D(waterTex, displaced.xy).rgb;
	} else if (displaced.z < sandMax) {
		mapped = texture2D(sandTex, displaced.xy).rgb;
	} else if (displaced.z < forestMin) {  //mix between sand, rock			
		mapped = texture2D(sandTex, displaced.xy).rgb;
	} else if (displaced.z  < forestMax) {  //mix between forest and rock
		vec3 stone = texture2D(stoneTex, 10*displaced.xy).rgb;
		vec3 forest = texture2D(treeTex, 10*displaced.xy).rgb;
		mapped = mix(forest, stone, slope);
	} else if (displaced.z < snowMin) { //mix between forest, rock and snow
		vec3 stone = texture2D(stoneTex, 10*displaced.xy).rgb;
		vec3 snow = texture2D(snowTex, 30*displaced.xy).rgb;
		mapped = mix(snow, stone, slope);
	} else {
		mapped = texture2D(snowTex, displaced.xy).rgb;
	} 

  
	//Ambient color component
	vec3 ambient = Ia * ka * mapped;
    // Assemble the colors.
    color = ambient + diffuse + specular;
	

}
