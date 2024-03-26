#include "shader.h"
#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <streambuf>
#include <string>
#include <vector>

void Shader::compileShader() {
  unsigned int vertex, fragment;
  // vertex shader
  vertex = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vertex, 1, &m_vertex_shader_code, NULL);
  glCompileShader(vertex);
  checkShaderErrors(vertex, "VERTEX");
  // fragment Shader
  fragment = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(fragment, 1, &m_fragment_shader_code, NULL);
  glCompileShader(fragment);
  checkShaderErrors(fragment, "FRAGMENT");
  // shader Program
  m_program_id = glCreateProgram();
  glAttachShader(m_program_id, vertex);
  glAttachShader(m_program_id, fragment);
  glLinkProgram(m_program_id);
  checkShaderErrors(m_program_id, "PROGRAM");
  // delete the shaders as they're linked into our program now and no longer necessary
  glDeleteShader(vertex);
  glDeleteShader(fragment);
}

void Shader::checkShaderErrors(GLuint id, const std::string type) {
  int success;
  char infoLog[1024];
  if (type != "PROGRAM") {
    glGetShaderiv(id, GL_COMPILE_STATUS, &success);
    if (!success) {
      glGetShaderInfoLog(id, 1024, NULL, infoLog);
      std::cout
          << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
          << infoLog
          << "\n -- --------------------------------------------------- -- "
          << std::endl;
    }
  } else {
    glGetProgramiv(id, GL_LINK_STATUS, &success);
    if (!success) {
      glGetProgramInfoLog(id, 1024, NULL, infoLog);
      std::cout
          << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
          << infoLog
          << "\n -- --------------------------------------------------- -- "
          << std::endl;
    }
  }
}

void Shader::use() {
  glUseProgram(m_program_id);
}

void Shader::loadShaders(const char* vertexShaderFilePath,
                         const char* fragmentShaderFilePath) {
  // retrieve the vertex/fragment source code from filePath
  std::string vertexCode;
  std::string fragmentCode;
  std::ifstream vShaderFile;
  std::ifstream fShaderFile;
  // ensure ifstream objects can throw exceptions:
  vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
  try {
    // open files
    vShaderFile.open(vertexShaderFilePath);
    fShaderFile.open(fragmentShaderFilePath);
    std::stringstream vShaderStream, fShaderStream;
    // read file's buffer contents into streams
    vShaderStream << vShaderFile.rdbuf();
    fShaderStream << fShaderFile.rdbuf();
    // close file handlers
    vShaderFile.close();
    fShaderFile.close();
    // convert stream into string
    vertexCode = vShaderStream.str();
    fragmentCode = fShaderStream.str();
  } catch (std::ifstream::failure& e) {
    std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what()
              << std::endl;
  }
  //convert into c string
  m_vertex_shader_code = vertexCode.c_str();
  m_fragment_shader_code = fragmentCode.c_str();
}

void Shader::loadVertexShader(const char* vertex_shader_code) {
  this->m_vertex_shader_code = vertex_shader_code;
}

void Shader::loadFragmentShader(const char* fragment_shader_code) {
  this->m_fragment_shader_code = fragment_shader_code;
}

void Shader::setBool(const std::string& name, bool value) const {
  glUniform1i(glGetUniformLocation(m_program_id, name.c_str()), (int)value);
}
void Shader::setInt(const std::string& name, int value) const {
  glUniform1i(glGetUniformLocation(m_program_id, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const {
  glUniform1f(glGetUniformLocation(m_program_id, name.c_str()), value);
}