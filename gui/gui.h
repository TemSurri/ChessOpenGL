#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "../game.h"
#include "vao.h"
#include "vbo.h"
#include "ebo.h"
#include "shader.h"

#include <algorithm>
#include <array>
#include <vector>
#include <utility>

constexpr int ATLAS_COLUMNS = 6;
constexpr int ATLAS_ROWS = 5;

constexpr float CELL_WIDTH = 1.0f / ATLAS_COLUMNS;
constexpr float CELL_HEIGHT = 1.0f / ATLAS_ROWS;



class GuiManager {
	


	public:
		// function to make widnow responsive
		static void framebufferSizeCallback(GLFWwindow* window,
			int width,
			int height);

		//window set up 
		GLFWwindow* guiWindowSetUp();
		void guiWindowCleanUp(GLFWwindow* window);

		int VIEWPORT_W = 800;
		int VIEWPORT_H = 800;

		//error logs
		enum ERROR {
			UNKNOWN,
			WINDOW_FAILED_INIT,
			GLFW_FAILED_INIT
		};

		struct ErrorObj {
			ERROR error = UNKNOWN;
			const char* error_msg;
		};

		
		// vertex pipeline

		struct AtlasCell
		{
			int column;
			int row;
		};

		struct UVRegion
		{
			float uMin, vMin;
			float uMax, vMax;
		};

		struct textured_vertex {

			GLfloat x;
			GLfloat y;
			GLfloat z = 0.0f;

			GLfloat u;
			GLfloat v;
		};

		struct colored_vertex {

			GLfloat x;
			GLfloat y;
			GLfloat z = 0.0f;

			GLfloat r;
			GLfloat g;
			GLfloat b;
		};

		struct Color {
			GLfloat r;
			GLfloat g;
			GLfloat b;
		};

		struct pieceText {

			std::array<textured_vertex, 4> vertices;

		};

		struct squareText {

			std::array<colored_vertex, 4> vertices;

		};

		std::vector<colored_vertex> getVerticesForBoard();

		std::vector<GLuint> getBoardIndices();
		
		std::vector<textured_vertex>getVerticesForPieces(ClassicChess& game);

		std::vector<GLuint>getPieceIndices(std::size_t pieceCount);

		AtlasCell getAtlasCell(ClassicChess::PieceTypeBit piece);

		UVRegion getUVRegion(int column, int row);

		GuiManager() {

		};

		int guiMainLoop();

		//game logic
		int mouseToSquare(GLFWwindow* window);


		// actual drawing material
		
		private:
			VAO BoardVAO;
			VAO PieceVAO;

			VBO BoardVBO;
			EBO BoardEBO;

			VBO PieceVBO;
			EBO PieceEBO;

			Shader boardShader;
			Shader pieceShader;

			GLuint atlasTexture = 0;

			GLsizei boardIndexCount = 0;
			GLsizei pieceIndexCount = 0;

		public:
			void initializeBoard();
			void initializePieces(ClassicChess& game);
			void initializeShaders();
			void initializeTexture();

			void drawBoard();
			void drawPieces();

};
