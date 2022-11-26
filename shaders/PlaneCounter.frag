#version 330 core                                                                               
out vec4 FragColor;
void main()
{
	FragColor.a = 1.0f;
	FragColor.rgb = vec3(0.1, .3, .2);
}