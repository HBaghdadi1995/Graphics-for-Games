#version 150 core
uniform sampler2D diffuseTex;
uniform sampler2D sobelTex;

in Vertex {
  vec2 texCoord;
} IN;

out vec4 gl_FragColor;

void main(void){
  vec3 diffuse = texture(diffuseTex, IN.texCoord).xyz;
  vec3 sobel = texture(sobelTex, IN.texCoord).xyz;

  gl_FragColor.xyz = diffuse;
  //gl_FragColor.xyz = vec3(1,1,1);
  gl_FragColor.xyz +=  sobel ;
  gl_FragColor.a = 1.0;
}
