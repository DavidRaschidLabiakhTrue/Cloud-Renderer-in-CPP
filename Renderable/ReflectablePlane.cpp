#include "ReflectablePlane.hpp"
#include "Scene.hpp"
#include <GLFW/glfw3.h>
#include "../imgui/imgui.h"
#include "../CoreWorkerBackend/Utils.hpp"

#include "Skybox.hpp"

bool ReflectablePlane::drawFog = false;



using glm::vec2;




float sign(float x) 
{
	if (x > 0.0f) return 1.0f;
	else if (x < 0.0f) { return -1.0f; }
	else return 0.0f;
}


ReflectablePlane::ReflectablePlane(int gl)
{
	modelMatrix = glm::mat4(1.0f);;



	// IGNORE DEAD
	planeOctaves = 0;
	planeFrequencyFactor = 0;
	planeCoverage = 0;
	planeFractureMultiplier = 0;
	planeFallOff = 0;
	posBuffer = 0;
	// IGNORE DEAD

	shader = new Shader("Reflectable Plane");
	shader->attachShader("shaders/ReflectablePlane.vert")->attachShader("shaders/ReflectablePlane.frag")->linkPrograms();

	this->planeGridLength = gl + (gl + 1) % 2; // gridLength must be odd


	planeResolution = 4;
	initializePlaneVAO(planeResolution, tileW, &vao, &vbo, &ebo);



	positionVec.resize(planeGridLength*planeGridLength);
	generateTileGrid(vec2(0.0,0.0));

	setPositionsArray(positionVec);

	planeColor = glm::vec4(120, 105, 75, 255)*1.5f / 255.f;
	planeFracturePower = .2;
}

void ReflectablePlane::generateTileGrid(vec2 offset)
{
	float sc = tileW;

	vec2 I = vec2(1, 0)*sc;
	vec2 J = vec2(0, 1)*sc;

	for (int i = 0; i < planeGridLength; i++) 
	{
		for (int j = 0; j < planeGridLength; j++) 
		{
			vec2 pos = (float)(j - planeGridLength / 2)*vec2(I) + (float)(i - planeGridLength / 2)*vec2(J);
			setPos(i, j, pos + offset);
		}
	}
}

void ReflectablePlane::deleteBuffer()
{
	glDeleteBuffers(1, &posBuffer);
	posBuffer = 0;
}

bool ReflectablePlane::getWhichTileCameraIs(vec2& result) 
{

	for (vec2 p : positionVec) 
	{
		if (inTile(*(scene->cam), p)) 
		{
			result = p;
			return true;
		}
	}
	return false;
}


void ReflectablePlane::draw()
{
	Scene* se = Drawable::scene;

	drawFog = !se->wireframe;

	if (up != 0.0f) 
	{
		glEnable(GL_CLIP_DISTANCE0);
	}
	glm::mat4 gWorld = modelMatrix;
	glm::mat4 gVP = se->proj * se->cam->GetViewMatrix();

	shader->use();
	shader->uploadVector3ToGPU("gEyeWorldPos", se->cam->cameraPosition);
	shader->uploadMatrix4ToGPU("gWorld", gWorld);
	shader->uploadMatrix4ToGPU("gVP", gVP);
	shader->uploadFloatToGPU("gDispFactor", planeDispersion);

	glm::vec4 clipPlane(0.0, 1.0, 0.0, -0);
	shader->uploadVector4ToGPU("clipPlane", clipPlane*up);
	shader->uploadVector3ToGPU("u_LightColor", se->lightColor);
	shader->uploadVector3ToGPU("u_LightPosition", se->lightPos);
	shader->uploadVector3ToGPU("u_ViewPosition", se->cam->cameraPosition);
	shader->uploadVector3ToGPU("fogColor", se->altFog);
	shader->uploadVector3ToGPU("rockColor", planeColor);
	shader->uploadVector3ToGPU("seed", se->seed);


	shader->uploadFloatToGPU("power", planeFracturePower);




	

	int nIstances = positionVec.size();

	drawVertices(nIstances);


	glDisable(GL_CLIP_DISTANCE0);
	up = 0.0;
}

void ReflectablePlane::setGui()
{

}

void ReflectablePlane::drawVertices(int nInstances) 
{
	glBindVertexArray(vao);
	shader->use();
	glDrawElementsInstanced(GL_PATCHES, (planeResolution-1)*(planeResolution-1)*2*3, GL_UNSIGNED_INT, 0, nInstances); // DRAWS INSTANCED PLANES
	glBindVertexArray(0);
}

void ReflectablePlane::setPositionsArray(std::vector<vec2> & pos) 
{
	if (posBuffer) 
	{
		this->deleteBuffer();
	}

	// vertex Buffer Object
	glGenBuffers(1, &posBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, posBuffer);
	glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(vec2), &pos[0], GL_STATIC_DRAW);

	glBindVertexArray(vao);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), (void*)0);

	glVertexAttribDivisor(3, 1);
	glBindVertexArray(0);
	
}

bool ReflectablePlane::inTile(const Camera camera, vec2 pos) 
{
	using glm::vec2;

	vec2 camPos(camera.cameraPosition);
	vec2 thisPos(pos);

	glm::bvec2 in(false, false);


	if ((camPos.x <= thisPos.x + 1.0 * tileW/2.0f) && (camPos.x >= thisPos.x - 1.0 * tileW/2.0f))
	{ 
		in.x = true; 
	}
	bool inY = false;
	if ((camPos.y <= thisPos.y + 1.0 * tileW/2.0f) && (camPos.y >= thisPos.y - 1.0 * tileW/2.0f))
	{ 
		in.y = true; 
	}

	return glm::all(in);

}


ReflectablePlane::~ReflectablePlane()
{

}

void ReflectablePlane::updateTilesPositions() 
{
	Scene* se = Drawable::scene;
	vec2 camPosition(se->cam->cameraPosition.x, se->cam->cameraPosition.z);
	int whichTile = -1;
	int howManyTiles = 0;

	vec2 currentTile;
	if (getWhichTileCameraIs(currentTile)) 
	{
		vec2 center = getPos(planeGridLength / 2, planeGridLength / 2);
		for (vec2& p : positionVec) 
		{
			p += currentTile - center;
		}
		setPositionsArray(positionVec);

		if (counterPlanePointer) 
		{
			vec2 center = getPos(planeGridLength / 2, planeGridLength / 2);
			counterPlanePointer->setPosition(center, 1.0*planeGridLength, counterPlanePointer->getHeight());
		}
	}
}


