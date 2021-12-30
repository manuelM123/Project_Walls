#include <glad/glad.h>

#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <shader_m.h>
#include <camera.h>
#include <iostream>
#include <vector>

//nota => ver camera position com cube position

// New file
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
void goMaze(unsigned int cubeVAO, Shader Light, glm::mat4 model, unsigned int floorVAO,Shader skyShader, unsigned int pointVAO);
unsigned int loadTexture(char const* path);
glm::mat4 resetModel(glm::mat4 model);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 2.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

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

float ang = 0.01;
float yMax = 0.00;

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
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
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
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
    
    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    
    // build and compile our shader zprogram
    // ------------------------------------

    //C:\\Users\\Legion\\Desktop\\Storage\\Uni\\3ano\\CG\\Walls\\Project_Walls\\Project\\Project\\Source
    Shader lightingShader("C:\\Users\\Legion\\Desktop\\Storage\\Uni\\3ano\\CG\\Walls\\Project_Walls\\Project\\Project\\Source\\2.1.basic_lighting.vs", "C:\\Users\\Legion\\Desktop\\Storage\\Uni\\3ano\\CG\\Walls\\Project_Walls\\Project\\Project\\Source\\2.1.basic_lighting.fs");
    Shader lampShader("C:\\Users\\Legion\\Desktop\\Storage\\Uni\\3ano\\CG\\Walls\\Project_Walls\\Project\\Project\\Source\\2.1.lamp.vs", "C:\\Users\\Legion\\Desktop\\Storage\\Uni\\3ano\\CG\\Walls\\Project_Walls\\Project\\Project\\Source\\2.1.lamp.fs");
    Shader skyShader("C:\\Users\\Legion\\Desktop\\Storage\\Uni\\3ano\\CG\\Walls\\Project_Walls\\Project\\Project\\Source\\VS.vs", "C:\\Users\\Legion\\Desktop\\Storage\\Uni\\3ano\\CG\\Walls\\Project_Walls\\Project\\Project\\Source\\FS.fs");
    
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

        //ch�o
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
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); //2-coluna onde come�a / 2 - tamanho / 9N elementos na linha / 6 tamanho at� inicio das normais
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
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); //2-coluna onde come�a / 2 - tamanho / 9N elementos na linha / 6 tamanho at� inicio das normais
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
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); //2-coluna onde come�a / 2 - tamanho / 9N elementos na linha / 6 tamanho at� inicio das normais
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
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float))); //2-coluna onde come�a / 2 - tamanho / 9N elementos na linha / 6 tamanho at� inicio das normais
    glEnableVertexAttribArray(2);

    //unsigned int texture1 = loadTexture("C:\\Users\\Legion\\Desktop\\Storage\\Uni\\3ano\\CG\\Walls\\Project_Walls\\Project\\Project\\Source\\awesomeface.png");
      texture1 = loadTexture("C:\\Users\\Legion\\Desktop\\Storage\\Uni\\3ano\\CG\\Walls\\Project_Walls\\Project\\Project\\Source\\wall5.jpeg");
      texture2 = loadTexture("C:\\Users\\Legion\\Desktop\\Storage\\Uni\\3ano\\CG\\Walls\\Project_Walls\\Project\\Project\\Source\\sky.png");
      texture3 = loadTexture("C:\\Users\\Legion\\Desktop\\Storage\\Uni\\3ano\\CG\\Walls\\Project_Walls\\Project\\Project\\Source\\floor.png");
    //---------------------------------------------------------------------------------------------------

    //lightingShader.use();
    //glUniform1i(glGetUniformLocation(lightingShader.ID, "texture1"), 0);
    //glUniform1i(glGetUniformLocation(skyShader.ID, "texture2"), 1);

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
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
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
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
        
        //atualiza��o da matrix model
        lightingShader.setMat4("model", model);

        //upload texture of shaders
        //glActiveTexture(GL_TEXTURE0);
        //glBindTexture(GL_TEXTURE_2D, texture1);

        //drawing maze
        goMaze(cubeVAO, lightingShader, model, floorVAO, skyShader, pointVAO);

        //activate sky shader
        skyShader.use();
        skyShader.setMat4("projection", projection);
        skyShader.setMat4("view", view);
        skyShader.setMat4("model", model);

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

        //ourShader.use();
        
        
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
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

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
    //if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS && yMax >= 105) ==> limite eixo y a usar quando jogador estiver bem posicionado

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);


    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        texture1 = loadTexture("C:\\Users\\Legion\\Desktop\\Storage\\Uni\\3ano\\CG\\Walls\\Project_Walls\\Project\\Project\\Source\\wall2.jpg");
        texture3 = loadTexture("C:\\Users\\Legion\\Desktop\\Storage\\Uni\\3ano\\CG\\Walls\\Project_Walls\\Project\\Project\\Source\\erva.jpg");

    }

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        texture1 = loadTexture("C:\\Users\\Legion\\Desktop\\Storage\\Uni\\3ano\\CG\\Walls\\Project_Walls\\Project\\Project\\Source\\wall5.jpeg");
        texture3 = loadTexture("C:\\Users\\Legion\\Desktop\\Storage\\Uni\\3ano\\CG\\Walls\\Project_Walls\\Project\\Project\\Source\\floor.png");
    }

    if (camera.Position.y > 0.0f)
        camera.Position.y = 0.0f;

    if (camera.Position.y < 0.0f)
        camera.Position.y = 0.0f;

    if (camera.Position.x < -0.50f)
        camera.Position.x = -0.50f;

    
    //teste => para usar depois em colis�es
    if (glfwGetKey(window, GLFW_KEY_END) == GLFW_PRESS) {
        std::cout << "Camera X coordinates: " << camera.Position.x;
        std::cout << "Camera Y coordinates: " << camera.Position.y;
        std::cout << "Camera Z coordinates: " << camera.Position.z << "\n\n"; 
    }

    if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS) {
        /*glm::mat4 T = glm::mat4(1.0f);
        std::vector <int> arr;
        arr.push_back(1);
        arr.push_back(2);
        arr.push_back(3);
        arr.insert(arr.end(),{ 6,9 });

        for (int i : arr)
            std::cout << i << "|"; dsdsdsds*/

        /*double dArray[16] = {0.0};
        const float* pSource = (const float*)glm::value_ptr(T);

        std::cout << "\n" << "----------Antes-----------" << "\n";

        for (int j = 0; j < 16; j++)
            std::cout << "[" << dArray[j] << "]";

        std::cout << "\n" << "----------Depois-----------" << "\n";
        T = glm::translate(T, glm::vec3(1.0f, 0.0f, 1.0f));

        for (int i = 0; i < 16; i++)
            dArray[i] = pSource[i];
        for (int j = 0; j < 16; j++)
            std::cout << "[" << dArray[j] << "]";

        std::cout << "\n" << "---------------------------" << "\n";*/

        cont = 0;
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
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

//funcionalidade extra => implementar 3 sizes de maze (pequeno, m�dio, grande)
void goMaze(unsigned int cubeVAO, Shader Light, glm::mat4 model, unsigned int floorVAO, Shader skyShader, unsigned int pointVAO) {
    int i;
    int j;

    int maze[8][6] = {
        {1,1,0,0,1,1},
        {1,0,1,0,0,1},
        {1,0,0,1,0,1},
        {1,0,0,0,0,1},
        {1,1,0,1,0,1},
        {1,1,1,1,0,1},
        {1,0,0,0,0,1},
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

                //pegar no modelo de ponto de origem e somar os valores x e z da transla��o => para obter as coordenadas de cada objeto no mundo
                //nota: p�r num array de posi��es 

                //arr.insert(arr.end(), { 6,9 });

                const float* pSource1 = (const float*)glm::value_ptr(camera.GetViewMatrix());
                for (int z = 0; z < 16; ++z)
                    dArray[z] = pSource1[z];

                int dArray1[16] = { 0.0 };

                const float* pSource = (const float*)glm::value_ptr(model);
                for (int i = 0; i < 16; ++i)
                    dArray1[i] = pSource[i];

                //guarda posi��es de matriz de cubos no vector (tendo em conta "i" e "j")
                arr.insert(arr.end(), {dArray1[12],dArray1[14]});

                float pos12 = (float)dArray1[12];
                float pos14 = (float)dArray1[14];

                float xPositive = pos12 + 0.62;
                float xNegative = pos12 - 0.62;
                float zPositive = pos14 + 0.62;
                float zNegative = pos14 - 0.62;

                arrLimits.insert(arrLimits.end(), {xPositive,xNegative,zPositive,zNegative});

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

                if (i > 0 && i%2 != 0) {

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
                        if (camera.Position.x >= arrLimits2[j + 1] && camera.Position.x <= arrLimits2[j] && camera.Position.z >= arrLimits2[j + 3] && camera.Position.z <= arrLimits2[j + 2]){
                            if (alreadyThere.size() == 0) {
                                POINTS++;
                                alreadyThere.insert(alreadyThere.end(), {arrLimits2[j], arrLimits2[j+1],arrLimits2[j+2],arrLimits2[j+3]});
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
                                    std::cout << POINTS << "\n";
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
                    drawPoints(pointVAO,showPoint);
                }

            }

            //---------------------------------------------------------------------------------------

            /*if (stop < 5) {
                std::cout << "VEC Limits:";
                for (int z = 0; z < arrLimits.size(); ++z)
                    std::cout << "[" << arrLimits[z] << "]";

                std::cout << "\n";
            }*/

        }

        /*if (stop < 5) {
            for (int z = 0; z < 16; ++z)
                std::cout << "[" << dArray[z] << "]";

            std::cout << "\n\n";
        }*/

        //parte das colis�es ==> ver melhor ==> pontos negativos nas posi��es da c�mera(dependendo da orienta��o da mesma)
        /*for (int j = 0; j <= arr.size() - 2; j = j + 2) {
            if (dArray[12] == arr[j] && dArray[14] == arr[j + 1]) {
            //if (camera.Position.x == arr[j] || camera.Position.z == arr[j + 1]) {
                std::cout << "ARR:" << arr[j] << "|" << arr[j + 1] << "\n";
                }
            //else
                //std::cout << "CAM:" << dArray[12] << "|" << dArray[14] << "\n";
        }*/

        /*for (int j = 0; j <= arrLimits.size() - 4; j = j + 4) {
            if(camera.Position.x >= arrLimits[j+1] && camera.Position.x <= arrLimits[j] && camera.Position.z >= arrLimits[j+3] && camera.Position.z <= arrLimits[j+2]){
                if (cont == 0) {
                    std::cout << "WALL" << "\n";
                    std::cout << "X: " << xPositionBefore << "\n";
                    std::cout << "Z: " << zPositionBefore << "\n";
                    std::cout << "J: " << j << "\n";
                }

                camera.Position.x = xPositionBefore;
                camera.Position.z = zPositionBefore;

                cont = 1;
            }
            else {
                xPositionBefore = camera.Position.x;
                zPositionBefore = camera.Position.z;
            }
        }*/

        /*if (stop == 4) {
            std::cout << arr.size();
        }*/
        //stop++;
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





    

