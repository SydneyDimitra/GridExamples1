#version 420 core

//first attribute in is the vertex position
layout (location=0) in vec3 inVert;
// our model / view  project transform
uniform mat4 MVP;


void main()
{
    gl_Position=MVP*vec4(inVert, 1.0f);
}
