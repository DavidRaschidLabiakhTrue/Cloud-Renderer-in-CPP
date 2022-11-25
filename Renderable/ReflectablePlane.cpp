#include "ReflectablePlane.hpp"
#include "Scene.hpp"
#include <GLFW/glfw3.h>
#include "../imgui/imgui.h"
#include "../Engine/Utils.hpp"

#include "Skybox.hpp"

bool ReflectablePlane::drawFog = true;

float sign(float x) 
{
	if (x > 0.0f) return 1.0f;
	else if (x < 0.0f) { return -1.0f; }
	else return 0.0f;
}


ReflectablePlane::ReflectablePlane(int gl)
{


	glm::mat4 id;
	glm::mat4 scaleMatrix = glm::scale(id, glm::vec3(1.0, 0.0, 1.0));
	glm::mat4 positionMatrix = glm::translate(id, glm::vec3(0., 0.0, 0.));
	modelMatrix = positionMatrix;

	octaves = 0;
	frequency = 0;
	grassCoverage = 0;
	tessMultiplier = 0;

	fogFalloff = 0;

	posBuffer = 0;

	shad = new Shader("TerrainTessShader");
	shad->attachShader("shaders/terrain.vert")->attachShader("shaders/terrain.frag")->linkPrograms();

	this->gridLength = gl + (gl + 1) % 2; //ensure gridLength is odd


	res = 4;
	initializePlaneVAO(res, tileW, &planeVAO, &planeVBO, &planeEBO);



	positionVec.resize(gridLength*gridLength);
	generateTileGrid(glm::vec2(0.0,0.0));

	setPositionsArray(positionVec);

	rockColor = glm::vec4(120, 105, 75, 255)*1.5f / 255.f;
	power = .2;
}

void ReflectablePlane::generateTileGrid(glm::vec2 offset)
{
	float sc = tileW;

	glm::vec2 I = glm::vec2(1, 0)*sc;
	glm::vec2 J = glm::vec2(0, 1)*sc;

	for (int i = 0; i < gridLength; i++) {
		for (int j = 0; j < gridLength; j++) {
			glm::vec2 pos = (float)(j - gridLength / 2)*glm::vec2(I) + (float)(i - gridLength / 2)*glm::vec2(J);
			setPos(i, j, pos + offset);
		}
	}
}

void ReflectablePlane::deleteBuffer(){
	glDeleteBuffers(1, &posBuffer);
	posBuffer = 0;
}

bool ReflectablePlane::getWhichTileCameraIs(glm::vec2& result) {

	for (glm::vec2 p : positionVec) {
		if (inTile(*(scene->cam), p)) {
			//std::cout << "You're in Tile: " << p.x << ", " << p.y << std::endl;
			result = p;
			return true;
		}
	}
	return false;
}


void ReflectablePlane::draw(){


	Scene* se = Drawable::scene;

	drawFog = !se->wireframe;

	if (up != 0.0f) 
	{
		glEnable(GL_CLIP_DISTANCE0);
	}
	glm::mat4 gWorld = modelMatrix;
	glm::mat4 gVP = se->proj * se->cam->GetViewMatrix();

	shad->use();
	shad->setVec3("gEyeWorldPos", se->cam->Position);
	shad->setMat4("gWorld", gWorld);
	shad->setMat4("gVP", gVP);
	shad->setFloat("gDispFactor", dispFactor);

	float waterHeight = (waterPtr ? waterPtr->getModelMatrix()[3][1] : 100.0);
	glm::vec4 clipPlane(0.0, 1.0, 0.0, -0);
	shad->setVec4("clipPlane", clipPlane*up);
	shad->setVec3("u_LightColor", se->lightColor);
	shad->setVec3("u_LightPosition", se->lightPos);
	shad->setVec3("u_ViewPosition", se->cam->Position);
	shad->setVec3("fogColor", se->altFog);
	shad->setVec3("rockColor", rockColor);
	shad->setVec3("seed", se->seed);

	shad->setInt("octaves", octaves);
	shad->setFloat("freq", frequency);
	shad->setFloat("u_grassCoverage", grassCoverage);
	shad->setFloat("waterHeight", 0);
	shad->setFloat("tessMultiplier", tessMultiplier);
	shad->setFloat("fogFalloff", fogFalloff*1.e-6);
	shad->setFloat("power", power);

	shad->setBool("normals", true);
	shad->setBool("drawFog", ReflectablePlane::drawFog);


	

	int nIstances = positionVec.size();

	drawVertices(nIstances);


	glDisable(GL_CLIP_DISTANCE0);
	up = 0.0;
}

void ReflectablePlane::setGui()
{

}

void ReflectablePlane::drawVertices(int nInstances) {
	glBindVertexArray(planeVAO);
	//shader.use();
	shad->use();
	glDrawElementsInstanced(GL_PATCHES, (res-1)*(res-1)*2*3, GL_UNSIGNED_INT, 0, nInstances); // DRAWS INSTANCED PLANES
	glBindVertexArray(0);
}

void ReflectablePlane::setPositionsArray(std::vector<glm::vec2> & pos) {
	if (posBuffer) {
		this->deleteBuffer();
	}

	// vertex Buffer Object
	glGenBuffers(1, &posBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, posBuffer);
	glBufferData(GL_ARRAY_BUFFER, pos.size() * sizeof(glm::vec2), &pos[0], GL_STATIC_DRAW);

	glBindVertexArray(planeVAO);
	glEnableVertexAttribArray(3);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, sizeof(glm::vec2), (void*)0);

	glVertexAttribDivisor(3, 1);
	glBindVertexArray(0);
	
}

bool ReflectablePlane::inTile(const Camera camera, glm::vec2 pos) {
	float camX = camera.Position.x;
	float camY = camera.Position.z;

	float x = pos.x;
	float y = pos.y;

	bool inX = false;
	if ((camX <= x + 1.0 * tileW/2.0f) && (camX >= x - 1.0 * tileW/2.0f)) { inX = true; }
	bool inY = false;
	if ((camY <= y + 1.0 * tileW/2.0f) && (camY >= y - 1.0 * tileW/2.0f)) { inY = true; }

	bool result = inX && inY;

	if (result) {

		//std::cout << y << " :y" << std::endl;
		//std::cout << y << " :y" << std::endl;

		//std::cout << y + scaleFactor * tileW / 2.0f << ": y + scalefactor" << std::endl;
		//std::cout << y - scaleFactor * tileW / 2.0f << ": y - scalefactor" << std::endl;
	}

	return result;

}


ReflectablePlane::~ReflectablePlane()
{

}

void ReflectablePlane::updateTilesPositions() {
	Scene* se = Drawable::scene;
	glm::vec2 camPosition(se->cam->Position.x, se->cam->Position.z);
	int whichTile = -1;
	int howManyTiles = 0;

	glm::vec2 currentTile;
	if (getWhichTileCameraIs(currentTile)) {
		glm::vec2 center = getPos(gridLength / 2, gridLength / 2);
		for (glm::vec2& p : positionVec) {
			p += currentTile - center;
		}
		setPositionsArray(positionVec);

		if (waterPtr) {
			glm::vec2 center = getPos(gridLength / 2, gridLength / 2);
			waterPtr->setPosition(center, 1.0*gridLength, waterPtr->getHeight());
		}
	}
}


void ReflectablePlane::reset() {
	int octaves = this->getOctaves();
	float freq = this->getFreq();
	float grassCoverage = this->getGrassCoverage();
	float dispFactor = this->getDispFactor();
	float tessMultiplier = this->getTessMultiplier();

	setOctaves(octaves);
	setFreq(freq);
	setGrassCoverage(grassCoverage);
	setDispFactor(dispFactor);
	setTessMultiplier(tessMultiplier);
}
