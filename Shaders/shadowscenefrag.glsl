#version 150 core

uniform sampler2D diffuseTex;
uniform sampler2D bumpTex;
uniform sampler2DShadow shadowTex;

uniform vec4 lightColour;
uniform vec3 lightPos;
uniform vec3 cameraPos;
uniform float lightRadius;
uniform float snowPileup;

in Vertex{
  vec3 colour;
  vec2 texCoord;
  vec3 normal;
  vec3 tangent;
  vec3 binormal;
  vec3 worldPos;
  vec4 shadowProj;
  vec3 position;
} IN;

out vec4 gl_FragColor;

void main(void){
  mat3 TBN = mat3(IN.tangent, IN.binormal, IN.normal);
  //vec3 normal = normalize(TBN * (texture2D(bumpTex, IN.texCoord).rgb * 2.0 - 1.0));
  vec3 normal = IN.normal;


  vec4 diffuse = texture2D(diffuseTex, IN.texCoord);
  //if(diffuse == vec4(0,0,0,1)){
  //vec4 diffuse = vec4(0,1,0,1);
    //diffuse = vec4(normal,1.0);
  //}

  vec3 incident = normalize(lightPos - IN.worldPos);
  float lambert = max(0.0,dot(incident, normal));

  float dist = length(lightPos - IN.worldPos);
  float atten = 1.0 - clamp(dist / lightRadius, 0.0, 1.0);

  vec3 viewDir = normalize(cameraPos - IN.worldPos);
  vec3 halfDir = normalize(incident + viewDir);

  float rFactor = max(0.0,dot(halfDir,normal));
  float sFactor = pow(rFactor,33.0);

  float shadow = 1.0;

  if(IN.shadowProj.w > 0.0){
    shadow = textureProj(shadowTex,IN.shadowProj);
  }

  lambert *= shadow;

  vec3 colour = (diffuse.rgb * lightColour.rgb);
  colour += (lightColour.rgb * sFactor) * 0.33;

  //if(IN.position.y > 0){
  colour = vec3(0.0,0.5,0.0);
  if((IN.position.y*2 + IN.worldPos.y)/3 > 500 - snowPileup ){
    colour = vec3(1,1,1);
  }
  //}

  gl_FragColor = vec4(colour * atten * lambert, 1);
  //gl_FragColor = vec4(colour, diffuse.a);
  //gl_FragColor = vec4(normal, diffuse.a);
  gl_FragColor.rgb += (diffuse.rgb * lightColour.rgb) * 0.1;
    //gl_FragColor = vec4(1,0,0,1);
  //gl_FragColor.rgb = normal;
}
