#ifndef SHADER_H
#define SHADER_H

#include <string>
#include "GLFW/glfw3.h"
#include "glad/glad.h"

class Shader {
 public:
  void checkShaderErrors(GLuint shader_id, const std::string type);
  void compileShader();
  void use();
  void loadShaders(const char* vertexShaderFilePath,
                   const char* fragmentShaderFilePath);
  void loadVertexShader(const char* vertex_shader_code);
  void loadFragmentShader(const char* fragment_shader_code);
  // utility uniform functions
  void setBool(const std::string& name, bool value) const;
  void setInt(const std::string& name, int value) const;
  void setFloat(const std::string& name, float value) const;

 private:
  const char* m_vertex_shader_code;
  const char* m_fragment_shader_code;
  GLuint m_program_id;
};

#endif