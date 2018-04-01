#version 150 core

uniform sampler2D diffuseTex;
uniform vec2 pixelSize;
uniform int isVertical;

in Vertex{
	vec2 texCoord;
	vec4 colour;
} IN;

out vec4 gl_FragColor;

const float weights[5] = float[](0.12,0.22,0.32,0.22,0.12);

mat3 sx = mat3(
    1.0, 2.0, 1.0,
    0.0, 0.0, 0.0,
   -1.0, -2.0, -1.0
);
mat3 sy = mat3(
    1.0, 0.0, -1.0,
    2.0, 0.0, -2.0,
    1.0, 0.0, -1.0
);

void main(void){
	gl_FragColor = vec4(0,0,0,0);

	vec3 diffuse = texture(screenTexture, TexCoords.st).rgb;
    mat3 I;
    for (int i=0; i<3; i++) {
        for (int j=0; j<3; j++) {
            vec3 sample  = texelFetch(screenTexture, ivec2(gl_FragCoord) + ivec2(i-1,j-1), 0 ).rgb;
            I[i][j] = length(sample);
    }

}
