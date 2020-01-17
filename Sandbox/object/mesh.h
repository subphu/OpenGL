//  Copyright © 2020 Subph. All rights reserved.
//

#pragma once

#define MESH_VERTEX 0
#define MESH_NORMAL 1
#define MESH_TEXTURE 2

#include <iostream>
#include <vector>

class Mesh {
    
public:
    std::vector<float> vertices;
    std::vector<float> normals;
    std::vector<float> texCoords;
    std::vector<int> indices;
    
    
    static unsigned long createSphere(std::vector<float> &vertices, std::vector<int> &indices, int stacks = 10, int sectors = 20, float radius = 1);
    static float * generateCube(unsigned long &memorySize, unsigned int details = MESH_VERTEX, float scale = 1);
    
private:
    
};
