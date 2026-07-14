#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include "../game.h"
#include <algorithm>

class GuiManager {
	struct Color {

		GLfloat r, g, b;


	};

	Color alt{ 0.5f,0.5f,0.5f };

	struct Vertex {
		GLfloat x;
		GLfloat y;
		GLfloat z = 0.0f;

	};

	struct Square {

		Color color;

		std::array<Vertex, 4> vertices;


	};

	struct RenderVertex
	{
		GLfloat x, y, z;
		GLfloat r, g, b;
	};

	public:
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
			const Color& color);


		std::vector<RenderVertex> getChessGameBoardVertices(ClassicChess& game);
		std::vector<GLuint> getChessGameBoardIndices();
		static void framebufferSizeCallback(GLFWwindow* window,
			int width,
			int height);

		int guiMainLoop();
		GLFWwindow* guiWindowSetUp();
		void guiWindowCleanUp(GLFWwindow* window);
		int mouseToSquare(GLFWwindow* window);
};
