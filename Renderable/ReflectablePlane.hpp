#pragma once
#include <glm/glm.hpp>
//#include <model.h>
#include "../CoreWorkerBackend/Texture.hpp"
//#include "TessShader.h"
#include "PlaneCounter.hpp"
#include <Camera.hpp>

#include "Drawable.hpp"
#include "PlaneCounter.hpp"

#include "Skybox.hpp"


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
		void setPositionsArray(std::vector<glm::vec2>& pos);
		void setSkybox(Skybox& skyboxSet)
		{
			this->skybox = &skyboxSet;
		}
		Skybox* skybox;
		glm::vec2 position, eps;
		float up = 0.0;
	
		bool inTile(Camera camera, glm::vec2 pos);
		static const int tileW = 10.*100.;
		PlaneCounter * counterPlanePointer = NULL;
		ReflectablePlane& setUp(float scale)
		{
			this->up = scale;

			return *this;
		}
		void deleteBuffer();
		int planeResolution;
		unsigned int vbo;
		unsigned int vao;
		unsigned int ebo;

		// ALL DEAD VARIABLES
		float planeDispersion;
		float planeScaleFactor;
		float planeFrequencyFactor;
		float planeCoverage;
		float planeFractureMultiplier;
		float planeFallOff;
		float planeFracturePower; 
		int planeOctaves;
		int planeGridLength;
		glm::vec3 planeColor;
		// ALL DEAD VARIABLES


		static bool drawFog;
		unsigned int * textures;
		unsigned int posBuffer;

		Shader * shader;
		glm::mat4 modelMatrix;

		std::vector<glm::vec2> positionVec;

		void drawVertices(int nInstances);

		void setPos(int row, int col, glm::vec2 pos) 
		{
			positionVec[col + row * planeGridLength] = pos;
		}

		glm::vec2 getPos(int row, int col) 
		{
			return positionVec[col + row * planeGridLength];
		}

		void generateTileGrid(glm::vec2 offset);
		bool getWhichTileCameraIs(glm::vec2& result);



	
};

