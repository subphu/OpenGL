//  Copyright © 2020 Subph. All rights reserved.
//

#include <glm/glm.hpp>
#include <vector>
#include <math.h>

#include "mesh.h"
#include "define.h"

float * Mesh::generateCube(unsigned long &memorySize, unsigned int details, float scale) {
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


void Mesh::createSphere(int wedge, int segment) {
    float x, y, z, xz;
    float s, t;

    float segmentStep = -2 * PI * 1 / segment;  // counter-clockwise
    float wedgeStep = PI * 1 / wedge;
    float segmentAngle, wedgeAngle;

    for(int i = 1; i < wedge; i++) {
        wedgeAngle = i * wedgeStep;             // starting from 0 to pi
        y  = cosf(wedgeAngle);                  // r * cos(u)
        xz = sinf(wedgeAngle);                  // r * sin(u)

        for(int j = 0; j < segment; j++) {
            segmentAngle = j * segmentStep;     // starting from 0 to 2pi
            x = xz * cosf(segmentAngle);        // r * sin(u) * cos(v)
            z = xz * sinf(segmentAngle);        // r * sin(u) * sin(v)
            
            vertices.insert(vertices.end(), { x, y, z });
            normals .insert(normals .end(), { x, y, z });

            s = (float)j / segment;             // vertex tex coord (s, t)
            t = (float)i / wedge;               // range between [0, 1]
            texCoords.insert(texCoords.end(), { s, t });
        }
    }
    vertices.insert(vertices.end(), { 0, 1, 0 });
    normals .insert(normals .end(), { 0, 1, 0 });
    
    vertices.insert(vertices.end(), { 0, -1, 0 });
    normals .insert(normals .end(), { 0, -1, 0 });
    
    int w1, w2;
    int last   = (wedge-1) * segment - 1;
    int top    = last + 1;
    int bottom = last + 2;
    for(int i = 0; i < segment; i++) {
        int d = (i + 1) % segment;
        indices.insert(indices.end(), { top   , i     , d      });
        indices.insert(indices.end(), { bottom, last-i, last-d });
    }
    for(int i = 0; i < wedge-2; i++) {
        w1 = i  * segment;
        w2 = w1 + segment;

        for(int j = 0; j < segment; j++) {
            int d = (j + 1) % segment;
            indices.insert(indices.end(), { w1+j, w2+j, w1+d });
            indices.insert(indices.end(), { w1+d, w2+j, w2+d });
        }
    }
}

unsigned long Mesh::sizeofVertices() { return sizeof(float) * vertices.size(); }
unsigned long Mesh::sizeofNormals() { return sizeof(float) * normals.size(); }
unsigned long Mesh::sizeofTexCoords() { return sizeof(float) * texCoords.size(); }
unsigned long Mesh::sizeofIndices() { return sizeof(int) * indices.size(); }
