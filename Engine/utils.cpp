#include "Utils.hpp"
#include <random>

void initializePlaneVAO(const int planeResolution, const int width, GLuint * vao, GLuint * vbo, GLuint * ebo) {

	//const int res = 3;
	const int nPoints = planeResolution * planeResolution;
	const int size = nPoints * 3 + nPoints * 3 + nPoints * 2;
	float * vertices = new float[size];
	for (int i = 0; i < planeResolution; i++) {
		for (int j = 0; j < planeResolution; j++) {
			//add position
			float x = j * (float)width / (planeResolution - 1) - width / 2.0;
			float y = 0.0;
			float z = -i * (float)width / (planeResolution - 1) + width / 2.0;

			vertices[(i + j * planeResolution) * 8] = x; //8 = 3 + 3 + 2, float per point
			vertices[(i + j * planeResolution) * 8 + 1] = y;
			vertices[(i + j * planeResolution) * 8 + 2] = z;

			//add normal
			float x_n = 0.0;
			float y_n = 1.0;
			float z_n = 0.0;

			vertices[(i + j * planeResolution) * 8 + 3] = x_n;
			vertices[(i + j * planeResolution) * 8 + 4] = y_n;
			vertices[(i + j * planeResolution) * 8 + 5] = z_n;

			//add texcoords
			vertices[(i + j * planeResolution) * 8 + 6] = (float)j / (planeResolution - 1);
			vertices[(i + j * planeResolution) * 8 + 7] = (float)(planeResolution - i - 1) / (planeResolution - 1);
		}
	}

	const int nTris = (planeResolution - 1)*(planeResolution - 1) * 2;
	int * trisIndices = new int[nTris * 3];

	for (int i = 0; i < nTris; i++) {
		int trisPerRow = 2 * (planeResolution - 1);
		for (int j = 0; j < trisPerRow; j++) {
			if (!(i % 2)) { //upper triangle
				int k = i * 3;
				int triIndex = i % trisPerRow;

				int row = i / trisPerRow;
				int col = triIndex / 2;
				trisIndices[k] = row * planeResolution + col;
				trisIndices[k + 1] = ++row*planeResolution + col;
				trisIndices[k + 2] = --row* planeResolution + ++col;
			}
			else {
				int k = i * 3;
				int triIndex = i % trisPerRow;

				int row = i / trisPerRow;
				int col = triIndex / 2;
				trisIndices[k] = row * planeResolution + ++col;
				trisIndices[k + 1] = ++row * planeResolution + --col;
				trisIndices[k + 2] = row * planeResolution + ++col;
			}
		}
	}

	/*
	for (int i = 0; i < res; i++) {
		for (int j = 0; j < res; j++) {
			for (int k = 0; k < 8; k++) {
				if (k == 3 || k == 6)
					std::cout << std::endl;
				std::cout << vertices[(i + j * res) * 8 + k] << ", ";
			}
			std::cout << std::endl;
		}
	}

	for (int i = 0; i < nTris; i++) {
		for (int k = 0; k < 3; k++)
		{
			std::cout << trisIndices[i * 3 + k] << ", ";
		}
		std::cout << std::endl;

	}

	std::cout << "TRISINDICES: " << nTris * 3 << std::endl;
	*/
	glGenVertexArrays(1, vao);
	glGenBuffers(1, vbo);
	glGenBuffers(1, ebo);

	glBindVertexArray(*vao);

	glBindBuffer(GL_ARRAY_BUFFER, *vbo);
	glBufferData(GL_ARRAY_BUFFER, size * sizeof(float), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, *ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, nTris * 3 * sizeof(unsigned int), trisIndices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);

	delete[] vertices;
}

