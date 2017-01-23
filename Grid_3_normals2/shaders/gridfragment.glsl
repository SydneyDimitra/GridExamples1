#version 420 core

layout (location=0) out vec4 fragColour;

uniform vec4 colour;
in float lightIntensity;
in vec3 vertColour;

void main()
{
 //   fragColour=vec4(1.0);
 //       fragColour=colour;
    fragColour.rgb=vertColour*lightIntensity;

}
