#version 150 core

in Vertex{
	vec2 texCoord;
	vec4 colour;
	vec3 	normal;
	vec3 	tangent;
	vec3 	worldPos;
} IN;

out vec4 gl_FragColor;

uniform sampler2D screenTexture;
uniform vec2 pixelSize;

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

void main()
{
    vec3 diffuse = texture(screenTexture, IN.texCoord).rgb;
    mat3 I;
    for (int i=0; i<3; i++) {
        for (int j=0; j<3; j++) {
            vec3 sample  = texelFetch(screenTexture, ivec2(gl_FragCoord) + ivec2((i-1)*pixelSize.x,j-1*pixelSize.y), 0 ).rgb;
            I[i][j] = length(sample);
          }
        }

        float gx = dot(sx[0], I[0]) + dot(sx[1], I[1]) + dot(sx[2], I[2]);
        float gy = dot(sy[0], I[0]) + dot(sy[1], I[1]) + dot(sy[2], I[2]);

        float g = sqrt(pow(gx, 2.0)+pow(gy, 2.0));
        gl_FragColor = vec4(diffuse - vec3(g), 1.0);
}
