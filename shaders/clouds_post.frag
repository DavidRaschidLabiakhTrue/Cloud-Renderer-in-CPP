#version 330 core

layout (location = 0) out vec4 FragColor;
in vec2 TexCoords;

uniform sampler2D clouds;
uniform vec2 cloudRenderResolution;

#define  offset_x  1. / cloudRenderResolution.x  
#define  offset_y  1. / cloudRenderResolution.y

const float kernel[9] = float[]
(
	1.0 / 16, 2.0 / 16, 1.0 / 16,
	2.0 / 16, 4.0 / 16, 2.0 / 16,
	1.0 / 16, 2.0 / 16, 1.0 / 16  
);

vec4 gaussianBlur(sampler2D tex, vec2 uv)
{
	 vec2 offsets[9] = vec2[](
			vec2(-offset_x,  offset_y), // top-left
			vec2( 0.0f,    offset_y), // top-center
			vec2( offset_x,  offset_y), // top-right
			vec2(-offset_x,  0.0f),   // center-left
			vec2( 0.0f,    0.0f),   // center-center
			vec2( offset_x,  0.0f),   // center-right
			vec2(-offset_x, -offset_y), // bottom-left
			vec2( 0.0f,   -offset_y), // bottom-center
			vec2( offset_x, -offset_y)  // bottom-right    
		);


    
    vec4 sampleTex[9];

    for(int i = 0; i < 9; i++)
    {	
        sampleTex[i] = texture(tex, uv.st + offsets[i]);
    }

    vec4 col = vec4(0.0);
    for(int i = 0; i < 9; i++)
	{
        col += sampleTex[i] * kernel[i];
    }
    return col;
}

void main()
{
	FragColor = gaussianBlur(clouds, TexCoords);
}  