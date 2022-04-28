#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <utility>

class Shader {
  public:
    unsigned int ID;
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    Shader(
        std::filesystem::path vertexPath,
        std::filesystem::path fragmentPath,
        std::filesystem::path geometryPath              = {},
        std::filesystem::path tessellationControlPath   = {},
        std::filesystem::path tesselationEvaluationPath = {}) {
        ID = glCreateProgram();
        for (auto& [type, name, path] :
             {std::tuple{GL_VERTEX_SHADER, "VERTEX", vertexPath},
              {GL_FRAGMENT_SHADER, "FRAGMENT", fragmentPath},
              {GL_GEOMETRY_SHADER, "GEOMETRY", geometryPath},
              {GL_TESS_EVALUATION_SHADER, "TESSELLATION CONTROL", tessellationControlPath},
              {GL_TESS_CONTROL_SHADER, "TESSELLATION EVALUATION", tesselationEvaluationPath}}) {
            if (!std::empty(path)) {
                auto code = std::string();
                try {
                    std::ifstream file(path);
                    file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
                    std::stringstream ss;
                    ss << file.rdbuf();
                    code = ss.str();
                } catch (std::ifstream::failure& ex) {
                    std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
                    std::cout << ex.what() << std::endl;
                }
                auto code_c_str = code.c_str();
                auto shader     = glCreateShader(type);
                glShaderSource(shader, 1, &code_c_str, NULL);
                glCompileShader(shader);
                checkCompileErrors(shader, name);
                glAttachShader(ID, shader);
                glDeleteShader(shader);
            }
        }
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM");
    }
    // activate the shader
    // ------------------------------------------------------------------------
    void use() { glUseProgram(ID); }
    // utility uniform functions
    // ------------------------------------------------------------------------
    void set(const std::string& name, bool value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
    }
    // ------------------------------------------------------------------------
    void set(const std::string& name, int value) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void set(const std::string& name, float value) const {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
    }
    // ------------------------------------------------------------------------
    void set(const std::string& name, const glm::vec2& value) const {
        glUniform2fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void set(const std::string& name, float x, float y) const {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), x, y);
    }
    // ------------------------------------------------------------------------
    void set(const std::string& name, const glm::vec3& value) const {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void set(const std::string& name, float x, float y, float z) const {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }
    // ------------------------------------------------------------------------
    void set(const std::string& name, const glm::vec4& value) const {
        glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void set(const std::string& name, float x, float y, float z, float w) {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), x, y, z, w);
    }
    // ------------------------------------------------------------------------
    void set(const std::string& name, const glm::mat2& mat) const {
        glUniformMatrix2fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void set(const std::string& name, const glm::mat3& mat) const {
        glUniformMatrix3fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    // ------------------------------------------------------------------------
    void set(const std::string& name, const glm::mat4& mat) const {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }

  private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(GLuint shader, std::string type) {
        GLint success;
        GLchar infoLog[1024];
        if (type != "PROGRAM") {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success) {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n"
                          << infoLog
                          << "\n -- --------------------------------------------------- -- "
                          << std::endl;
            }
        } else {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success) {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n"
                          << infoLog
                          << "\n -- --------------------------------------------------- -- "
                          << std::endl;
            }
        }
    }
};
