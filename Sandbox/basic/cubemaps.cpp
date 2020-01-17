//  Copyright © 2020 Subph. All rights reserved.
//

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <cmath>
#include <unistd.h>

#include "basic.h"
#include "../define.h"
#include "../shader.h"
#include "../camera.h"
#include "../system/system.h"
#include "../libraries/stb_image.h"
#include "../object/mesh.h"

unsigned int loadCubemap(std::string *faces);
void renderSphere();

void runCubemaps() {
    glEnable(GL_DEPTH_TEST);
    
    System &system = System::instance();
    Size<int> size = system.getFramebufferSize();
    float ratio = (float)size.width / (float)size.height;
    
    Shader shader = Shader();
    shader.addShaderFrom("shader/basic/cubemap.vert", GL_VERTEX_SHADER);
    shader.addShaderFrom("shader/basic/cubemap.frag", GL_FRAGMENT_SHADER);
    shader.compile();
    
    Shader skyboxShader = Shader();
    skyboxShader.addShaderFrom("shader/basic/skybox.vert", GL_VERTEX_SHADER);
    skyboxShader.addShaderFrom("shader/basic/skybox.frag", GL_FRAGMENT_SHADER);
    skyboxShader.compile();
    
    std::vector<float> vertices;
    std::vector<int> indices;
    unsigned long sphereSize = Mesh::createSphere(vertices, indices, 20, 40);

    LOG indices.size() ENDL;
    unsigned int sphereVAO, sphereVBO, sphereEBO;
    glGenVertexArrays(1, &sphereVAO);
    glBindVertexArray(sphereVAO);
    glGenBuffers(1, &sphereVBO);
    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    glBufferData(GL_ARRAY_BUFFER, sphereSize, &vertices[0], GL_STATIC_DRAW);
    glGenBuffers(1, &sphereEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(int) * indices.size(), &indices[0], GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    unsigned long skyboxVerticesSize;
    float * skyboxVertices = Mesh::createCube(skyboxVerticesSize, MESH_VERTEX, 2);
    
    GLuint skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glBindVertexArray(skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, skyboxVerticesSize, skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    
    std::string faces[] = {
        "resources/skybox/lake/right.jpg",
        "resources/skybox/lake/left.jpg",
        "resources/skybox/lake/top.jpg",
        "resources/skybox/lake/bottom.jpg",
        "resources/skybox/lake/front.jpg",
        "resources/skybox/lake/back.jpg"
    };
    unsigned int cubemapTexture = loadCubemap(faces);

    shader.use();
    shader.setUniform1i("skybox", 0);
    shader.setUniform1i("reflection", 1);

    skyboxShader.use();
    skyboxShader.setUniform1i("skybox", 0);

    float lag = 0;
    float lastTime = system.getTime();
    float frameDelay = 1.f/60.f;
    
    float refractRatio = 1 / 1.33;
    int reflection = 0;
    bool prevKeyZ = false;

    Camera camera(glm::vec3(0.0f, 2.0f, 8.0f));
    
    system.disableCursor();

    while (system.getWindowState()) {
        if (system.getKeyState(key_esc)) system.closeWindow();
        
        lag += system.getTime() - lastTime;
        lastTime = system.getTime();
        
        if (lag < frameDelay) {
            usleep((frameDelay - lag) * 1000000);
            lag += system.getTime() - lastTime;
            lastTime = system.getTime();
        }
        
        while (lag > frameDelay) {
            glm::vec3 movement = glm::vec3(0.f, 0.f, 0.f);
            movement.x = system.getKeyState(key_d) - system.getKeyState(key_a);
            movement.y = system.getKeyState(key_q) - system.getKeyState(key_e);
            movement.z = system.getKeyState(key_w) - system.getKeyState(key_s);
            camera.move(movement);
            
            glm::vec2 delta = system.getCursorMovement();
            camera.turn(delta);
            
            bool currentKeyZ = system.getKeyState(key_z);
            reflection = prevKeyZ && !currentKeyZ ? !reflection : reflection;
            prevKeyZ = currentKeyZ;
            
            lag -= frameDelay;
        }
        
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        shader.use();
        shader.setUniformMatrix4fv("view", camera.getViewMatrix());
        shader.setUniformMatrix4fv("projection", camera.getProjection(ratio));
        shader.setUniform1i("reflection", reflection);
        shader.setUniform1f("refractRatio", refractRatio);
        shader.setUniform3f("viewPos", camera.getPosition());
        shader.setUniformMatrix4fv("model", glm::mat4(1.0f));
        
        glBindVertexArray(sphereVAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        
        glDepthFunc(GL_LEQUAL);
        skyboxShader.use();
        skyboxShader.setUniformMatrix4fv("view", glm::mat4(glm::mat3(camera.getViewMatrix())));
        skyboxShader.setUniformMatrix4fv("projection", camera.getProjection(ratio));
        
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glDepthFunc(GL_LESS);
        
        system.swapBuffer();
        system.pollEvents();
    }
    
    glDeleteVertexArrays(1, &sphereVAO);
    glDeleteVertexArrays(1, &skyboxVAO);
    glDeleteBuffers(1, &sphereEBO);
    glDeleteBuffers(1, &sphereVBO);
    glDeleteBuffers(1, &skyboxVBO);
}

// loads a cubemap texture from 6 individual texture faces
// +X (right), -X (left), +Y (top), -Y (bottom), +Z (front), -Z (back)
unsigned int loadCubemap(std::string *faces) {
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

    int width, height, colorChannels;
    for (int i = 0; i < 6; i++) {
        unsigned char *data = stbi_load(faces[i].c_str(), &width, &height, &colorChannels, 0);
        if (!data) {
            LOG "Cubemap texture failed to load at path:" SPACE faces[i] ENDL;
            stbi_image_free(data);
            return texture;
        }

        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }
    
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return texture;
}
