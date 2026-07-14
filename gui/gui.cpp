#include "gui.h"
#include <iostream>
#include "shader.h"
#include "vao.h"
#include "vbo.h"
#include "ebo.h"
#include "../external/stb/stb_image.h"


//convert data into a square
GuiManager::Square GuiManager::makeSquare(
    float x,
    float y,
    float size,
    const UVRegion& uv)
{
    Square square{};

    square.vertices = { {
            // Position                    // Atlas coordinate
            {x,        y,        0.0f,     uv.uMin, uv.vMin}, // bottom-left
            {x + size, y,        0.0f,     uv.uMax, uv.vMin}, // bottom-right
            {x + size, y + size, 0.0f,     uv.uMax, uv.vMax}, // top-right
            {x,        y + size, 0.0f,     uv.uMin, uv.vMax}  // top-left
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

std::vector<GuiManager::RenderVertex>
GuiManager::getChessGameBoardVertices(ClassicChess& game)
{
    std::vector<RenderVertex> boardVertices;
    boardVertices.reserve(64 * 4);

    constexpr float boardStart = -0.8f;
    constexpr float squareSize = 0.2f;

    for (int row = 0; row < 8; row++)
    {
        for (int col = 0; col < 8; col++)
        {
            int squareNumber = row * 8 + col;

            bool darkSquare =
                ((row + col) % 2 != 0);

            ClassicChess::PieceTypeBit piece =
                game.piece_on_square(squareNumber);

            AtlasCell atlasCell =
                getAtlasCell(piece, darkSquare);

            UVRegion uv =
                getUVRegion(
                    atlasCell.column,
                    atlasCell.row
                );

            float x =
                boardStart + col * squareSize;

            float y =
                boardStart + row * squareSize;

            Square square =
                makeSquare(x, y, squareSize, uv);

            for (const Vertex& vertex : square.vertices)
            {
                boardVertices.push_back({
                    vertex.x,
                    vertex.y,
                    vertex.z,
                    vertex.u,
                    vertex.v
                    });
            }
        }
    }

    return boardVertices;
}

GLuint loadChessAtlas(const char* path)
{
    int width = 0;
    int height = 0;
    int channels = 0;

    stbi_set_flip_vertically_on_load(false);

    unsigned char* data = stbi_load(
        path,
        &width,
        &height,
        &channels,
        STBI_rgb_alpha
    );

    if (!data)
    {
        std::cerr
            << "Failed to load atlas: "
            << path << '\n'
            << stbi_failure_reason() << '\n';

        return 0;
    }

    std::cout
        << "Loaded atlas: "
        << width << " x " << height << '\n';

    GLuint textureID = 0;
    glGenTextures(1, &textureID);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_WRAP_S,
        GL_CLAMP_TO_EDGE
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_WRAP_T,
        GL_CLAMP_TO_EDGE
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MIN_FILTER,
        GL_LINEAR
    );

    glTexParameteri(
        GL_TEXTURE_2D,
        GL_TEXTURE_MAG_FILTER,
        GL_LINEAR
    );

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_RGBA8,
        width,
        height,
        0,
        GL_RGBA,
        GL_UNSIGNED_BYTE,
        data
    );

    // Do not generate mipmaps yet.
    stbi_image_free(data);

    glBindTexture(GL_TEXTURE_2D, 0);

    return textureID;
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

GuiManager::AtlasCell GuiManager::getAtlasCell(
    ClassicChess::PieceTypeBit piece,
    bool darkSquare)
{
    // Empty squares are in the final atlas row.
    if (piece == ClassicChess::NO_PIECE)
    {
        return {
            darkSquare ? 1 : 0,
            4
        };
    }

    bool whitePiece =
        piece >= ClassicChess::W_PAWN &&
        piece <= ClassicChess::W_KING;

    int row;

    if (whitePiece)
        row = darkSquare ? 1 : 0;
    else
        row = darkSquare ? 3 : 2;

    int column;

    switch (piece)
    {
    case ClassicChess::W_KING:
    case ClassicChess::B_KING:
        column = 0;
        break;

    case ClassicChess::W_QUEEN:
    case ClassicChess::B_QUEEN:
        column = 1;
        break;

    case ClassicChess::W_ROOK:
    case ClassicChess::B_ROOK:
        column = 2;
        break;

    case ClassicChess::W_BISHOP:
    case ClassicChess::B_BISHOP:
        column = 3;
        break;

    case ClassicChess::W_KNIGHT:
    case ClassicChess::B_KNIGHT:
        column = 4;
        break;

    case ClassicChess::W_PAWN:
    case ClassicChess::B_PAWN:
        column = 5;
        break;

    default:
        return { -1, -1 };
    }

    return { column, row };
};


GuiManager::UVRegion GuiManager::getUVRegion(
    int column,
    int row)
{
    constexpr float atlasWidth = 1374.0f;
    constexpr float atlasHeight = 1145.0f;

    constexpr float cellPixels = 229.0f;
    constexpr float paddingPixels = 2.0f;

    float left =
        column * cellPixels + paddingPixels;

    float right =
        (column + 1) * cellPixels - paddingPixels;

    // Image rows are numbered from the top.
    float top =
        row * cellPixels + paddingPixels;

    float bottom =
        (row + 1) * cellPixels - paddingPixels;

    float uMin = left / atlasWidth;
    float uMax = right / atlasWidth;

    // Convert image Y coordinates into OpenGL UV coordinates.
    float vMax = 1.0f - top / atlasHeight;
    float vMin = 1.0f - bottom / atlasHeight;

    return {
        uMin,
        vMin,
        uMax,
        vMax
    };
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

    
    GLuint atlasTexture = loadChessAtlas(
        "resources/textures/chess_atlas.png"
    );

    if (atlasTexture == 0)
    {
        guiWindowCleanUp(window);
        return -1;
    }

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
        2,
        GL_FLOAT,
        sizeof(RenderVertex),
        reinterpret_cast<void*>(offsetof(RenderVertex, u))
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
            glUniform1i(
                glGetUniformLocation(
                    shaderProgram.ID,
                    "chessAtlas"
                ),
                0
            );

            glClearColor(0.82f, 0.76f, 0.71f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, atlasTexture);

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