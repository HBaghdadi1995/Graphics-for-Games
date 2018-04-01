#version 150 core

uniform sampler2D diffuseTex;
uniform sampler2D diffuseTex2;

in Vertex {
	vec2 texCoord;
	vec4 colour;
} IN;

out vec4 gl_FragColor;

void main(void){
	gl_FragColor = mix(texture(diffuseTex, IN.texCoord),texture(diffuseTex2, IN.texCoord),0.5);
	//gl_FragColor = IN.colour;
	//gl_FragColor = texture(diffuseTex2, IN.texCoord);
}