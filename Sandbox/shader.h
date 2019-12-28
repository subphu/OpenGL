//  Copyright © 2019 Subph. All rights reserved.
//

#pragma once

#include <glm/glm.hpp>
#include <iostream>

class Shader {
public:
    unsigned int ID;
    
    Shader();
    ~Shader();
    
    void addShaderFrom(const char* path, unsigned int type);
    void addShader(const char* code, unsigned int type);
    
    void bindFragDataLocation(unsigned int colorNumber, const char* name);
    
    void compile();
    void use();
    
    void setUniform1i(const std::string &name, int v0) const;
    void setUniform1f(const std::string &name, float v0) const;
    void setUniform2f(const std::string &name, float v0, float v1) const;
    void setUniform3f(const std::string &name, float v0, float v1, float v2) const;
    void setUniform4f(const std::string &name, float v0,float v1, float v2, float v3) const;
    
    void setUniformMatrix3fv(const std::string &name, const glm::mat3 &mat) const;
    void setUniformMatrix4fv(const std::string &name, const glm::mat4 &mat) const;
    
    int getAttribLocation(const std::string &name) ;

private:
    
    void checkCompileErrors(unsigned int shader, std::string type);
    
    std::string readShaderCode(const char* path);
    
};
