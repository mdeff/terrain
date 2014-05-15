#version 330 core

// Light properties.
uniform vec3 Ia, Id, Is;

// Texture 0. Defined by glActiveTexture and passed by glUniform1i.
uniform sampler2D heightMapTex;

// Environmental textures 1-6. Defined by glActiveTexture and passed by glUniform1i.
uniform sampler2D sandTex, iceMoutainTex, treeTex, stoneTex, underWaterTex, snowTex;


// Vertices 3D position (after heightmap displacement) in model space.
in vec3 vertexPosition3DModel;

// Vertex position in light source clip space.
in vec4 ShadowCoord;

// Light and view directions.
in vec3 lightDir, viewDir;

// First output buffer is pixel color.
// gl_FragColor
layout(location = 0) out vec3 color;


void main(){
	color = vec3(0.0,0.4,0.5);
}
	