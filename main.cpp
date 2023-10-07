#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>
#include <shader.h>

static uint32_t ss_id = 0;
const int SCR_WIDTH = 1024;
const int SCR_HEIGHT = 768;

void dumpFramebufferToPPM(std::string prefix, uint32_t width, uint32_t height);

void processInput(GLFWwindow *window);

void framebufferSizeCallback(GLFWwindow *window, int width, int height);

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow *window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "Solar System", NULL, NULL);

    if (window == NULL) {
        std::cout << "GLFW Window Failed" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);

    if (!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::cout << "GLAD Initialization Failed" << std::endl;
        return -1;
    }

    // configure global openGL state
    glEnable(GL_DEPTH_TEST);

    // build and compile shader program
    Shader shader("shaders/shader.vs", "shaders/shader.fs");

    // cube vertices
    float vertices[] = {
            // back face, yellow
            -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 0.0f,
            1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 0.0f,
            -1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 0.0f,

            // front face, purple
            -1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
            1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
            -1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 1.0f,

            // right face, green
            -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
            -1.0f, 1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
            -1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
            -1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,

            // left face, red
            1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
            1.0f, 1.0f, -1.0f, 1.0f, 0.0f, 0.0f,
            1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f,
            1.0f, -1.0f, -1.0f, 1.0f, 0.0f, 0.0f,
            1.0f, -1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
            1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

            // bottom face, light blue
            -1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
            1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
            -1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f,
            -1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 1.0f,

            // top face, blue
            -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
            1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, 1.0f, -1.0f, 0.0f, 0.0f, 1.0f,
    };

    uint32_t VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // bind Vertex Array Object
    glBindVertexArray(VAO);

    // bind vertices array to a vertex buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);

    // color attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *) (3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        // background color
        glClearColor(0.3f, 0.4f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // activate shader
        shader.use();

        glm::mat4 sun, earth, moon;

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 proj = glm::mat4(1.0f);

        view = glm::lookAt(glm::vec3(0.0f, 120.0f, 1.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        proj = glm::perspective(glm::radians(30.0f), (float) 4 / (float) 3, 0.1f, 1000.0f);

        shader.setMat4("view", view);
        shader.setMat4("projection", proj);

        //render container
        glBindVertexArray(VAO);

        sun = glm::scale(model, glm::vec3(12.0f, 12.0f, 12.0f));
        shader.setMat4("model", sun);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        earth = glm::translate(model, glm::vec3(24.0f, 0.0f, 0.0f));
        earth = glm::scale(earth, glm::vec3(6.0f, 6.0f, 6.0f));
        earth = glm::rotate(earth, (float)glm::radians(-23.4), glm::vec3(0.0f, 0.0f, 1.0f));
        shader.setMat4("model", earth);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        moon = glm::translate(model, glm::vec3(36.0f, 0.0f, 0.0f));
        moon = glm::scale(moon, glm::vec3(3.0f, 3.0f, 3.0f));
        shader.setMat4("model", moon);
        glDrawArrays(GL_TRIANGLES, 0, 36);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //release resource
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);

    glfwTerminate();
    return 0;
}

void framebufferSizeCallback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window) {
    //press escape to exit
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    //press p to capture screen
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        std::cout << "Capture Window " << ss_id << std::endl;
        int buffer_width, buffer_height;
        glfwGetFramebufferSize(window, &buffer_width, &buffer_height);
        dumpFramebufferToPPM("Assignment0-ss", buffer_width, buffer_height);
    }
}

void dumpFramebufferToPPM(std::string prefix, uint32_t width, uint32_t height) {
    int pixelChannel = 3;
    int totalPixelSize = pixelChannel * width * height * sizeof(GLubyte);
    GLubyte *pixels = new GLubyte[totalPixelSize];
    glReadPixels(0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);

    std::string fileName = prefix + std::to_string(ss_id) + ".ppm";
    std::filesystem::path filePath = std::filesystem::current_path() / fileName;
    std::ofstream fout(filePath.string());

    fout << "P3\n" << width << " " << height << "\n" << 255 << std::endl;
    for (size_t i = 0; i < height; i++) {
        for (size_t j = 0; j < width; j++) {
            size_t cur = pixelChannel * ((height - i - 1) * width + j);
            fout << (int) pixels[cur] << " " << (int) pixels[cur + 1] << " " << (int) pixels[cur + 2] << " ";
        }
        fout << std::endl;
    }

    ss_id++;

    delete[] pixels;
    fout.flush();
    fout.close();
}
