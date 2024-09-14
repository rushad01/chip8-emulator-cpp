#include <stdint.h>
#include <cstdio>
#include <iostream>
#include <vector>
#include "GLFW/glfw3.h"
#include "glad/glad.h"

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glm/ext/vector_float3.hpp"
#include "glm/glm.hpp"

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"

#include "chip8.h"
#include "shader.h"

#define SCREEN_WIDTH 64
#define SCREEN_HEIGHT 32

//Selecting GPU.  If you don't have gpu comment this part.
#ifdef __cplusplus
extern "C" {
#endif

__declspec(dllexport) uint32_t NvOptimusEnablement = 1;
__declspec(dllexport) int AmdPowerXpressRequestHighPerformance = 1;

#ifdef __cplusplus
}
#endif
//end of gpu selection.

chip8 myChip8;
Shader shader;

struct Vertex {
  float x, y, z;
};

// CallBack function for various event handling and helper methods
void window_size_callback(GLFWwindow* window, int width, int height);
static void error_callback(int error, const char* description);
static void keypress_callback(GLFWwindow* window, int key, int scancode,
                              int action, int mods);
void drawPixel(int x, int y);
void updateQuads(const chip8& Chip8);
void openglInformation();

int main(int argc, char** argv) {

  // Chip8 initializatio
  if (argc < 2) {
    std::cout << "chip8-emulator-cpp.exe uses: \n"
              << argv[0] << " path/to/chip8/program\n";
    return 1;
  }
  myChip8.initialize();
  if (!myChip8.load_game(argv[1])) {
    std::cout << "chip8-emulator-cpp.exe uses: \n"
              << argv[1] << " path/to/chip8/program\n";
    return 1;
  }
  //Error Callback function for use
  glfwSetErrorCallback(error_callback);

  glfwInit();
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  GLFWwindow* window = glfwCreateWindow(SCREEN_WIDTH * 10, SCREEN_HEIGHT * 10,
                                        "CHIP8 Emulator", NULL, NULL);
  if (window == NULL) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    return -1;
  }
  //window resize callback
  glfwSetWindowSizeCallback(window, window_size_callback);

  //Keyprocessing for the windows
  glfwSetKeyCallback(window, keypress_callback);

  //GLFW Context
  glfwMakeContextCurrent(window);

  glfwSwapInterval(1);  // Enable vsync

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cout << "Failed to initialize OpenGL context" << std::endl;
    return -1;
  }
  shader.loadShaders("../resources/shader.vert", "../resources/shader.frag");
  //Video Card Information
  //This call must be after the context is set and also after all function from glad are loaded
  openglInformation();
  shader.compileShader();

  // Main loop
  while (!glfwWindowShouldClose(window)) {
    glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    myChip8.emulate_cycle();
    if (myChip8.drawFlag == true) {
      updateQuads(myChip8);
      myChip8.drawFlag = false;
      glfwSwapBuffers(window);
    }
    glfwPollEvents();
  }

  glfwDestroyWindow(window);
  glfwTerminate();

  return 0;
}

void window_size_callback(GLFWwindow* window, int width, int height) {

  glViewport(0, 0, width, height);  // Change the view port if needed
}

// graphics api(OpenGL) keyprocessing
static void keypress_callback(GLFWwindow* window, int key, int scancode,
                              int action, int mods) {
  if (action == GLFW_PRESS) {
    switch (key) {
      case GLFW_KEY_ESCAPE:
        glfwSetWindowShouldClose(window, GLFW_TRUE);
        break;

      case GLFW_KEY_1:
        myChip8.key[0x1] = 0;
        break;

      case GLFW_KEY_2:
        myChip8.key[0x2] = 0;
        break;

      case GLFW_KEY_3:
        myChip8.key[0x3] = 0;
        break;

      case GLFW_KEY_4:
        myChip8.key[0xC] = 0;
        break;

      case GLFW_KEY_Q:
        myChip8.key[0x4] = 0;
        break;

      case GLFW_KEY_W:
        myChip8.key[0x5] = 0;
        break;

      case GLFW_KEY_E:
        myChip8.key[0x6] = 0;
        break;

      case GLFW_KEY_R:
        myChip8.key[0xD] = 0;
        break;

      case GLFW_KEY_A:
        myChip8.key[0x7] = 0;
        break;

      case GLFW_KEY_S:
        myChip8.key[0x8] = 0;
        break;

      case GLFW_KEY_D:
        myChip8.key[0x9] = 0;
        break;

      case GLFW_KEY_F:
        myChip8.key[0xE] = 0;
        break;

      case GLFW_KEY_Z:
        myChip8.key[0xA] = 0;
        break;

      case GLFW_KEY_X:
        myChip8.key[0x0] = 0;
        break;

      case GLFW_KEY_C:
        myChip8.key[0xB] = 0;
        break;

      case GLFW_KEY_V:
        myChip8.key[0xF] = 0;
        break;

      default:
        break;
    }
  }
}

void drawPixel(int x, int y) {
  std::vector<glm::vec3> vertices;
  vertices.resize(4);
  vertices[0] =
      glm::vec3((x * 10.0) + 10.0f, (y * 10.0) + 10.0, 0.0f);  //top right
  vertices[1] =
      glm::vec3((x * 10.0) + 10.0f, (y * 10.0f), 0.0f);          //bottom right
  vertices[2] = glm::vec3((x * 10.0), (y * 10.0f), 0.0f);        //bottom left
  vertices[3] = glm::vec3((x * 10.0), (y * 10.0) + 10.0, 0.0f);  //top left
  //Orthogonal projection for window coordinate
  glm::mat4 projection = glm::ortho(0.0f, SCREEN_WIDTH * 10.0f,
                                    SCREEN_HEIGHT * 10.0f, 0.0f, -1.0f, 1.0f);
  for (auto& vertex : vertices)
    vertex = glm::vec3(projection * glm::vec4(vertex, 1.0f));
  unsigned int indices[] = {
      // note that we start from 0!
      0, 1, 3,  // first Triangle
      1, 2, 3   // second Triangle
  };
  unsigned int VBO, VAO, EBO;
  glGenVertexArrays(1, &VAO);
  glGenBuffers(1, &VBO);
  glGenBuffers(1, &EBO);
  // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
  glBindVertexArray(VAO);

  glBindBuffer(GL_ARRAY_BUFFER, VBO);
  glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(glm::vec3),
               vertices.data(), GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices,
               GL_STATIC_DRAW);

  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);

  glBindVertexArray(0);
  shader.use();
  glBindVertexArray(VAO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void updateQuads(const chip8& Chip8) {
  // cycle through VRAM and draw every pixel
  for (unsigned int y = 0; y < 32; y++) {
    for (unsigned int x = 0; x < 64; x++) {
      if (Chip8.gfx[x + (y * 64)])
        drawPixel(x, y);
    }
  }
}

//Video Card Information
//This call must be after the context is set and also after all function from glad are loaded
void openglInformation() {
  const GLubyte* vendor = glGetString(GL_VENDOR);  //Returns the vendor
  const GLubyte* renderer =
      glGetString(GL_RENDERER);  // Returns a hint to the model

  std::cout << "Vendor: " << (char*)vendor << "\n"
            << "Renderer: " << (char*)renderer << "\n"
            << "OpenGL Version: " << (char*)glGetString(GL_VERSION) << "\n"
            << "Shading Language Version: "
            << (char*)glGetString(GL_SHADING_LANGUAGE_VERSION) << "\n";
}

// opengl error callback
static void error_callback(int error, const char* description) {
  fprintf(stderr, "Errors: %s \n", description);
}