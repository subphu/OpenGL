//  Copyright © 2019 Subph. All rights reserved.
//

#include <glm/glm.hpp>

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include "shader.h"
#include "define.h"

#define PROGRAM "PROGRAM"
#define VERTEX "VERTEX"
#define FRAGMENT "FRAGMENT"

Shader::Shader() {
    ID = glCreateProgram();
}

Shader::~Shader() {
    glDeleteProgram(ID);
}

void Shader::addShaderFrom(const char* path, unsigned int type) {
    std::string str = readShaderCode(path);
    addShader(str.c_str(), type);
}

void Shader::addShader(const char* code, unsigned int type) {
    unsigned int shader = glCreateShader(type);
    glShaderSource(shader, 1, &code, NULL);
    glCompileShader(shader);
    checkCompileErrors(shader, type == GL_VERTEX_SHADER ? VERTEX : FRAGMENT);
    glAttachShader(ID, shader);
    glDeleteShader(shader);
}

void Shader::bindFragDataLocation(unsigned int colorNumber, const char* name) {
    glBindFragDataLocation(ID, colorNumber, name);
}

void Shader::compile() {
    glLinkProgram(ID);
    checkCompileErrors(ID, PROGRAM);
}

void Shader::use() {
    glUseProgram(ID);
}

void Shader::setUniform1i(const std::string &name, int v0) const {
    glUniform1i(glGetUniformLocation(ID, name.c_str()), v0);
}

void Shader::setUniform1f(const std::string &name, float v0) const {
    glUniform1f(glGetUniformLocation(ID, name.c_str()), v0);
}

void Shader::setUniform2f(const std::string &name, float v0, float v1) const {
    glUniform2f(glGetUniformLocation(ID, name.c_str()), v0, v1);
}

void Shader::setUniform3f(const std::string &name, float v0, float v1, float v2) const {
    glUniform3f(glGetUniformLocation(ID, name.c_str()), v0, v1, v2);
}

void Shader::setUniform4f(const std::string &name, float v0,float v1, float v2, float v3) const {
    glUniform4f(glGetUniformLocation(ID, name.c_str()), v0, v1, v2, v3);
}

void Shader::setUniform3f(const std::string &name, glm::vec3 vec) const {
    glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &vec[0]);
}

void Shader::setUniform4f(const std::string &name, glm::vec4 vec) const{
    glUniform4fv(glGetUniformLocation(ID, name.c_str()), 1, &vec[0]);
}

void Shader::setUniformMatrix3fv(const std::string &name, const glm::mat3 &mat) const {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setUniformMatrix4fv(const std::string &name, const glm::mat4 &mat) const {
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

int Shader::getAttribLocation(const std::string &name) {
    return glGetAttribLocation(ID, name.c_str());
}

void Shader::checkCompileErrors(unsigned int shader, std::string type) {
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

std::string Shader::readShaderCode(const char* path) {
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
