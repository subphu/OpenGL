//  Copyright © 2020 Subph. All rights reserved.
//

#pragma once

#define MESH_VERTEX 0
#define MESH_NORMAL 1
#define MESH_TEXTURE 2

class Mesh {
    
public:
    
    static float * createCube(unsigned long &memorySize, unsigned int details = MESH_VERTEX, float scale = 1);
    
private:
    
};
