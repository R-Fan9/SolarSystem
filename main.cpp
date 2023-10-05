#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <filesystem>

static unsigned int ss_id = 0;

void dumpFramebufferToPPM(std::string prefix, unsigned int width, unsigned int height);

void processInput(GLFWwindow *window);

void framebufferSizeCallback(GLFWwindow *window, int width, int height);

unsigned int createVertexShader(int *success);

unsigned int createFragmentShader(int *success);

unsigned int createShaderProgram(unsigned int vertexShader, unsigned int fragmentShader, int *success);

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    int windowWidth = 512;
    int windowHeight = 512;
    GLFWwindow *window = glfwCreateWindow(windowWidth, windowHeight, "Assignment0", NULL, NULL);

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

    //shaders
    int success;
    char errorMsg[512];
    unsigned int vs = createVertexShader(&success);
    if (!success) {
        glGetShaderInfoLog(vs, 512, NULL, errorMsg);
        std::cout << "Vertex Shader Failed: " << errorMsg << std::endl;
    }

    unsigned int fs = createFragmentShader(&success);
    if (!success) {
        glGetShaderInfoLog(fs, 512, NULL, errorMsg);
        std::cout << "Fragment Shader Failed: " << errorMsg << std::endl;
    }

    unsigned int shaderProgram = createShaderProgram(vs, fs, &success);
    if (!success) {
        glGetProgramInfoLog(shaderProgram, 512, NULL, errorMsg);
        std::cout << "Program Link Error: " << errorMsg << std::endl;
    }

    glDeleteShader(vs);
    glDeleteShader(fs);

    //triangle vertex positions
    float tri_verts[] = {
            -0.8f, -0.8f, 0.0f, //bottom left
            0.8f, -0.8f, 0.0f, //bottom right
            0.0f, 0.8f, 0.0f  //top middle
    };

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(tri_verts), tri_verts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *) 0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    while (!glfwWindowShouldClose(window)) {
        processInput(window);

        //background color
        glClearColor(0.3f, 0.4f, 0.5f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        //draw things
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    //release resource
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    glfwTerminate();
    return 0;
}

unsigned int createVertexShader(int *success) {
    const char *vertexShaderSource = "#version 330 core\n"
                                     "layout (location = 0) in vec3 aPos;\n"
                                     "void main()\n"
                                     "{\n"
                                     "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n"
                                     "}\0";

    unsigned int vs = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vs, 1, &vertexShaderSource, NULL);
    glCompileShader(vs);
    glGetShaderiv(vs, GL_COMPILE_STATUS, success);
    return vs;
}

unsigned int createFragmentShader(int *success) {
    const char *fragmentShaderSource = "#version 330 core\n"
                                       "out vec4 FragColor;\n"
                                       "void main()\n"
                                       "{\n"
                                       "   FragColor = vec4(0.9f, 0.8f, 0.7f, 1.0f);\n" //triangle color green
                                       "}\n\0";

    unsigned int fs = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fs, 1, &fragmentShaderSource, NULL);
    glCompileShader(fs);
    glGetShaderiv(fs, GL_COMPILE_STATUS, success);
    return fs;
}

unsigned int createShaderProgram(unsigned int vertexShader, unsigned int fragmentShader, int *success) {
    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, success);

    return shaderProgram;
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

void dumpFramebufferToPPM(std::string prefix, unsigned int width, unsigned int height) {
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
