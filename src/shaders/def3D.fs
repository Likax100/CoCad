#version 460 core
out vec4 frag_color;

in vec3 inNormal;

uniform vec3 v3_model_color;
uniform vec3 v3_light_origin;
uniform float diffuse_intensity;

void main() {
  vec3 col;

  // lighting = ambient + diffuse + (specular, which i dont need )
  //Ambient:
  vec3 ambient = vec3(0.15, 0.15, 0.15);
    
  //Diffuse:
  vec3 normal = normalize(inNormal);
  vec3 lightColor = vec3(1.0, 1.0, 1.0);
  vec3 lightSource = v3_light_origin;

  float diffuseStrength = max(0.0, dot(lightSource, normal));
  vec3 diffuse = diffuseStrength * lightColor;

  vec3 lighting = ambient + (diffuse_intensity * diffuse);
  col = v3_model_color * lighting; 

  frag_color = vec4(col.rgb, 1.0); 
}
