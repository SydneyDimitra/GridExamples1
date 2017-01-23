#version 420 core

layout (location=0) out vec4 fragColour;

uniform vec4 colour;
in vec3 lightIntensity;

void main()
{
 //   fragColour=vec4(1.0);
 //       fragColour=colour;
    fragColour.rgb=lightIntensity;

}
