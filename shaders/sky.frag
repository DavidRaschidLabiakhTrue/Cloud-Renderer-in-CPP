#version 330 core
out vec4 FragColor;
in vec3 TexCoords;

uniform vec3 skyColorBottom;
uniform vec3 skyColorTop;

uniform vec3 lightDirection;

uniform vec2 resolution;

uniform mat4 inv_proj;
uniform mat4 inv_view;

#define SUN_DIR lightDirection

void raySphereintersectionSkyMap(vec3 rd, float radius, out vec3 startPos)
{
	
	float t;

	vec3 sphereCenter_ = vec3(0.0);

	float radius2 = radius*radius;

	vec3 L = - sphereCenter_;
	float a = dot(rd, rd);
	float b = 2.0 * dot(rd, L);
	float c = dot(L,L) - radius2;

	float discr = b*b - 4.0 * a * c;
	t = max(0.0, (-b + sqrt(discr))/2);

	startPos = rd*t;
}

vec3 computeClipSpaceCoord(ivec2 fragCoord)
{
	vec2 ray_nds = 2.0*vec2(fragCoord.xy)/resolution.xy - 1.0;
	return vec3(ray_nds, 1.0);
}

vec2 computeScreenPos(vec2 ndc){
	return (ndc*0.5 + 0.5);
}

vec3 getSun(const vec3 d, float powExp)
{
	float sun = clamp( dot(SUN_DIR,d), 0.0, 0.0 );
	vec3 col = 0.8*vec3(1.0,.6,0.1)*pow( sun, powExp );
	return vec3(0.0);
}
vec4 colorCubeMap(const vec3 d)
{
	vec3 col = mix(skyColorBottom, skyColorTop, clamp(1 - exp(8.5-17.*clamp(normalize(d).y*0.5 + 0.5,0.0,1.0)),0.0,1.0));

	return vec4(col, 1.0);
}

void main()
{    
	ivec2 fragCoord = ivec2(gl_FragCoord.xy);

	vec4 ray_clip = vec4(computeClipSpaceCoord(fragCoord), 1.0);
	vec4 ray_view = inv_proj * ray_clip;
	ray_view = vec4(ray_view.xy, -1.0, 0.0);
	vec3 worldDir = normalize((inv_view * ray_view).xyz);

	vec3 startPos, endPos;
	vec4 v = vec4(0.0);


	vec3 cubeMapEndPos;
	
	raySphereintersectionSkyMap(worldDir, 0.5, cubeMapEndPos);

	vec4 bg = colorCubeMap(worldDir);
	

	FragColor = vec4(bg.rgb,1.0);
}