#version 410

uniform mat4 invV;

uniform vec3 vLightPos;

uniform sampler2D textureSampler;
uniform sampler2D normalSampler;
uniform vec3 color;
uniform float opacity;

in vec3 normal;
in vec3 pos;
in vec3 tang;
in vec2 tc;

out vec4 FragColor;

void main() {
    vec3 lightDir = normalize(vLightPos-pos);
    vec3 nNormal = normalize(normal);
    vec3 nTangent = normalize(tang);
    vec3 nBinorm = normalize(cross(nNormal,nTangent));
    
    nNormal = (normalize(mat3(nTangent,nBinorm,nNormal) * ((texture(normalSampler, tc ).rgb-0.5)*2.0)));
    
    
    vec3 eyePos = (invV * vec4(0.0,0.0,0.0,1.0)).xyz;
    
    vec3 eyeDir = normalize(eyePos-pos);

    vec3 texValue = color * texture(textureSampler, tc ).rgb;
    vec3 r = reflect(-lightDir, nNormal);
    
    vec3 specular = vec3(1.0,1.0,1.0) * pow(max(0.0,dot(eyeDir,r)),12.0);
    vec3 diffuse  = texValue * max(0.0,dot(lightDir,nNormal));
    vec3 ambient  = texValue * vec3(0.2,0.2,0.2);
    
    FragColor = vec4(specular+diffuse+ambient, opacity);
}
