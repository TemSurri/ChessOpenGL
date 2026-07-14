#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../game.h"
#include <algorithm>
#include <array>
#include <vector>

constexpr int ATLAS_COLUMNS = 6;
constexpr int ATLAS_ROWS = 5;

constexpr float CELL_WIDTH = 1.0f / ATLAS_COLUMNS;
constexpr float CELL_HEIGHT = 1.0f / ATLAS_ROWS;



class GuiManager {
	


	public:
		struct UVRegion
		{
			float uMin;
			float vMin;
			float uMax;
			float vMax;
		};

		struct AtlasCell
		{
			int column;
			int row;
		};

		struct Vertex
		{
			GLfloat x, y, z;
			GLfloat u, v;
		};

		struct Square
		{
			std::array<Vertex, 4> vertices;
		};

		struct RenderVertex
		{
			GLfloat x, y, z;
			GLfloat u, v;
		};







		int VIEWPORT_W = 800;
		int VIEWPORT_H = 800;

		enum ERROR {
			UNKNOWN,
			WINDOW_FAILED_INIT,
			GLFW_FAILED_INIT
		};

		struct ErrorObj {
			ERROR error = UNKNOWN;
			const char* error_msg;
		};

		GuiManager() {

		};
		Square makeSquare(
			float x,
			float y,
			float size,
			const UVRegion& uv);

		UVRegion getUVRegion(
			int column,
			int row);
		
		std::vector<RenderVertex> getChessGameBoardVertices(ClassicChess& game);
		std::vector<GLuint> getChessGameBoardIndices();
		static void framebufferSizeCallback(GLFWwindow* window,
			int width,
			int height);

		int guiMainLoop();
		GLFWwindow* guiWindowSetUp();
		void guiWindowCleanUp(GLFWwindow* window);
		int mouseToSquare(GLFWwindow* window);
		AtlasCell getAtlasCell(
			ClassicChess::PieceTypeBit piece,
			bool darkSquare);


};
