#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <learnopengl/shader.h>
#include <learnopengl/camera.h>
#include <learnopengl/model.h>
#include <iostream>

//-----------------------------------函数声明-------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow *window);
unsigned int loadTexture(char const * path);

//-------------------------------------全局变量-------------------------------------------
//窗体宽高
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1600;

//摄像机相关
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// 时间
float deltaTime = 0.0f;
float lastFrame = 0.0f;

GLuint VAO;
GLuint VBO;
GLuint texture_diffuse1;
GLuint texture_normal1;

bool bShowBox = false;


//TextureFromFile(const char *path, const string &directory, bool gamma = false)
void initTexture()
{
    texture_diffuse1 = loadTexture("/Users/chen/Documents/LearnOpenGL-master/51CTO/Learn_OpenGL/resources/textures/brickwall.jpg");
    texture_normal1  = loadTexture("/Users/chen/Documents/LearnOpenGL-master/51CTO/Learn_OpenGL/resources/textures/brickwall_normal.jpg");
}

void initModel()
{
    float vertices[] = {
            -0.5f, -0.5f, -0.5f,            0.0f,  0.0f, -1.0f,0.0f, 0.0f,
            0.5f, -0.5f, -0.5f,            0.0f,  0.0f, -1.0f,1.0f, 0.0f,
            0.5f,  0.5f, -0.5f,            0.0f,  0.0f, -1.0f,1.0f, 1.0f,
            0.5f,  0.5f, -0.5f,            0.0f,  0.0f, -1.0f,1.0f, 1.0f,
            -0.5f,  0.5f, -0.5f,            0.0f,  0.0f, -1.0f,0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,            0.0f,  0.0f, -1.0f,0.0f, 0.0f,

            -0.5f, -0.5f,  0.5f,            0.0f,  0.0f,  1.0f,0.0f, 0.0f,
            0.5f, -0.5f,  0.5f,            0.0f,  0.0f,  1.0f,1.0f, 0.0f,
            0.5f,  0.5f,  0.5f,            0.0f,  0.0f,  1.0f,1.0f, 1.0f,
            0.5f,  0.5f,  0.5f,            0.0f,  0.0f,  1.0f,1.0f, 1.0f,
            -0.5f,  0.5f,  0.5f,            0.0f,  0.0f,  1.0f,0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,            0.0f,  0.0f,  1.0f,0.0f, 0.0f,

            -0.5f,  0.5f,  0.5f,            -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,            -1.0f,  0.0f,  0.0f, 1.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,            -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
            -0.5f, -0.5f, -0.5f,            -1.0f,  0.0f,  0.0f, 0.0f, 1.0f,
            -0.5f, -0.5f,  0.5f,            -1.0f,  0.0f,  0.0f, 0.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,            -1.0f,  0.0f,  0.0f, 1.0f, 0.0f,

            0.5f,  0.5f,  0.5f,            1.0f,  0.0f,  0.0f,1.0f, 0.0f,
            0.5f,  0.5f, -0.5f,            1.0f,  0.0f,  0.0f,1.0f, 1.0f,
            0.5f, -0.5f, -0.5f,            1.0f,  0.0f,  0.0f,0.0f, 1.0f,
            0.5f, -0.5f, -0.5f,            1.0f,  0.0f,  0.0f,0.0f, 1.0f,
            0.5f, -0.5f,  0.5f,            1.0f,  0.0f,  0.0f,0.0f, 0.0f,
            0.5f,  0.5f,  0.5f,            1.0f,  0.0f,  0.0f,1.0f, 0.0f,

            -0.5f, -0.5f, -0.5f,             0.0f, -1.0f,  0.0f,0.0f, 1.0f,
            0.5f, -0.5f, -0.5f,             0.0f, -1.0f,  0.0f,1.0f, 1.0f,
            0.5f, -0.5f,  0.5f,             0.0f, -1.0f,  0.0f,1.0f, 0.0f,
            0.5f, -0.5f,  0.5f,             0.0f, -1.0f,  0.0f,1.0f, 0.0f,
            -0.5f, -0.5f,  0.5f,             0.0f, -1.0f,  0.0f,0.0f, 0.0f,
            -0.5f, -0.5f, -0.5f,             0.0f, -1.0f,  0.0f,0.0f, 1.0f,

            -0.5f,  0.5f, -0.5f,             0.0f,  1.0f,  0.0f,0.0f, 1.0f,
            0.5f,  0.5f, -0.5f,             0.0f,  1.0f,  0.0f,1.0f, 1.0f,
            0.5f,  0.5f,  0.5f,             0.0f,  1.0f,  0.0f,1.0f, 0.0f,
            0.5f,  0.5f,  0.5f,             0.0f,  1.0f,  0.0f,1.0f, 0.0f,
            -0.5f,  0.5f,  0.5f,             0.0f,  1.0f,  0.0f,0.0f, 0.0f,
            -0.5f,  0.5f, -0.5f,             0.0f,  1.0f,  0.0f,0.0f, 1.0f,
    };

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER,VBO);
    glBufferData(GL_ARRAY_BUFFER,sizeof(vertices),vertices,GL_STATIC_DRAW);

    // (GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, const void *pointer);
    glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)0);
    glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(sizeof(float)*3));
    glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,8*sizeof(float),(void*)(sizeof(float)*6));
    glEnableVertexAttribArray(0);// 打开锚定点0
    glEnableVertexAttribArray(1);// 打开锚定点1
    glEnableVertexAttribArray(2);// 打开锚定点1

    glBindBuffer(GL_ARRAY_BUFFER,0);

    glBindVertexArray(0);
}

//主函数
int main()
{
    // glfw: 初始化和配置
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // uncomment this statement to fix compilation on OS X
#endif

    // glfw 窗体创建
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "ModelDemo", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "创建GLFW窗体失败" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);

    // 鼠标滑动回调函数
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // 设置全局opengl状态
    glEnable(GL_DEPTH_TEST);//开启深度测试

    // diffuse
//    Shader ourShader("/Users/zhechen/Documents/LearnOpenGL-master/LearnOpenGL-test/src/3.model_loading/1.model_loading/1.model_loading.vs", "/Users/zhechen/Documents/LearnOpenGL-master/LearnOpenGL-test/src/3.model_loading/1.model_loading/1.model_loading.fs");

    // normal
    Shader ourShader("/Users/chen/Documents/LearnOpenGL/src/3.model_loading/1.model_loading/4.normal_mapping.vert",
                     "/Users/chen/Documents/LearnOpenGL/src/3.model_loading/1.model_loading/4.normal_mapping.frag");

    Shader ts_Shader("/Users/chen/Documents/LearnOpenGL/src/3.model_loading/1.model_loading/normal_TS.vert",
                     "/Users/chen/Documents/LearnOpenGL/src/3.model_loading/1.model_loading/normal_TS.frag");

    Shader ws_Shader("/Users/chen/Documents/LearnOpenGL/src/3.model_loading/1.model_loading/normal_WS.vert",
                     "/Users/chen/Documents/LearnOpenGL/src/3.model_loading/1.model_loading/normal_WS.frag");

    Shader yuanshiShader("/Users/chen/Documents/LearnOpenGL/src/3.model_loading/1.model_loading/4.normal_mapping_yuanshi.vert",
                         "/Users/chen/Documents/LearnOpenGL/src/3.model_loading/1.model_loading/4.normal_mapping_yuanshi.frag");

    Shader geoNormalShader("/Users/chen/Documents/LearnOpenGL/src/3.model_loading/1.model_loading/9.3.normal_visualization.vert",
                           "/Users/chen/Documents/LearnOpenGL/src/3.model_loading/1.model_loading/9.3.normal_visualization.frag",
                           "/Users/chen/Documents/LearnOpenGL/src/3.model_loading/1.model_loading/9.3.normal_visualization.geom");

    Model ourModel("/Users/chen/Documents/LearnOpenGL/resources/objects/nanosuit/nanosuit.obj");

    Model testModel("/Users/chen/Documents/LearnOpenGL/resources/objects/nanosuit/nanosuit.obj");
    // draw in wireframe
//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    if(bShowBox)
    {
        initModel();
        initTexture();
    }

    // glm::mat4 test = glm::mat4(1.0f);
    // test = glm::translate(test, glm::vec3(-0.8f, -0.8f, 0.0f));
    // test = glm::scale(test, glm::vec3(0.1f, 0.1f, 0.1f));
    // test = glm::rotate(test, (float)glfwGetTime(), glm::vec3(0.0f, 1.0f, 0.0f));
    //
    // glm::mat4 view = camera.GetViewMatrix();
    // test = view*test;
    //
    // auto in_t  = glm::transpose(glm::inverse(test));
    // auto t_in  = glm::inverse(glm::transpose(test));
    //
    // if(in_t == t_in)
    // {
    //     std::cout<<"=====dddd=="<<std::endl;
    // }

    //循环渲染
    while (!glfwWindowShouldClose(window))
    {
        //获取时间
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // 键盘输入
        processInput(window);

        // 渲染
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        float timeScale = 0.6f;
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        glm::vec3 lightPos(0.0f, 1.5f, 2.0f);
//        glm::vec3 lightPos(0.0f, 1.0f, 2.0f);

        {
            ourShader.use();
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(-0.8f, -0.8f, 0.0f));
            model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
            model = glm::rotate(model, (float)glfwGetTime()*timeScale, glm::vec3(0.0f, 1.0f, 0.0f));

            ourShader.setMat4("model", model);
            ourShader.setMat4("projection", projection);
            ourShader.setMat4("view", view);
            ourShader.setVec3("lightPos", glm::vec3(-0.5f, 1.5f, 2.0f));
            ourShader.setVec3("viewPos", camera.Position);

            ourModel.Draw(ourShader);
        }

        // 切线空间计算效果似乎更好一些
        // tangent space
        {
            ts_Shader.use();
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.0f, -0.8f, 0.0f));
            model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
            model = glm::rotate(model, (float)glfwGetTime()*timeScale, glm::vec3(0.0f, 1.0f, 0.0f));

            ts_Shader.setMat4("model", model);
            ts_Shader.setMat4("projection", projection);
            ts_Shader.setMat4("view", view);
            ts_Shader.setVec3("lightPos", glm::vec3 (-0.5f, 1.5f, 2.0f));
            ts_Shader.setVec3("viewPos", camera.Position);

            testModel.Draw(ts_Shader);
        }

        // 纯粹的法线
        // 生成法线并不需要
        {
            geoNormalShader.use();
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.8f, -0.8f, 0.0f));
            model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
            model = glm::rotate(model, (float)glfwGetTime()*timeScale, glm::vec3(0.0f, 1.0f, 0.0f));

            geoNormalShader.setMat4("model", model);
            geoNormalShader.setMat4("projection", projection);
            geoNormalShader.setMat4("view", view);
//            geoNormalShader.setVec3("lightPos", glm::vec3 (-0.5f, 1.5f, 2.0f));
//            geoNormalShader.setVec3("viewPos", camera.Position);

            testModel.Draw(geoNormalShader);
        }

        // 纯粹的模型
        {
            ws_Shader.use();
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(0.8f, -0.8f, 0.0f));
            model = glm::scale(model, glm::vec3(0.1f, 0.1f, 0.1f));
            model = glm::rotate(model, (float)glfwGetTime()*timeScale, glm::vec3(0.0f, 1.0f, 0.0f));

            ws_Shader.setMat4("model", model);
            ws_Shader.setMat4("projection", projection);
            ws_Shader.setMat4("view", view);
            ws_Shader.setVec3("lightPos", glm::vec3(-0.5f, 1.5f, 2.0f));
            ws_Shader.setVec3("viewPos", camera.Position);

            testModel.Draw(ws_Shader);
        }

        // render box
        if(bShowBox)
        {
            glBindVertexArray(VAO);
            ts_Shader.use();
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(-0.8f, -0.8f, 0.0f));
            model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
            model = glm::rotate(model, (float)glfwGetTime()*timeScale*0.2f, glm::vec3(0.0f, 1.0f, 0.0f));

            ts_Shader.setMat4("model", model);
            ts_Shader.setMat4("projection", projection);
            ts_Shader.setMat4("view", view);
            ts_Shader.setVec3("lightPos", glm::vec3(-0.5f, 1.5f, 2.0f));
            ts_Shader.setVec3("viewPos", camera.Position);

            glActiveTexture(GL_TEXTURE0);glBindTexture(GL_TEXTURE_2D , texture_diffuse1);
            glActiveTexture(GL_TEXTURE1);glBindTexture(GL_TEXTURE_2D , texture_normal1);

            ts_Shader.setInt("texture_diffuse1", 0);
            ts_Shader.setInt("texture_normal1" , 1);

            glDrawArrays(GL_TRIANGLES , 0 , 36);
        }


        // glfw: 交换缓冲区和轮询IO事件（按下/释放按键，移动鼠标等）
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // glfw: 终止，清除所有先前分配的GLFW资源。
    glfwTerminate();
    return 0;
}
//键盘按键控制
void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)camera.ProcessKeyboard(Camera_Movement::FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)camera.ProcessKeyboard(Camera_Movement::BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)camera.ProcessKeyboard(Camera_Movement::LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)camera.ProcessKeyboard(Camera_Movement::RIGHT, deltaTime);
}

// glfw: 窗口改变回调函数
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
}

// glfw: 鼠标滑动回调函数
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;
    camera.ProcessMouseMovement(xoffset, yoffset);
}
// glfw: 鼠标滚轮回调函数
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

unsigned int loadTexture(char const * path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char *data = stbi_load(path, &width, &height, &nrComponents, 0);
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

