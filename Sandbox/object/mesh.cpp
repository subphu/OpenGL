//  Copyright © 2020 Subph. All rights reserved.
//

#include "mesh.h"

#include <iostream>
#include <vector>
#include <math.h>

float * Mesh::createCube(unsigned long &memorySize, unsigned int details, float scale) {
    float vertices[8][3] = {
        {-.5,  .5,  .5}, {-.5, -.5,  .5}, { .5,  .5,  .5}, { .5, -.5,  .5},
        { .5,  .5, -.5}, { .5, -.5, -.5}, {-.5,  .5, -.5}, {-.5, -.5, -.5}
    };
    unsigned int order[] = {
        6, 7, 0, 0, 7, 1,   2, 3, 4, 4, 3, 5,
        1, 7, 3, 3, 7, 5,   6, 0, 4, 4, 0, 2,
        4, 5, 6, 6, 5, 7,   0, 1, 2, 2, 1, 3
    };
    
    for (int i = 0; i < 24; i++) *(*vertices + i) *= scale;
    
    bool normal = details & MESH_NORMAL;
    bool texture = details & MESH_TEXTURE;
    unsigned int idx = 0;
    unsigned int size = 3 + (normal * 3) + (texture * 2);
    unsigned int totalSize = size * 36;
    float * data = (float*) calloc (totalSize, sizeof(float));
    
    for (int i = 0; i < 36; i++) {
        unsigned int verIdx = order[i];
        idx = i * size;
        data[idx++] = vertices[verIdx][0];
        data[idx++] = vertices[verIdx][1];
        data[idx++] = vertices[verIdx][2];
        if (normal) {
            int side = i / 6;
            int axis = side / 2;
            data[idx + axis] = side % 2 * 2 - 1;
            idx += 3;
        }
        if (texture) {
            int start = i * size;
            int axis = i / 12;
            if (axis == 0) data[idx++] = data[start + 1] > 0;
            else           data[idx++] = data[start    ] > 0;
            if (axis == 2) data[idx++] = data[start + 1] > 0;
            else           data[idx++] = data[start + 2] < 0;
        }
    }
    memorySize = sizeof(float) * totalSize;
    return data;
}


