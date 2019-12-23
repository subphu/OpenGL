//  Copyright © 2019 Subph. All rights reserved.
//

#ifndef SHADER_H
#define SHADER_H

#define PROGRAM "PROGRAM"
#define VERTEX "VERTEX"
#define FRAGMENT "FRAGMENT"

#include <GL/glew.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader {
public:
    unsigned int ID;
    
    Shader() {
        ID = glCreateProgram();
    }
    
    ~Shader() {
        glDeleteProgram(ID);
    }
    
    void addShaderFrom(const char* path, GLuint type) {
        const char* code = readShaderCode(path).c_str();
        addShader(code, type);
    }
    
    void addShader(const char* code, GLuint type) {
        GLuint shader = glCreateShader(type);
        glShaderSource(shader, 1, &code, NULL);
        glCompileShader(shader);
        checkCompileErrors(shader, shader == GL_VERTEX_SHADER ? VERTEX : FRAGMENT);
        glAttachShader(ID, shader);
        glDeleteShader(shader);
    }
    
    void bindFragDataLocation(GLuint colorNumber, const char* name) {
        glBindFragDataLocation(ID, colorNumber, name);
    }
    
    void compile() {
        glLinkProgram(ID);
        checkCompileErrors(ID, PROGRAM);
    }
    
    void use() {
        glUseProgram(ID);
    }
    
    void setUniform1i(const std::string &name, int v0) const {
        glUniform1i(glGetUniformLocation(ID, name.c_str()), v0);
    }
    
    void setUniform1f(const std::string &name, float v0) const {
        glUniform1f(glGetUniformLocation(ID, name.c_str()), v0);
    }
    
    void setUniform2f(const std::string &name, float v0, float v1) const {
        glUniform2f(glGetUniformLocation(ID, name.c_str()), v0, v1);
    }
    
    void setUniform3f(const std::string &name, float v0, float v1, float v2) const {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), v0, v1, v2);
    }
    
    void setUniform4f(const std::string &name, float v0,float v1, float v2, float v3) const {
        glUniform4f(glGetUniformLocation(ID, name.c_str()), v0, v1, v2, v3);
    }
    
    void setUniformMatrix3fv(const std::string &name, const glm::mat3 &mat) const {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    
    void setUniformMatrix4fv(const std::string &name, const glm::mat4 &mat) const {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
    }
    
    GLint getAttribLocation(const std::string &name) {
        return glGetAttribLocation(ID, name.c_str());
    }

private:
    void checkCompileErrors(GLuint shader, std::string type) {
        int success;
        char infoLog[1024];
        
        if (type == PROGRAM) {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            
            if (!success) {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << std::endl;
            }
        } else {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            
            if (!success) {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << std::endl;
            }
        }
    }
    
    std::string readShaderCode(const char* path) {
        std::ifstream file;
        
        file.exceptions (std::ifstream::failbit | std::ifstream::badbit);
        try {
            file.open(path);
            std::stringstream stream;
            stream << file.rdbuf();
            file.close();
            return stream.str();
        } catch (std::ifstream::failure e) {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
            return NULL;
        }
    }
};

#endif /* Shader_h */
