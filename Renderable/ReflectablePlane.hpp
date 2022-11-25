#pragma once
#include <glm/glm.hpp>
//#include <model.h>
#include "../Engine/Texture.hpp"
//#include "TessShader.h"
#include "PlaneCounter.hpp"
#include <Camera.hpp>

#include "Drawable.hpp"
#include "PlaneCounter.hpp"

#include "Skybox.hpp"

enum tPosition {
	C, N, S, E, W, SE, SW, NE, NW, totTiles
};

// The purpose of this class is to solve a bug with cloud rendering due to how ray casting works within the ray cast shaders.
// Without it, the clouds will quite literally bend and distort over the screen, even if the thing itself looks bad.
class ReflectablePlane : public Drawable
{
public:

	ReflectablePlane(int gl);
	
	virtual ~ReflectablePlane();
	virtual void draw();
	virtual void setGui();

	void updateTilesPositions();
	void setPositionsArray(std::vector<glm::vec2> & pos);
	void setSkybox(Skybox& skyboxSet)
	{
		this->skybox = &skyboxSet;
	}
	Skybox* skybox;
	glm::vec2 position, eps;
	float up = 0.0;
	
	bool inTile(Camera camera, glm::vec2 pos);
	static const int tileW = 10.*100.;

	//Model * planeModel;
	PlaneCounter * waterPtr = NULL;

	ReflectablePlane& setUp(float scale)
	{
		this->up = scale;

		return *this;
	}

	void setOctaves(int oct) {
		if (oct > 0) octaves = oct;
	}

	void setFreq(float freq) {
		if (freq > 0.0f) { this->frequency = freq; }
	}

	void setDispFactor(float disp) {
		if (disp > 0.0f) { dispFactor = disp; }
	}

	void setScale(float scale) {
		glm::mat4 id;
		glm::mat4 scaleMatrix = glm::scale(id, glm::vec3(scale, 0.0, scale));
		glm::mat4 positionMatrix = glm::translate(id, glm::vec3(position.x*scale/this->scaleFactor, 0.0, position.y*scale / this->scaleFactor));
		modelMatrix = positionMatrix * scaleMatrix;
		scaleFactor = scale;
	}

	void setGrassCoverage(float gc) {
		grassCoverage = gc;
	}

	void setTessMultiplier(float tm) {
		if (tm > 0.0) tessMultiplier = tm;
	}

	int getOctaves() const { return octaves; }
	float getFreq() const { return frequency; }
	float getDispFactor() const { return dispFactor; }
	float getScale() const { return scaleFactor; }
	float getGrassCoverage() const { return grassCoverage; }
	float getTessMultiplier() const { return tessMultiplier; }

private:

	void deleteBuffer();
	int res;
	unsigned int planeVBO, planeVAO, planeEBO;
	float dispFactor, scaleFactor, frequency, grassCoverage, tessMultiplier, fogFalloff, power;
	int octaves;
	int gridLength;
	glm::vec3 rockColor;

	static bool drawFog;
	unsigned int * textures, posBuffer;

	Shader * shad;
	glm::mat4 modelMatrix;

	std::vector<glm::vec2> positionVec;

	void drawVertices(int nInstances);

	void setPos(int row, int col, glm::vec2 pos) {
		positionVec[col + row * gridLength] = pos;
	}

	glm::vec2 getPos(int row, int col) {
		return positionVec[col + row * gridLength];
	}

	void generateTileGrid(glm::vec2 offset);
	bool getWhichTileCameraIs(glm::vec2& result);

	void getColRow(int i, int& col, int& row) {
		col = (i) % gridLength;

		row = (i - col) / gridLength;
	}

	void reset();
};

