#version 420 core

//first attribute in is the vertex position
layout (location=0) in vec3 inVert;
layout (location=1) in vec3 normal;
// our model / view  project transform
uniform mat4 MVP;
uniform mat4 MV;
uniform mat3 normalMatrix;
out vec3 lightIntensity;
// position of light (directional light)
uniform vec3 lightPos=vec3(5,5,5);
uniform vec4 colour;

void main()
{
    gl_Position=MVP*vec4(inVert, 1.0f);

    vec3 tnorm = normalize(normalMatrix * normal);
    vec4 eyeCoords = MV * vec4(inVert, 1.0);
    vec3 s = normalize(vec3(lightPos-eyeCoords.xyz));
    lightIntensity=colour.rgb*max(dot(s, tnorm), 0.0);

}
