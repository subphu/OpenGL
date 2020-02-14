//  Copyright © 2020 Subph. All rights reserved.
//

#pragma once

#define MESH_VERTEX 0
#define MESH_NORMAL 1
#define MESH_TEXTURE 2

#include <iostream>
#include <vector>
#include <glm/glm.hpp>

class Mesh {
    
public:
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<float> texCoords;
    std::vector<int> indices;
    
    static float * generateCube(unsigned long &memorySize, unsigned int details = MESH_VERTEX, float scale = 1);
    
    void createCube();
    void createSphere(int wedge = 10, int segment = 20);
    
    void genBuffer();
    void draw();
    
    void scale(glm::vec3 size);
    void rotate(float angle, glm::vec3 axis);
    void translate(glm::vec3 translation);
    
    glm::mat4 getMatrix();
    
    unsigned long sizeofVertices();
    unsigned long sizeofNormals();
    unsigned long sizeofTexCoords();
    unsigned long sizeofIndices();
    
private:
    unsigned int vao, vbo, ebo;
    glm::mat4 model = glm::mat4(1.0f);
    
};
