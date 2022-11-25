#version 430 core                                                                               
                                                                                                
layout (location = 0) in vec3 planePositionIn;                                           
layout (location = 1) in vec3 planeNormalIn;   
layout (location = 2) in vec2 texCoordinateIn;                                                  
layout (location = 3) in vec2 a_Position;
			  
uniform mat4 gWorld;          
                                                                                                
out vec3 WorldPos_CS_in;                                                                        
out vec2 TexCoord_CS_in;                                                                        
out vec3 Normal_CS_in;                                                                          
                                                                                                
void main()                                                                                     
{         
    WorldPos_CS_in = (gWorld * vec4(planePositionIn, 1.0)).xyz;
	WorldPos_CS_in.xz += a_Position;
	Normal_CS_in  = planeNormalIn;                                  
    TexCoord_CS_in = texCoordinateIn;                                                            

}
