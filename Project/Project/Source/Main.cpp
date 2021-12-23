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
void goMaze(unsigned int cubeVAO, Shader Light, glm::mat4 model, unsigned int floorVAO);
unsigned int loadTexture(char const* path);
glm::mat4 resetModel(glm::mat4 model);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(6.0f, 2.0f, 2.0f));
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

int stop = 0;
bool cont = false;
int dArray[16] = { 0.0 };
std::vector <int> arr;

void drawCubes(unsigned int cubeVAO) {
    glBindVertexArray(cubeVAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void drawFloor(unsigned int floorVAO) {
    glBindVertexArray(floorVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
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
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
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
    
    //Shader ourShader("C:\\Users\\Legion\\Desktop\\Storage\\Uni\\3ano\\CG\\Walls\\Project_Walls\\Project\\Project\\Source\\VS.vs", "C:\\Users\\Legion\\Desktop\\Storage\\Uni\\3ano\\CG\\Walls\\Project_Walls\\Project\\Project\\Source\\FS.fs");
    
    
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

    //unsigned int texture1 = loadTexture("C:\\Users\\Legion\\Desktop\\Storage\\Uni\\3ano\\CG\\Walls\\Project_Walls\\Project\\Project\\Source\\awesomeface.png");
      unsigned int texture1 = loadTexture("C:\\Users\\Legion\\Desktop\\Storage\\Uni\\3ano\\CG\\Walls\\Project_Walls\\Project\\Project\\Source\\wall.jpg");
    //---------------------------------------------------------------------------------------------------

    //lightingShader.use();
    //glUniform1i(glGetUniformLocation(lightingShader.ID, "texture1"), 0);

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
        
        //atualização da matrix model
        lightingShader.setMat4("model", model);

        //upload texture of shaders
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        
        // render the cube
        /*glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);*/
        //drawCubes(cubeVAO);

        //drawing maze
        goMaze(cubeVAO, lightingShader, model, floorVAO);

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

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS and camera.Position.y <= 3.0f and camera.Position.y >= -0.3f)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS and camera.Position.y <= 3.0f and camera.Position.y >= -0.3f)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);

    if (camera.Position.y > 3.0f)
        camera.Position.y = 3.0f;

    if (camera.Position.y < -0.3f)
        camera.Position.y = -0.3f;
    
    //teste => para usar depois em colisões
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
            std::cout << i << "|";*/

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

void goMaze(unsigned int cubeVAO, Shader Light, glm::mat4 model, unsigned int floorVAO) {
    int i;
    int j;

    int maze[5][6] = {
        {1,1,0,0,1,1},
        {1,1,1,0,0,1},
        {1,0,0,1,0,1},
        {1,1,0,0,0,1},
        {1,1,0,1,1,1}
    };

    //inital_model corresponds to the point of origin
    glm::mat4 initial_model;
    initial_model = model;
    std::vector <int> arr;

    for (i = 0; i < 5; i++) {
        int pos = 0;
        for (j = 5; j >= 0; j--) {
            model = resetModel(initial_model);
            pos = 5 - j;
            if (maze[i][j] == 1) {  // Means there is a cube 
                model = glm::translate(model, glm::vec3(i, 0, pos));
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
                arr.insert(arr.end(), {dArray1[12],dArray1[14]});
                
                drawCubes(cubeVAO);
            }

            else {
                model = glm::translate(model, glm::vec3(i, 0, pos));
                Light.setMat4("model", model);
                drawFloor(floorVAO);
            }

            /*double dArray[16] = {0.0};

            const float* pSource = (const float*)glm::value_ptr(model);
            for (int i = 0; i < 16; ++i)
                dArray[i] = pSource[i];

            for (int t = 0; t < 16; t++) {
                if (camera.Position.x == dArray[t] || camera.Position.z == dArray[t])
                    std::cout << "IGUAL ";
            }*/

            /*if (stop == 0) {
                    const float* pSource = (const float*)glm::value_ptr(model);
                    for (int i = 0; i < 16; ++i)
                        dArray[i] = pSource[i];

                    for (int t = 0; t < 16; t++) {
                        std::cout << "Number t: " << dArray[t] << "\n";
                    }
                    std::cout << "|||||||" << "\n";
                }
                stop++;*/

            if (stop < 5) {
                std::cout << "VEC:";
                for (int z = 0; z < arr.size(); ++z)
                    std::cout << "[" << arr[z] << "]";
                
                std::cout << "\n";
            }
        }

        if (stop < 5) {
            for (int z = 0; z < 16; ++z)
                std::cout << "[" << dArray[z] << "]";

            std::cout << "\n\n";
        }

        for (int j = 0; j <= arr.size() - 2; j = j + 2) {
            if (dArray[12] == arr[j] && dArray[14] == arr[j + 1])
                std::cout << "IGUAL" << "\n";
        }

        /*if (stop == 5) {
            std::cout << "VEC:";
            for (int z = 0; z < arr.size(); ++z)
                std::cout << "[" << arr[z] << "]";
        }*/
        stop++;
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





    

