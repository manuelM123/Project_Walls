#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader_m.h>
#include <camera.h>
#include <iostream>
#include <vector>

#include <iostream>
#include <map>
#include <string>
#include <ft2build.h>
#include FT_FREETYPE_H

//nota => ver camera position com cube position

// New file
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void goMaze(unsigned int cubeVAO, Shader Light, glm::mat4 model, unsigned int floorVAO, Shader skyShader, unsigned int pointVAO);
void draw(void);
void drawHUD(Shader& shader);
void transferDataToGPUMemory(void);
GLuint LoadShaders(const char* vertex_file_path, const char* fragment_file_path);
GLuint programID;
GLuint VertexArrayID;
GLuint vertexbuffer;
GLuint colorbuffer;
void RenderText(Shader& shader, std::string text, float x, float y, float scale, glm::vec3 color);
unsigned int loadTexture(char const* path);
glm::mat4 resetModel(glm::mat4 model);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;


// Holds all state information relevant to a character as loaded using FreeType
struct Character {
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2   Size;      // Size of glyph
    glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Horizontal offset to advance to next glyph
};

std::map<GLchar, Character> Characters;
unsigned int VAOtext, VBOtext;


// camera
//Camera X coordinates: 66.4122Camera Y coordinates: 1.85396Camera Z coordinates: 1.10673
Camera camera(glm::vec3(0.0f, 0.0f, 2.0f));

float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;
bool cameraUnlocked = false;
bool exitMaze = false;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(0.0f, 3.0f, 0.0f);
//glm::vec3 lightPos(1.0f, 0.0f, 0.0f);

//variables for colision
float xstep = 0.01f;
float ystep = 0.01f;
float zstep = 0.01f;
glm::vec3 PositionBefore = camera.Position;

int POINTS = 0;
float XExitV2 = 0.00f;

//variavel controlo
int cont = 0;

int dArray[16] = { 0.0 };
std::vector <int> arr;
std::vector <float> arrLimits;
std::vector <float> arrLimits2;
std::vector <float> alreadyThere;

//texturas
unsigned int texture1;
unsigned int texture2;
unsigned int texture3;
unsigned int texture4;

bool showPoint = true;


void drawCubes(unsigned int cubeVAO) {
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void drawFloor(unsigned int floorVAO) {
    glBindVertexArray(floorVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void drawSky(unsigned int skyVAO) {
    glBindVertexArray(skyVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void drawPoints(unsigned int pointVAO, bool showPoint) {
    if (showPoint == true) {
        glBindVertexArray(pointVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}

void drawMenu(unsigned int menuVAO) {
    glBindVertexArray(menuVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

float ang = 0.01;
float yMax = 0.00;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();

    //to prevent stutters
    glfwWindowHint(GLFW_SAMPLES, 4);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Project Walls", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }



    // ----------------------------
    //           Freetype
    // ----------------------------



    // Compile and setup the shader
    // ----------------------------
    Shader shader("C:\\Users\\Legion\\Desktop\\Storage\\Uni\\3ano\\CG\\Walls\\Project_Walls\\Project_Walls\\Project\\Project\\Source\\text.vs", "C:\\Users\\Legion\\Desktop\\Storage\\Uni\\3ano\\CG\\Walls\\Project_Walls\\Project_Walls\\Project\\Project\\Source\\text.fs");
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(SCR_WIDTH), 0.0f, static_cast<float>(SCR_HEIGHT));
    shader.use();
    glUniformMatrix4fv(glGetUniformLocation(shader.ID, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // FreeType
    // --------
    FT_Library ft;
    if (FT_Init_FreeType(&ft))
    {
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;
        return -1;
    }

    // Path to font
    std::string font_name = "C:\\Users\\Legion\\Desktop\\Storage\\Uni\\3ano\\CG\\Walls\\Project_Walls\\Project_Walls\\Project\\Project\\Source\\CourierNew.ttf";
    if (font_name.empty())
    {
        std::cout << "ERROR::FREETYPE: Failed to load font_name" << std::endl;
        return -1;
    }

    FT_Face face;
    if (FT_New_Face(ft, font_name.c_str(), 0, &face)) {
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;
        return -1;
    }
    else {
        // set size to load glyphs as
        FT_Set_Pixel_Sizes(face, 0, 48);

        // disable byte-alignment restriction
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

        // load first 128 characters of ASCII set
        for (unsigned char c = 0; c < 128; c++)
        {
            // Load character glyph
            if (FT_Load_Char(face, c, FT_LOAD_RENDER))
            {
                std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
                continue;
            }
            // generate texture
            unsigned int texture;
            glGenTextures(1, &texture);
            glBindTexture(GL_TEXTURE_2D, texture);
            glTexImage2D(
                GL_TEXTURE_2D,
                0,
                GL_RED,
                face->glyph->bitmap.width,
                face->glyph->bitmap.rows,
                0,
                GL_RED,
                GL_UNSIGNED_BYTE,
                face->glyph->bitmap.buffer
            );
            // set texture options
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            // Now store character for later use
            Character character = {
                texture,
                glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
                glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
                static_cast<unsigned int>(face->glyph->advance.x)
            };
            Characters.insert(std::pair<char, Character>(c, character));
        }
        glBindTexture(GL_TEXTURE_2D, 0);
    }
    // destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);

    // Configure VAO/VBO for texture quads
    // -----------------------------------
    glGenVertexArrays(1, &VAOtext);
    glGenBuffers(1, &VBOtext);
    glBindVertexArray(VAOtext);
    glBindBuffer(GL_ARRAY_BUFFER, VBOtext);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);



    // ----------------------------------
    // End of Freetype
    // ----------------------------------



    // configure global opengl state
    // -----------------------------

    glEnable(GL_DEPTH_TEST);

    // build and compile our shader zprogram
    // ------------------------------------

    // C:\\Users\\Legion\\Desktop\\Storage\\Uni\\3ano\\CG\\Walls\\Project_Walls\\Project\\Project\\Source
    // C:\\Users\\manue\\Desktop\\Project_Walls\\Project\\Project\\Source
    Shader lightingShader("C:\\Users\\Legion\\Desktop\\Storage\\Uni\\3ano\\CG\\Walls\\Project_Walls\\Project_Walls\\Project\\Project\\Source\\2.1.basic_lighting.vs", "C:\\Users\\Legion\\Desktop\\Storage\\Uni\\3ano\\CG\\Walls\\Project_Walls\\Project_Walls\\Project\\Project\\Source\\2.1.basic_lighting.fs");
    Shader lampShader("C:\\Users\\Legion\\Desktop\\Storage\\Uni\\3ano\\CG\\Walls\\Project_Walls\\Project_Walls\\Project\\Project\\Source\\2.1.lamp.vs", "C:\\Users\\Legion\\Desktop\\Storage\\Uni\\3ano\\CG\\Walls\\Project_Walls\\Project_Walls\\Project\\Project\\Source\\2.1.lamp.fs");
    Shader skyShader("C:\\Users\\Legion\\Desktop\\Storage\\Uni\\3ano\\CG\\Walls\\Project_Walls\\Project_Walls\\Project\\Project\\Source\\VS.vs", "C:\\Users\\Legion\\Desktop\\Storage\\Uni\\3ano\\CG\\Walls\\Project_Walls\\Project_Walls\\Project\\Project\\Source\\FS.fs");


    //------------------------------
    camera.Position.x = 65.691f;
    camera.Position.y = 1.85396f;
    camera.Position.z = -0.159782f;

    camera.Front.x = -0.999985f;
    camera.Front.y = 0.00174533f;
    camera.Front.z = 0.00523764f;

    camera.Up.x = 0.0017453f;
    camera.Up.y = 0.999998f;
    camera.Up.z = -9.1414e-06f;
    //------------------------------


    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {

        //lados restantes     //normals            //texture coords
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   1.0f, 0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   1.0f, 1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,   1.0f, 0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,   1.0f, 1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,   1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,

        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,   0.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,   1.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,   1.0f,  1.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,   1.0f,  1.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,   0.0f,  1.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,   0.0f,  0.0f,

        //floor of the labirint //normals     //texture coords
        -0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,     0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,  1.0f,  1.0f,

        0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,  0.0f,  0.0f,

        //topo                //normals            //texture coords
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,   1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,   1.0f,  1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,   1.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f
    };

    float verticesLamp[] = {

        //lados restantes
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        //chão
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        //topo 
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };

    float floor[] = {
        -0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,  0.0f, 0.0f, //bottom left
        0.5f, -0.5f, -0.5f,     0.0f, -1.0f,  0.0f,  1.0f, 0.0f, //bottom right
        0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,  1.0f, 1.0f, //top right

        0.5f, -0.5f,  0.5f,     0.0f, -1.0f,  0.0f,  1.0f, 1.0f, //top right
        -0.5f, -0.5f,  0.5f,    0.0f, -1.0f,  0.0f,  0.0f, 1.0f, //top left
        -0.5f, -0.5f, -0.5f,    0.0f, -1.0f,  0.0f,  0.0f, 0.0f, //bottom left
    };

    float skyVertices[] = {
        //lados restantes     //normals            //texture coords
        -20.0f, -20.0f, -20.0f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
        20.0f, -20.0f, -20.0f,  0.0f,  0.0f, -1.0f,   1.0f, 0.0f,
        20.0f,  20.0f, -20.0f,  0.0f,  0.0f, -1.0f,   1.0f, 1.0f,
        20.0f,  20.0f, -20.0f,  0.0f,  0.0f, -1.0f,   1.0f, 1.0f,
        -20.0f,  20.0f, -20.0f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -20.0f, -20.0f, -20.0f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -20.0f, -20.0f,  20.0f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
        20.0f, -20.0f,  20.0f,  0.0f,  0.0f,  1.0f,   1.0f, 0.0f,
        20.0f,  20.0f,  20.0f,  0.0f,  0.0f,  1.0f,   1.0f, 1.0f,
        20.0f,  20.0f,  20.0f,  0.0f,  0.0f,  1.0f,   1.0f, 1.0f,
        -20.0f,  20.0f,  20.0f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
        -20.0f, -20.0f,  20.0f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,

        -20.0f,  20.0f,  20.0f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -20.0f,  20.0f, -20.0f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -20.0f, -20.0f, -20.0f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -20.0f, -20.0f, -20.0f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -20.0f, -20.0f,  20.0f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -20.0f,  20.0f,  20.0f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,

        20.0f,  20.0f,  20.0f,  1.0f,  0.0f,  0.0f,   0.0f,  0.0f,
        20.0f,  20.0f, -20.0f,  1.0f,  0.0f,  0.0f,   1.0f,  0.0f,
        20.0f, -20.0f, -20.0f,  1.0f,  0.0f,  0.0f,   1.0f,  1.0f,
        20.0f, -20.0f, -20.0f,  1.0f,  0.0f,  0.0f,   1.0f,  1.0f,
        20.0f, -20.0f,  20.0f,  1.0f,  0.0f,  0.0f,   0.0f,  1.0f,
        20.0f,  20.0f,  20.0f,  1.0f,  0.0f,  0.0f,   0.0f,  0.0f,

        //floor of the sky         //normals     //texture coords
        -20.0f, -20.0f, -20.0f,    0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        20.0f, -20.0f, -20.0f,     0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        20.0f, -20.0f,  20.0f,     0.0f, -1.0f,  0.0f,  1.0f,  1.0f,

        20.0f, -20.0f,  20.0f,     0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
        -20.0f, -20.0f,  20.0f,    0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
        -20.0f, -20.0f, -20.0f,    0.0f, -1.0f,  0.0f,  0.0f,  0.0f,

        //topo                   //normals            //texture coords
        -20.0f,  20.0f, -20.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        20.0f,  20.0f, -20.0f,  0.0f,  1.0f,  0.0f,   1.0f,  0.0f,
        20.0f,  20.0f,  20.0f,  0.0f,  1.0f,  0.0f,   1.0f,  1.0f,
        20.0f,  20.0f,  20.0f,  0.0f,  1.0f,  0.0f,   1.0f,  1.0f,
        -20.0f,  20.0f,  20.0f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
        -20.0f,  20.0f, -20.0f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f
    };

    float menu[] = {
        30.0f,  20.0f,  20.0f,  -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        30.0f,  20.0f, -20.0f,  -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        30.0f, -20.0f, -20.0f,  -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        30.0f, -20.0f, -20.0f,  -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        30.0f, -20.0f,  20.0f,  -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        30.0f,  20.0f,  20.0f,  -1.0f,  0.0f,  0.0f,  0.0f,  0.0f
    };

    float pointsVertices[] = {

        //lados restantes     //normals            //texture coords
        -0.1f, -0.1f, -0.1f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
        0.1f, -0.1f, -0.1f,  0.0f,  0.0f, -1.0f,   1.0f, 0.0f,
        0.1f,  0.1f, -0.1f,  0.0f,  0.0f, -1.0f,   1.0f, 1.0f,
        0.1f,  0.1f, -0.1f,  0.0f,  0.0f, -1.0f,   1.0f, 1.0f,
        -0.1f,  0.1f, -0.1f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
        -0.1f, -0.1f, -0.1f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

        -0.1f, -0.1f,  0.1f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,
        0.1f, -0.1f,  0.1f,  0.0f,  0.0f,  1.0f,   1.0f, 0.0f,
        0.1f,  0.1f,  0.1f,  0.0f,  0.0f,  1.0f,   1.0f, 1.0f,
        0.1f,  0.1f,  0.1f,  0.0f,  0.0f,  1.0f,   1.0f, 1.0f,
        -0.1f,  0.1f,  0.1f,  0.0f,  0.0f,  1.0f,  0.0f, 1.0f,
        -0.1f, -0.1f,  0.1f,  0.0f,  0.0f,  1.0f,  0.0f, 0.0f,

        -0.1f,  0.1f,  0.1f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,
        -0.1f,  0.1f, -0.1f, -1.0f,  0.0f,  0.0f,  1.0f,  0.0f,
        -0.1f, -0.1f, -0.1f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.1f, -0.1f, -0.1f, -1.0f,  0.0f,  0.0f,  1.0f,  1.0f,
        -0.1f, -0.1f,  0.1f, -1.0f,  0.0f,  0.0f,  0.0f,  1.0f,
        -0.1f,  0.1f,  0.1f, -1.0f,  0.0f,  0.0f,  0.0f,  0.0f,

        0.1f,  0.1f,  0.1f,  1.0f,  0.0f,  0.0f,   0.0f,  0.0f,
        0.1f,  0.1f, -0.1f,  1.0f,  0.0f,  0.0f,   1.0f,  0.0f,
        0.1f, -0.1f, -0.1f,  1.0f,  0.0f,  0.0f,   1.0f,  1.0f,
        0.1f, -0.1f, -0.1f,  1.0f,  0.0f,  0.0f,   1.0f,  1.0f,
        0.1f, -0.1f,  0.1f,  1.0f,  0.0f,  0.0f,   0.0f,  1.0f,
        0.1f,  0.1f,  0.1f,  1.0f,  0.0f,  0.0f,   0.0f,  0.0f,

        //floor of the labirint //normals     //texture coords
        -0.1f, -0.1f, -0.1f,    0.0f, -1.0f,  0.0f,  0.0f,  0.0f,
        0.1f, -0.1f, -0.1f,     0.0f, -1.0f,  0.0f,  1.0f,  0.0f,
        0.1f, -0.1f,  0.1f,     0.0f, -1.0f,  0.0f,  1.0f,  1.0f,

        0.1f, -0.1f,  0.1f,     0.0f, -1.0f,  0.0f,  1.0f,  1.0f,
        -0.1f, -0.1f,  0.1f,    0.0f, -1.0f,  0.0f,  0.0f,  1.0f,
        -0.1f, -0.1f, -0.1f,    0.0f, -1.0f,  0.0f,  0.0f,  0.0f,

        //topo                //normals            //texture coords
        -0.1f,  0.1f, -0.1f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f,
        0.1f,  0.1f,  -0.1f,  0.0f,  1.0f,  0.0f,   1.0f,  0.0f,
        0.1f,  0.1f,   0.1f,  0.0f,  1.0f,  0.0f,   1.0f,  1.0f,
        0.1f,  0.1f,   0.1f,  0.0f,  1.0f,  0.0f,   1.0f,  1.0f,
        -0.1f,  0.1f,  0.1f,  0.0f,  1.0f,  0.0f,  0.0f,  1.0f,
        -0.1f,  0.1f, -0.1f,  0.0f,  1.0f,  0.0f,  0.0f,  0.0f
    };

    // first, configure the cube's VAO (and VBO)
    unsigned int VBO, cubeVAO, VAO;
    //glGenVertexArrays(1, &cubeVAO);
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); //2-coluna onde começa / 2 - tamanho / 9N elementos na linha / 6 tamanho até inicio das normais
    glEnableVertexAttribArray(2);

    // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    unsigned int lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glGenBuffers(1, &VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VAO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verticesLamp), verticesLamp, GL_STATIC_DRAW);

    glBindVertexArray(lightVAO);

    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // third, configure the floor's VAO (and VBO)
    unsigned int VBO1, floorVAO;
    glGenVertexArrays(1, &floorVAO);
    glGenBuffers(1, &VBO1);

    glBindBuffer(GL_ARRAY_BUFFER, VBO1);
    glBufferData(GL_ARRAY_BUFFER, sizeof(floor), floor, GL_STATIC_DRAW);

    glBindVertexArray(floorVAO);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); //2-coluna onde começa / 2 - tamanho / 9N elementos na linha / 6 tamanho até inicio das normais
    glEnableVertexAttribArray(2);

    // forth, configure the sky's VAO (and VBO)
    unsigned int VBO2, skyVAO;
    glGenVertexArrays(1, &skyVAO);
    glGenBuffers(1, &VBO2);

    glBindBuffer(GL_ARRAY_BUFFER, VBO2);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyVertices), skyVertices, GL_STATIC_DRAW);

    glBindVertexArray(skyVAO);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); //2-coluna onde começa / 2 - tamanho / 9N elementos na linha / 6 tamanho até inicio das normais
    glEnableVertexAttribArray(2);

    // fifth, configure the point's VAO (and VBO)
    unsigned int VBO3, pointVAO;
    glGenVertexArrays(1, &pointVAO);
    glGenBuffers(1, &VBO3);

    glBindBuffer(GL_ARRAY_BUFFER, VBO3);
    glBufferData(GL_ARRAY_BUFFER, sizeof(pointsVertices), pointsVertices, GL_STATIC_DRAW);

    glBindVertexArray(pointVAO);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); //2-coluna onde começa / 2 - tamanho / 9N elementos na linha / 6 tamanho até inicio das normais
    glEnableVertexAttribArray(2);

    // sixth, configure the menu's VAO (and VBO)
    unsigned int VBO4, menuVAO;
    glGenVertexArrays(1, &menuVAO);
    glGenBuffers(1, &VBO4);

    glBindBuffer(GL_ARRAY_BUFFER, VBO4);
    glBufferData(GL_ARRAY_BUFFER, sizeof(menu), menu, GL_STATIC_DRAW);

    glBindVertexArray(menuVAO);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); //2-coluna onde começa / 2 - tamanho / 9N elementos na linha / 6 tamanho até inicio das normais
    glEnableVertexAttribArray(2);

    //unsigned int texture1 = loadTexture("C:\\Users\\Legion\\Desktop\\Storage\\Uni\\3ano\\CG\\Walls\\Project_Walls\\Project\\Project\\Source\\awesomeface.png");
    texture1 = loadTexture("C:\\Users\\Legion\\Desktop\\Storage\\Uni\\3ano\\CG\\Walls\\Project_Walls\\Project_Walls\\Project\\Project\\Source\\wall5.jpeg");
    texture2 = loadTexture("C:\\Users\\Legion\\Desktop\\Storage\\Uni\\3ano\\CG\\Walls\\Project_Walls\\Project_Walls\\Project\\Project\\Source\\sky.png");
    texture3 = loadTexture("C:\\Users\\Legion\\Desktop\\Storage\\Uni\\3ano\\CG\\Walls\\Project_Walls\\Project_Walls\\Project\\Project\\Source\\floor.png");
    texture4 = loadTexture("C:\\Users\\Legion\\Desktop\\Storage\\Uni\\3ano\\CG\\Walls\\Project_Walls\\Project_Walls\\Project\\Project\\Source\\start.png");
    //---------------------------------------------------------------------------------------------------

    //lightingShader.use();
    //glUniform1i(glGetUniformLocation(lightingShader.ID, "texture1"), 0);
    //glUniform1i(glGetUniformLocation(skyShader.ID, "texture2"), 1);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        for (int i = 0; i < 2; i++)
        {
            if (i == 0)
            {
                glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);

                // per-frame time logic
                // --------------------
                float currentFrame = glfwGetTime();
                deltaTime = currentFrame - lastFrame;
                lastFrame = currentFrame;

                // input
                // -----
                processInput(window);

                // render
                // ------
                glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
                glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

                // be sure to activate shader when setting uniforms/drawing objects
                lightingShader.use();
                lightingShader.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
                lightingShader.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
                lightingShader.setVec3("lightPos", lightPos);
                lightingShader.setVec3("viewPos", camera.Position);

                // view/projection transformations
                glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
                glm::mat4 view = camera.GetViewMatrix();
                lightingShader.setMat4("projection", projection);
                lightingShader.setMat4("view", view);

                // world transformation
                glm::mat4 model = glm::mat4(1.0f);

                //atualização da matrix model
                lightingShader.setMat4("model", model);

                //upload texture of shaders
                //glActiveTexture(GL_TEXTURE0);
                //glBindTexture(GL_TEXTURE_2D, texture1);

                //drawing maze
                goMaze(cubeVAO, lightingShader, model, floorVAO, skyShader, pointVAO);

                if (camera.Position.x > XExitV2 && XExitV2 != 0.00f && cameraUnlocked == true) {
                    camera.Position.x = 65.691f;
                    camera.Position.y = 1.85396f;
                    camera.Position.z = -0.159782f;

                    camera.Front.x = -0.999985f;
                    camera.Front.y = 0.00174533f;
                    camera.Front.z = 0.00523764f;

                    camera.Up.x = 0.0017453f;
                    camera.Up.y = 0.999998f;
                    camera.Up.z = -9.1414e-06f;

                    texture4 = loadTexture("C:\\Users\\Legion\\Desktop\\Storage\\Uni\\3ano\\CG\\Walls\\Project_Walls\\Project_Walls\\Project\\Project\\Source\\exit.png");
                    cameraUnlocked = false;
                    exitMaze = true;
                }

                //activate sky shader
                skyShader.use();
                skyShader.setMat4("projection", projection);
                skyShader.setMat4("view", view);
                skyShader.setMat4("model", model);

                //upload texture of shaders
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture4);

                // render the menu
                drawMenu(menuVAO);

                //upload texture of shaders
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture2);

                // render the sky
                drawSky(skyVAO);

                // also draw the lamp object
                lampShader.use();
                lampShader.setMat4("projection", projection);
                lampShader.setMat4("view", view);
                model = glm::mat4(1.0f);
                model = glm::translate(model, lightPos);
                model = glm::scale(model, glm::vec3(0.2f)); // a smaller cube
                lampShader.setMat4("model", model);

                glBindVertexArray(lightVAO);
                glDrawArrays(GL_TRIANGLES, 0, 36);
                //glfwSwapBuffers(window);
            }
            else
            {
                if(cameraUnlocked == true)
                drawHUD(shader);
                //glfwSwapBuffers(window);
            }
        }

        glfwSwapBuffers(window);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    //glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);

    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

void drawHUD(Shader& shader)
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_DEPTH_TEST);

    glViewport(0, SCR_HEIGHT * 0.875, SCR_WIDTH * 0.25, SCR_HEIGHT * 0.125);
    RenderText(shader, "Points: " + std::to_string(POINTS), 0.0f, 0.0f, 3.0f, glm::vec3(1.0f, 0.8f, 0.2f));
    glEnable(GL_DEPTH_TEST);
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    //if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && yMax >= 105) ==> limite eixo y a usar quando jogador estiver bem posicionado

    if (cameraUnlocked == true) {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.ProcessKeyboard(FORWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.ProcessKeyboard(BACKWARD, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.ProcessKeyboard(LEFT, deltaTime);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.ProcessKeyboard(RIGHT, deltaTime);

        if (camera.Position.y > 0.0f)
            camera.Position.y = 0.0f;

        if (camera.Position.y < 0.0f)
            camera.Position.y = 0.0f;

        if (camera.Position.x < -0.50f)
            camera.Position.x = -0.50f;
    }

    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
        if (exitMaze == false) {
            camera = glm::vec3(0.0f, 0.0f, 2.0f);
            cameraUnlocked = true;
        }
    }

    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        texture1 = loadTexture("C:\\Users\\Legion\\Desktop\\Storage\\Uni\\3ano\\CG\\Walls\\Project_Walls\\Project_Walls\\Project\\Project\\Source\\wall2.jpg");
        texture3 = loadTexture("C:\\Users\\Legion\\Desktop\\Storage\\Uni\\3ano\\CG\\Walls\\Project_Walls\\Project_Walls\\Project\\Project\\Source\\erva.jpg");

    }

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        texture1 = loadTexture("C:\\Users\\Legion\\Desktop\\Storage\\Uni\\3ano\\CG\\Walls\\Project_Walls\\Project_Walls\\Project\\Project\\Source\\wall5.jpeg");
        texture3 = loadTexture("C:\\Users\\Legion\\Desktop\\Storage\\Uni\\3ano\\CG\\Walls\\Project_Walls\\Project_Walls\\Project\\Project\\Source\\floor.png");
    }

    //teste => para usar depois em colisões
    if (glfwGetKey(window, GLFW_KEY_END) == GLFW_PRESS) {
        std::cout << "Camera X coordinates: " << camera.Position.x;
        std::cout << "Camera Y coordinates: " << camera.Position.y;
        std::cout << "Camera Z coordinates: " << camera.Position.z << "\n\n";

        std::cout << "Camera X coordinates: " << camera.Front.x;
        std::cout << "Camera Y coordinates: " << camera.Front.y;
        std::cout << "Camera Z coordinates: " << camera.Front.z << "\n\n";

        std::cout << "Camera X coordinates: " << camera.Up.x;
        std::cout << "Camera Y coordinates: " << camera.Up.y;
        std::cout << "Camera Z coordinates: " << camera.Up.z << "\n\n";
    }

    if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS) {
        cont = 0;
    }
}

// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    yMax = ypos;

    lastX = xpos;
    lastY = ypos;

    if (cameraUnlocked == true)
        camera.ProcessMouseMovement(xoffset, yoffset);
    //std::cout << "Mouse coordinates: " << xpos << ", " << ypos << std::endl;
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
    ang += 0.01;
}

glm::mat4 resetModel(glm::mat4 model) {
    glm::mat4 initial_model = model;
    return initial_model;
}

//funcionalidade extra => implementar 3 sizes de maze (pequeno, médio, grande)
void goMaze(unsigned int cubeVAO, Shader Light, glm::mat4 model, unsigned int floorVAO, Shader skyShader, unsigned int pointVAO) {
    int i;
    int j;

    int maze[8][6] = {
        {1,1,1,0,1,1},
        {1,1,0,0,0,1},
        {1,0,0,1,0,1},
        {1,0,1,0,0,1},
        {1,0,0,1,0,1},
        {1,1,0,1,0,1},
        {1,0,0,0,1,1},
        {1,0,1,1,1,1},
    };

    //inital_model corresponds to the point of origin
    glm::mat4 initial_model;
    initial_model = model;
    //std::vector <int> arr;

    for (i = 0; i < 8; i++) {
        int pos = 0;
        for (j = 5; j >= 0; j--) {
            model = resetModel(initial_model);
            pos = 5 - j;

            if (maze[i][j] == 1) {  // Means there is a cube 
                model = glm::translate(model, glm::vec3(i, 0, pos));
                //Light.setMat4("model", model);

                Light.use();
                Light.setVec3("objectColor", 1.0f, 0.5f, 0.31f);
                Light.setVec3("lightColor", 1.0f, 1.0f, 1.0f);
                Light.setVec3("lightPos", lightPos);
                Light.setVec3("viewPos", camera.Position);

                // view/projection transformations
                glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
                glm::mat4 view = camera.GetViewMatrix();
                Light.setMat4("projection", projection);
                Light.setMat4("view", view);
                Light.setMat4("model", model);

                //pegar no modelo de ponto de origem e somar os valores x e z da translação => para obter as coordenadas de cada objeto no mundo
                //nota: pôr num array de posições 

                //arr.insert(arr.end(), { 6,9 });

                const float* pSource1 = (const float*)glm::value_ptr(camera.GetViewMatrix());
                for (int z = 0; z < 16; ++z)
                    dArray[z] = pSource1[z];

                int dArray1[16] = { 0.0 };

                const float* pSource = (const float*)glm::value_ptr(model);
                for (int i = 0; i < 16; ++i)
                    dArray1[i] = pSource[i];

                //guarda posições de matriz de cubos no vector (tendo em conta "i" e "j")
                arr.insert(arr.end(), { dArray1[12],dArray1[14] });

                float pos12 = (float)dArray1[12];
                float pos14 = (float)dArray1[14];

                float xPositive = pos12 + 0.62;
                float xNegative = pos12 - 0.62;
                float zPositive = pos14 + 0.62;
                float zNegative = pos14 - 0.62;

                arrLimits.insert(arrLimits.end(), { xPositive,xNegative,zPositive,zNegative });

                //upload texture of shaders
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture1);

                drawCubes(cubeVAO);

                for (int j = 0; j <= arrLimits.size() - 4; j = j + 4) {
                    if (camera.Position.x >= arrLimits[j + 1] && camera.Position.x <= arrLimits[j] && camera.Position.z >= arrLimits[j + 3] && camera.Position.z <= arrLimits[j + 2]) {
                        camera.Position = PositionBefore;
                    }
                }
                PositionBefore = camera.Position;
            }

            else {
                model = glm::translate(model, glm::vec3(i, 0, pos));
                //Light.setMat4("model", model);

                glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
                glm::mat4 view = camera.GetViewMatrix();
                //activate sky shader
                skyShader.use();
                skyShader.setMat4("projection", projection);
                skyShader.setMat4("view", view);
                skyShader.setMat4("model", model);

                //upload texture of shaders
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, texture3);

                drawFloor(floorVAO);

                if (i > 0 && i % 2 != 0) {

                    int dArray2[16] = { 0.0 };

                    const float* pSource2 = (const float*)glm::value_ptr(model);
                    for (int i = 0; i < 16; ++i)
                        dArray2[i] = pSource2[i];

                    float pos12V2 = (float)dArray2[12];
                    float pos14V2 = (float)dArray2[14];

                    float xPositive2 = pos12V2 + 0.20;
                    float xNegative2 = pos12V2 - 0.20;
                    float zPositive2 = pos14V2 + 0.20;
                    float zNegative2 = pos14V2 - 0.20;

                    if(i == 7)
                     XExitV2 = pos12V2 + 0.62;

                    arrLimits2.insert(arrLimits2.end(), { xPositive2,xNegative2,zPositive2,zNegative2 });

                    float x = 0.00f;
                    float z = 0.00f;

                    float X = 0.00f;
                    float X1 = 0.00f;
                    float Z1 = 0.00f;
                    float Z2 = 0.00f;

                    int currentPos = 0;

                    bool existe = false;

                    int count = 0;

                    for (int j = 0; j <= arrLimits2.size() - 4; j = j + 4) {
                        if (camera.Position.x >= arrLimits2[j + 1] && camera.Position.x <= arrLimits2[j] && camera.Position.z >= arrLimits2[j + 3] && camera.Position.z <= arrLimits2[j + 2]) {
                            if (alreadyThere.size() == 0) {
                                POINTS++;
                                alreadyThere.insert(alreadyThere.end(), { arrLimits2[j], arrLimits2[j + 1],arrLimits2[j + 2],arrLimits2[j + 3] });
                            }

                            if (alreadyThere.size() != 0) {

                                X = arrLimits2[j];
                                X1 = arrLimits2[j + 1];
                                Z1 = arrLimits2[j + 2];
                                Z2 = arrLimits2[j + 3];

                                for (int t = 0; t <= alreadyThere.size() - 4; t = t + 4) {
                                    if (camera.Position.x <= alreadyThere[t] && camera.Position.x >= alreadyThere[t + 1] && camera.Position.z <= alreadyThere[t + 2] && camera.Position.z >= alreadyThere[t + 3]) {
                                        existe = true;
                                    }
                                }

                                if (existe == false) {
                                    POINTS++;
                                    //std::cout << "|||||||||||ENTROU||||||||||||||||";
                                    alreadyThere.insert(alreadyThere.end(), { X,X1,Z1,Z2 });
                                }
                            }
                        }
                    }

                    if (alreadyThere.size() != 0) {
                        for (int t = 0; t <= alreadyThere.size() - 4; t = t + 4) {
                            if (xPositive2 == alreadyThere[t] && xNegative2 == alreadyThere[t + 1] && zPositive2 == alreadyThere[t + 2] && zNegative2 == alreadyThere[t + 3]) {
                                showPoint = false;
                                //if (cont == 0) {
                                //std::cout << POINTS << "\n";
                                cont = 1;
                                //}
                                break;
                            }
                            else
                                showPoint = true;
                        }
                    }

                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, texture1);
                    drawPoints(pointVAO, showPoint);
                }
            }
        }
    }
}

// utility function for loading a 2D texture from file
// ---------------------------------------------------
unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

void RenderText(Shader& shader, std::string text, float x, float y, float scale, glm::vec3 color)
{
    // activate corresponding render state
    shader.use();
    glUniform3f(glGetUniformLocation(shader.ID, "textColor"), color.x, color.y, color.z);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAOtext);

    // Iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBOtext);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}