#include "gui.h"
#include <iostream>
#include "shader.h"
#include "vao.h"
#include "vbo.h"
#include "ebo.h"
#include "../external/stb/stb_image.h"



//getting vertices
std::vector<GuiManager::colored_vertex>
GuiManager::getVerticesForBoard()
{
    std::vector<colored_vertex> vertices;
    vertices.reserve(64 * 4);

    constexpr float boardStart = -0.8f;
    constexpr float squareSize = 0.2f;

    const Color light{ 0.85f, 0.78f, 0.65f };
    const Color dark{ 0.35f, 0.18f, 0.10f };

    for (int row = 0; row < 8; row++)
    {
        for (int col = 0; col < 8; col++)
        {
            float x = boardStart + col * squareSize;
            float y = boardStart + row * squareSize;

            Color color =
                ((row + col) % 2 == 0)
                ? light
                : dark;

            vertices.insert(vertices.end(), {
                {x,              y,              0.0f, color.r, color.g, color.b},
                {x + squareSize, y,              0.0f, color.r, color.g, color.b},
                {x + squareSize, y + squareSize, 0.0f, color.r, color.g, color.b},
                {x,              y + squareSize, 0.0f, color.r, color.g, color.b}
                });
        }
    }

    return vertices;
}

GuiManager::AtlasCell GuiManager::getAtlasCell(
    ClassicChess::PieceTypeBit piece)
{
    bool isWhite =
        piece >= ClassicChess::W_PAWN &&
        piece <= ClassicChess::W_KING;

    int row = isWhite ? 0 : 1;
    int column = -1;

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
}

GuiManager::UVRegion GuiManager::getUVRegion(
    int column,
    int row)
{
    constexpr float cellWidth = 1.0f / 6.0f;
    constexpr float cellHeight = 1.0f / 2.0f;

    float uMin = column * cellWidth;
    float uMax = (column + 1) * cellWidth;

    // Atlas rows begin at the top, but OpenGL UV Y begins at the bottom.
    float vMax = 1.0f - row * cellHeight;
    float vMin = 1.0f - (row + 1) * cellHeight;

    return {
        uMin,
        vMin,
        uMax,
        vMax
    };
}


std::vector<GuiManager::textured_vertex>
GuiManager::getVerticesForPieces(ClassicChess& game)
{
    std::vector<textured_vertex> vertices;
    vertices.reserve(32 * 4);

    constexpr float boardStart = -0.8f;
    constexpr float squareSize = 0.2f;

    for (int square = 0; square < 64; square++)
    {
        ClassicChess::PieceTypeBit piece =
            game.piece_on_square(square);

        if (piece == ClassicChess::NO_PIECE)
            continue;

        int row = square / 8;
        int col = square % 8;

        float x = boardStart + col * squareSize;
        float y = boardStart + row * squareSize;

        AtlasCell cell = getAtlasCell(piece);
        UVRegion uv = getUVRegion(cell.column, cell.row);

        vertices.insert(vertices.end(), {
            // Bottom-left
            {x,              y,              0.0f, uv.uMin, uv.vMin},

            // Bottom-right
            {x + squareSize, y,              0.0f, uv.uMax, uv.vMin},

            // Top-right
            {x + squareSize, y + squareSize, 0.0f, uv.uMax, uv.vMax},

            // Top-left
            {x,              y + squareSize, 0.0f, uv.uMin, uv.vMax}
            });
    }

    return vertices;
}

std::vector<GLuint>
GuiManager::getPieceIndices(std::size_t pieceCount)
{
    std::vector<GLuint> indices;
    indices.reserve(pieceCount * 6);

    for (GLuint piece = 0;
        piece < static_cast<GLuint>(pieceCount);
        piece++)
    {
        GLuint base = piece * 4;

        indices.insert(indices.end(), {
            base, base + 1, base + 2,
            base, base + 2, base + 3
            });
    }

    return indices;
}

std::vector<GLuint> GuiManager::getBoardIndices()
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

//chess atlas texture set up
GLuint loadTexture(const char* path)
{
    int width = 0;
    int height = 0;
    int channels = 0;

    stbi_set_flip_vertically_on_load(true);

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
            << "Failed to load texture: "
            << path << '\n'
            << stbi_failure_reason() << '\n';

        return 0;
    }

    GLuint textureID = 0;

    glGenTextures(1, &textureID);
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

    stbi_image_free(data);

    glBindTexture(GL_TEXTURE_2D, 0);

    return textureID;
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
    glfwSetFramebufferSizeCallback(
        window,
        &GuiManager::framebufferSizeCallback
    );
    // use glad to load openGl functions
    gladLoadGL();
    //--------------------------------------------

    ClassicChess game;
    //board
    auto indicesBoard = getBoardIndices();
    auto verticesBoard = getVerticesForBoard();
    
    VAO vaoBoard;
    vaoBoard.Bind();

    VBO vboBoard(
        verticesBoard.data(),
        static_cast<GLsizeiptr>(
            verticesBoard.size() * sizeof(colored_vertex)
            )
    );

    EBO eboBoard(
        indicesBoard.data(),
        static_cast<GLsizeiptr>(
            indicesBoard.size() * sizeof(GLuint)
            )
    );
    
    vaoBoard.Bind();
    vaoBoard.LinkAttrib(
        vboBoard,
        0, // shader location
        3, // x, y, z
        GL_FLOAT,
        sizeof(colored_vertex),
        reinterpret_cast<void*>(
            offsetof(colored_vertex, x)
            )
    );

    vaoBoard.LinkAttrib(
        vboBoard,
        1, // shader location
        3, // r, g, b
        GL_FLOAT,
        sizeof(colored_vertex),
        reinterpret_cast<void*>(
            offsetof(colored_vertex, r)
            )
    );

    

    vaoBoard.Unbind();
    vboBoard.Unbind();
    eboBoard.Unbind();

    // pieces
    auto pieceVertices = getVerticesForPieces(game);

    auto pieceIndices =
        getPieceIndices(pieceVertices.size() / 4);

    VAO pieceVAO;
    pieceVAO.Bind();

    VBO pieceVBO(
        pieceVertices.data(),
        static_cast<GLsizeiptr>(
            pieceVertices.size() *
            sizeof(textured_vertex)
            )
    );

    EBO pieceEBO(
        pieceIndices.data(),
        static_cast<GLsizeiptr>(
            pieceIndices.size() *
            sizeof(GLuint)
            )
    );

    pieceVAO.LinkAttrib(
        pieceVBO,
        0,
        3,
        GL_FLOAT,
        sizeof(textured_vertex),
        reinterpret_cast<void*>(
            offsetof(textured_vertex, x)
            )
    );

    pieceVAO.LinkAttrib(
        pieceVBO,
        1,
        2,
        GL_FLOAT,
        sizeof(textured_vertex),
        reinterpret_cast<void*>(
            offsetof(textured_vertex, u)
            )
    );

    pieceVAO.Unbind();
    pieceVBO.Unbind();
    pieceEBO.Unbind();

    //load atlas
    GLuint atlasTexture = loadTexture(
        "resources/textures/chess_atlas.png"
    );

    if (atlasTexture == 0)
    {
        guiWindowCleanUp(window);
        return -1;
    }

    glEnable(GL_BLEND);

    glBlendFunc(
        GL_SRC_ALPHA,
        GL_ONE_MINUS_SRC_ALPHA
    );

    //setting up shaders
    try
    {
        Shader shaderProgram(
            "resources/shaders/default.vert",
            "resources/shaders/default.frag"
        );

        Shader pieceShader(
            "resources/shaders/piece.vert",
            "resources/shaders/piece.frag"
        );

        pieceShader.Activate();

        glUniform1i(
            glGetUniformLocation(
                pieceShader.ID,
                "chessAtlas"
            ),
            0
        );


        while (!glfwWindowShouldClose(window))
        {
            glClearColor(0.82f, 0.76f, 0.71f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            // 1. Draw the colored board first.
            shaderProgram.Activate();
            vaoBoard.Bind();

            glDrawElements(
                GL_TRIANGLES,
                static_cast<GLsizei>(indicesBoard.size()),
                GL_UNSIGNED_INT,
                nullptr
            );

            // 2. Draw transparent pieces over the board.
            pieceShader.Activate();

            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, atlasTexture);

            pieceVAO.Bind();

            glDrawElements(
                GL_TRIANGLES,
                static_cast<GLsizei>(pieceIndices.size()),
                GL_UNSIGNED_INT,
                nullptr
            );

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        vboBoard.Delete();
        vaoBoard.Delete();
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