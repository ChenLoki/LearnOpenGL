#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>

#include <learnopengl/filesystem.h>
#include <learnopengl/shader_t.h>

#include <iostream>

#include <glm/glm.hpp>
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow *window);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// stores how much we're seeing of either texture
float mixValue = 0.2f;

// int main(int argc, char** argv)
// {
//     // Application app;
//     // app.PushLayer(new SandboxLayer());
//     // app.Run();
//
//     /// 产生窗口的最小框架
//     {
//         glfwInit();
//         glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);// opengl版本号
//         glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);// opengl版本号
//         glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);//// opengl配置 核心模式（还有立即模式）
// #ifdef __APPLE__
//         glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
// #endif
//
//         GLFWwindow* window = glfwCreateWindow(600, 600, "LearnOpenGL", NULL, NULL);
//         if (window == nullptr)
//         {
//             std::cout << "Failed to create GLFW window" << std::endl;
//             glfwTerminate();
//             return -1;
//         }
//
//         // gladLoadGLLoader函数必须有这个context
//         glfwMakeContextCurrent(window);
//
//         // GLAD是用来管理OpenGL的函数指针的，所以在调用任何OpenGL的函数之前我们需要初始化GLAD
//         if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
//         {
//             std::cout << "Failed to initialize GLAD" << std::endl;
//             return -1;
//         }
//
//         glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
//
//         //
//         float vertices[] = {
//             0.5f, 0.5f, 0.0f,    1.0,0.0,0.0,  1.0,1.0,
//             0.5f, -0.5f, 0.0f,   0.0,1.0,0.0,  1.0,0.0,
//             -0.5f, -0.5f, 0.0f,  0.0,0.0,1.0,  0.0,0.0,
//             -0.5f, 0.5f, 0.0f,   1.0,0.0,0.0,  0.0,1.0,
//             // -0.5f, -0.5f, 0.0f, 0.0,1.0,0.0,
//             // -0.5f, 0.5f, 0.0f,  0.0,0.0,1.0,
//         };
//
//         unsigned int indices[] = {
//             0,1,3,
//             1,2,3,
//         };
//
//         // shader
//         Shader test_shader("/Users/chen/Documents/LearnOpenGL/src/1.getting_started/4.6.textures_exercise4/4.5.texture.vert",
//                      "/Users/chen/Documents/LearnOpenGL/src/1.getting_started/4.6.textures_exercise4/4.5.texture.frag");
//
//         //1、创建
//         //2、绑定
//         //3、添加数据
//
//         // VAO
//         unsigned int VAO;
//         glGenVertexArrays(1, &VAO);
//         glBindVertexArray(VAO);
//
//         unsigned int EBO;
//         glGenBuffers(1,&EBO);
//         glBindBuffer(GL_ELEMENT_ARRAY_BUFFER , EBO);
//         glBufferData(GL_ELEMENT_ARRAY_BUFFER , sizeof(indices) , indices , GL_STATIC_DRAW);
//
//         unsigned int VBO;
//         glGenBuffers(1, &VBO);
//         glBindBuffer(GL_ARRAY_BUFFER , VBO);
//         glBufferData(GL_ARRAY_BUFFER , sizeof(vertices) , vertices , GL_STATIC_DRAW);
//
//         glVertexAttribPointer(0,3,GL_FLOAT,GL_FALSE,8*sizeof(float) , (void*)0);
//         glVertexAttribPointer(1,3,GL_FLOAT,GL_FALSE,8*sizeof(float) , (void*)(sizeof(float)*3));
//         glVertexAttribPointer(2,2,GL_FLOAT,GL_FALSE,8*sizeof(float) , (void*)(sizeof(float)*6));
//         glEnableVertexAttribArray(0);
//         glEnableVertexAttribArray(1);
//         glEnableVertexAttribArray(2);
//
//         glBindVertexArray(0);
//
//         // texture
//         // std::string path = "/Users/chen/Documents/Learn_OpenGL_cz/";
//         // auto tex	= Texture2D::Create(path+"assets/textures/Checkerboard.png");
//         // std::dynamic_pointer_cast<OpenGLTexture2D>(tex)->Bind();
//         // std::dynamic_pointer_cast<OpenGLTexture2D>(tex)->SetInt("u_Texture", 0);
//         unsigned int texture1;
//         glGenTextures(1,&texture1);
//         glBindTexture(GL_TEXTURE_2D, texture1);
//         // format
//         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
//         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//         glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//
//         // data
//         int width, height, nrChannels;
//         stbi_set_flip_vertically_on_load(true);
//         // unsigned char *data = stbi_load((path+"assets/textures/Checkerboard.png").c_str(), &width, &height, &nrChannels, 0);
//         auto data = stbi_load(FileSystem::getPath("resources/textures/container.png").c_str(), &width, &height, &nrChannels, 0);
//
//         if (data)
//         {
//             glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
//             glGenerateMipmap(GL_TEXTURE_2D);
//         }
//         else
//         {
//             std::cout << "Failed to load texture" << std::endl;
//         }
//         stbi_image_free(data);
//
//
//         //texture2
//         unsigned int texture2;
//         glGenTextures(1,&texture2);
//         glBindTexture(GL_TEXTURE_2D, texture2);
//         // format
//         glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_WRAP_S , GL_REPEAT);
//         glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_WRAP_T , GL_REPEAT);
//         glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_MIN_FILTER , GL_LINEAR);
//         glTexParameteri(GL_TEXTURE_2D , GL_TEXTURE_MAG_FILTER , GL_LINEAR);
//
//         // data
//         int width2, height2, nrChannels2;
//         stbi_set_flip_vertically_on_load(true);
//         // unsigned char *data2 = stbi_load((path+"assets/textures/ChernoLogo.png").c_str(), &width, &height, &nrChannels, 0);
//         auto data2 = stbi_load(FileSystem::getPath("resources/textures/awesomeface.png").c_str(), &width, &height, &nrChannels, 0);
//
//         if (data2)
//         {
//             glTexImage2D(GL_TEXTURE_2D , 0 , GL_RGB , width2 , height2 , 0 , GL_RGB , GL_UNSIGNED_BYTE , data2);
//             glGenerateMipmap(GL_TEXTURE_2D);
//         }
//         else
//         {
//             std::cout << "Failed to load texture" << std::endl;
//         }
//         stbi_image_free(data2);
//
//         // test_shader->Bind();
//         // test_shader->SetInt("texture1",0);
//         // test_shader->SetInt("u_textureb",1);
//         glUseProgram(test_shader.ID);
//         glUniform1i(glGetUniformLocation(test_shader.ID, "texture1"), 0);
//         glUniform1i(glGetUniformLocation(test_shader.ID, "texture2"), 1);
//
//         while (!glfwWindowShouldClose(window))
//         {
//             processInput(window);
//
//             glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
//             glClear(GL_COLOR_BUFFER_BIT);
//
//             // test_shader->Bind();
//             test_shader.use();
//             auto time = glfwGetTime();
//             float color_r = sin(time)*0.5+0.5;
//             float color_g = cos(time)*0.5+0.5;
//             float color_b = cos(time+2.0)*0.5+0.5;
//             test_shader.setVec3("u_color" , glm::vec3(color_r,color_g,color_b));
//
//             glActiveTexture(GL_TEXTURE0);
//             glBindTexture(GL_TEXTURE_2D , texture1);
//             glUniform1i(glGetUniformLocation(test_shader.ID, "texture1"), 0);
//
//             glActiveTexture(GL_TEXTURE0+1);
//             glBindTexture(GL_TEXTURE_2D , texture2);
//             glUniform1i(glGetUniformLocation(test_shader.ID, "texture2"), 1);
//
//
//
//
//             glBindVertexArray(VAO);
//             // glDrawArrays(GL_TRIANGLES , 0 , 6);
//             glDrawElements(GL_TRIANGLES , 6, GL_UNSIGNED_INT , 0);
//             // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
//             // glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
//
//
//
//             glfwSwapBuffers(window);
//             glfwPollEvents();
//         }
//
//         glDeleteVertexArrays(1, &VAO);
//         glDeleteBuffers(1, &VBO);
//         glDeleteBuffers(1, &EBO);
//
//         glfwTerminate();
//     }
//
//     return 0;
// }
//
// void processInput(GLFWwindow *window)
// {
//     if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
//         glfwSetWindowShouldClose(window, true);
// }
//
//
// void framebuffer_size_callback(GLFWwindow* window, int width, int height)
// {
//     // make sure the viewport matches the new window dimensions; note that width and
//     // height will be significantly larger than specified on retina displays.
//     glViewport(0, 0, width, height);
// }

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

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }


    // build and compile our shader zprogram
    // ------------------------------------
    Shader ourShader("/Users/chen/Documents/LearnOpenGL/src/1.getting_started/4.6.textures_exercise4/4.5.texture.vert",
                     "/Users/chen/Documents/LearnOpenGL/src/1.getting_started/4.6.textures_exercise4/4.5.texture.frag");

    // set up vertex data (and buffer(s)) and configure vertex attributes
    // ------------------------------------------------------------------
    float vertices[] = {
        // positions          // colors           // texture coords
         0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   1.0f, 1.0f, // top right
         0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 0.0f,   1.0f, 0.0f, // bottom right
        -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f, // bottom left
        -0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   0.0f, 1.0f  // top left
    };
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    // texture coord attribute
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);


    // load and create a texture
    // -------------------------
    unsigned int texture1, texture2;
    // texture 1
    // ---------
    glGenTextures(1, &texture1);
    glBindTexture(GL_TEXTURE_2D, texture1);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    int width, height, nrChannels;
    stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
    // The FileSystem::getPath(...) is part of the GitHub repository so we can find files on any IDE/platform; replace it with your own image path.
    unsigned char *data = stbi_load(FileSystem::getPath("resources/textures/container.jpg").c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    // texture 2
    // ---------
    glGenTextures(1, &texture2);
    glBindTexture(GL_TEXTURE_2D, texture2);
    // set the texture wrapping parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	// set texture wrapping to GL_REPEAT (default wrapping method)
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    // set texture filtering parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    // load image, create texture and generate mipmaps
    data = stbi_load(FileSystem::getPath("resources/textures/awesomeface.png").c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        // note that the awesomeface.png has transparency and thus an alpha channel, so make sure to tell OpenGL the data type is of GL_RGBA
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    // tell opengl for each sampler to which texture unit it belongs to (only has to be done once)
    // -------------------------------------------------------------------------------------------
    ourShader.use(); // don't forget to activate/use the shader before setting uniforms!
    // either set it manually like so:
    glUniform1i(glGetUniformLocation(ourShader.ID, "texturea"), 0);
    // or set it via the texture class
    ourShader.setInt("textureb", 1);


    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // input
        // -----
        processInput(window);

        // render
        // ------
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // bind textures on corresponding texture units
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        // set the texture mix value in the shader
        ourShader.setFloat("mixValue", mixValue);

        // render container
        ourShader.use();
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // optional: de-allocate all resources once they've outlived their purpose:
    // ------------------------------------------------------------------------
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);

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

    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        mixValue += 0.001f; // change this value accordingly (might be too slow or too fast based on system hardware)
        if(mixValue >= 1.0f)
            mixValue = 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        mixValue -= 0.001f; // change this value accordingly (might be too slow or too fast based on system hardware)
        if (mixValue <= 0.0f)
            mixValue = 0.0f;
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