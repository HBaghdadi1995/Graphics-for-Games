#version 150 core

uniform sampler2D diffuseTex;

in Vertex {
	vec2 texCoord;
	vec4 colour;
	vec3 	normal;
	vec3 	tangent;
	vec3 	worldPos;
} IN;

out vec4 gl_FragColor;

void main(void){
	vec4 col = texture(diffuseTex, IN.texCoord);
	gl_FragColor = col;
	if(col == vec4(0.0,0.0,0.0,1.0)){
		gl_FragColor.a = 0.0;
		//gl_FragColor.a = 0.0;
	}
}
