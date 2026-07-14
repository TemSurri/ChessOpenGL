#include "gui.h"
#include <iostream>
#include "shader.h"
#include "vao.h"
#include "vbo.h"
#include "ebo.h"

//convert data into a square
GuiManager::Square GuiManager::makeSquare(
    float x,
    float y,
    float size,
    const Color& color)
{
    Square square{};

    square.color = color;

    square.vertices = { {
        {x,        y,        0.0f}, // bottom-left
        {x + size, y,        0.0f}, // bottom-right
        {x + size, y + size, 0.0f}, // top-right
        {x,        y + size, 0.0f}  // top-left
    } };

    return square;
}

// little set up
GLFWwindow* GuiManager::guiWindowSetUp() {

    // init glfw
    if (!glfwInit()) {
        throw ErrorObj{ GLFW_FAILED_INIT , "glfw failed init"};
    }

    // give info to window creations
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    //create window
    GLFWwindow* window = glfwCreateWindow(800, 800, "Tem's Chess", NULL, NULL);
    if (window == NULL) {
        
        ErrorObj error{ WINDOW_FAILED_INIT, "glfw window failed to create" };
        throw error;

    }

    return window;

}

//window clean up
void GuiManager::guiWindowCleanUp(GLFWwindow* window){

    if (window != nullptr) {
        glfwDestroyWindow(window);
    }
    glfwTerminate();

}

//change the size of the viewport window automatically
void GuiManager::framebufferSizeCallback(
    GLFWwindow* window,
    int width,
    int height)
{
    // Choose the smaller dimension so the viewport remains square.
    int viewportSize = std::min(width, height);

    // Center the square viewport.
    int viewportX = (width - viewportSize) / 2;
    int viewportY = (height - viewportSize) / 2;

    glViewport(
        viewportX,
        viewportY,
        viewportSize,
        viewportSize
    );
}

// used to convert mouse hit coord to sqaure
int GuiManager::mouseToSquare(GLFWwindow* window)
{
    double mouseX;
    double mouseY;
    glfwGetCursorPos(window, &mouseX, &mouseY);

    int width;
    int height;
    glfwGetWindowSize(window, &width, &height);

    // Same calculation used by the responsive viewport.
    float viewportSize =
        static_cast<float>(std::min(width, height));

    float viewportX = (width - viewportSize) / 2.0f;
    float viewportY = (height - viewportSize) / 2.0f;

    // Board occupies -0.8 to 0.8, or 80% of viewport.
    float boardSize = viewportSize * 0.8f;

    // Remaining 20% gives a 10% margin on each side.
    float boardLeft = viewportX + viewportSize * 0.1f;
    float boardTop = viewportY + viewportSize * 0.1f;

    float squareSize = boardSize / 8.0f;

    // Check whether click is inside the board.
    if (mouseX < boardLeft ||
        mouseX >= boardLeft + boardSize ||
        mouseY < boardTop ||
        mouseY >= boardTop + boardSize)
    {
        return -1;
    }

    int column = static_cast<int>(
        (mouseX - boardLeft) / squareSize
        );

    // GLFW mouse Y begins at the top.
    int rowFromTop = static_cast<int>(
        (mouseY - boardTop) / squareSize
        );

    // Bitboard row 0 is usually at the bottom.
    int row = 7 - rowFromTop;

    return row * 8 + column;
}

std::vector<GuiManager::RenderVertex> GuiManager::getChessGameBoardVertices(ClassicChess& game) {
    

    std::vector<RenderVertex> boardVertices;
    boardVertices.reserve(64 * 4);

    constexpr float boardStart = -0.8f;
    constexpr float squareSize = 0.2f;

    const Color light{ 0.85f, 0.85f, 0.85f };
    const Color dark{ 0.25f, 0.25f, 0.25f };

    for (int row = 0; row < 8; row++)
    {
        for (int col = 0; col < 8; col++)
        {
            int squareNumber = row * 8 + col;

            Color squareColor =
                ((row + col) % 2 == 0) ? light : dark;

            float x = boardStart + col * squareSize;
            float y = boardStart + row * squareSize;

            Square square = makeSquare(
                x,
                y,
                squareSize,
                squareColor
            );

            for (const Vertex& vertex : square.vertices)
            {
                boardVertices.push_back({
                    vertex.x,
                    vertex.y,
                    vertex.z,
                    square.color.r,
                    square.color.g,
                    square.color.b
                    });
            }

            ClassicChess::PieceTypeBit type =
                game.piece_on_square(squareNumber);

            // Use type later when generating piece texture data.
        }
    }

    return boardVertices;


}

std::vector<GLuint> GuiManager::getChessGameBoardIndices()
{
    std::vector<GLuint> indices;
    indices.reserve(64 * 6);

    for (GLuint square = 0; square < 64; square++)
    {
        GLuint base = square * 4;

        indices.insert(indices.end(), {
            base, base + 1, base + 2,
            base, base + 2, base + 3
            });
    }

    return indices;
}

int GuiManager::guiMainLoop()
{
    GLFWwindow* window = nullptr;

    // window pbject should be successfuly pointed to by window 
    try {
        window = guiWindowSetUp();
    }
    catch (const ErrorObj& error) {

        guiWindowCleanUp(window);
        std::cout << error.error_msg << std::endl;
        return -1;
    }

    // make openGl context connected to our window
    glfwMakeContextCurrent(window);

    // use glad to load openGl functions
    gladLoadGL();
    glfwSetFramebufferSizeCallback(
        window,
        &GuiManager::framebufferSizeCallback
    );

    int framebufferWidth;
    int framebufferHeight;

    glfwGetFramebufferSize(
        window,
        &framebufferWidth,
        &framebufferHeight
    );

    framebufferSizeCallback(
        window,
        framebufferWidth,
        framebufferHeight
    );

    glClearColor(0.42f, 0.13f, 0.11f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    glfwSwapBuffers(window);
    //--------------------------------------------

    ClassicChess game;

    auto vertices = getChessGameBoardVertices(game);
    auto indices = getChessGameBoardIndices();

    VAO boardVAO;
    boardVAO.Bind();

    VBO boardVBO(
        vertices.data(),
        static_cast<GLsizeiptr>(
            vertices.size() * sizeof(RenderVertex)
            )
    );

    EBO boardEBO(
        indices.data(),
        static_cast<GLsizeiptr>(
            indices.size() * sizeof(GLuint)
            )
    );

    boardVAO.LinkAttrib(
        boardVBO,
        0,
        3,
        GL_FLOAT,
        sizeof(RenderVertex),
        reinterpret_cast<void*>(offsetof(RenderVertex, x))
    );

    boardVAO.LinkAttrib(
        boardVBO,
        1,
        3,
        GL_FLOAT,
        sizeof(RenderVertex),
        reinterpret_cast<void*>(offsetof(RenderVertex, r))
    );

    

    //setting up shaders
    try
    {
        Shader shaderProgram(
            "resources/shaders/default.vert",
            "resources/shaders/default.frag"
        );

        shaderProgram.Activate();


        while (!glfwWindowShouldClose(window)) {

            
            shaderProgram.Activate();
            glClearColor(0.82f, 0.76f, 0.71f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            boardVAO.Bind();
            glDrawElements(GL_TRIANGLES, static_cast<GLsizei>(indices.size()), GL_UNSIGNED_INT, 0);


            glfwSwapBuffers(window);
            if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
            {
                int square = mouseToSquare(window);

                if (square != -1)
                {
                    std::cout << "Clicked square: " << square << '\n';
                }
            }

            glfwPollEvents();



        }

        shaderProgram.Delete();






    }




    catch (const Shader::ErrorObj& error)
    {
        std::cerr << "Shader error: " << error.error_msg << '\n';
        guiWindowCleanUp(window);
        return -1;
    }
    //clean up window
    guiWindowCleanUp(window);
    return 0;
}