#version 330 core                                                                               

layout (location = 0) in vec3 planePositionIn;                                           
uniform mat4 modelMatrix;
uniform mat4 gVP;

void main()
{
	gl_Position =  gVP * modelMatrix * vec4(planePositionIn, 1.0);
}