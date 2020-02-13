//  Copyright © 2020 Subph. All rights reserved.
//

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <vector>
#include <cmath>
#include <unistd.h>

#include "tutorial.h"
#include "../define.h"
#include "../shader.h"
#include "../camera.h"
#include "../system/system.h"
#include "../libraries/stb_image.h"
#include "../object/mesh.h"

void runInstancing() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glEnable(GL_FRAMEBUFFER_SRGB);
    
    System &system = System::instance();
    Size<int> size = system.getFramebufferSize();
    float ratio = (float)size.width / (float)size.height;
    
    Shader shader = Shader();
    shader.addShaderFrom("shader/tutorial/instancing.vert", GL_VERTEX_SHADER);
    shader.addShaderFrom("shader/tutorial/instancing.frag", GL_FRAGMENT_SHADER);
    shader.compile();
    
    Shader lightShader = Shader();
    lightShader.addShaderFrom("shader/tutorial/light.vert", GL_VERTEX_SHADER);
    lightShader.addShaderFrom("shader/tutorial/light.frag", GL_FRAGMENT_SHADER);
    lightShader.compile();
        
    Mesh sphere = Mesh();
    sphere.createSphere(25, 50);

    unsigned int sphereVAO, sphereVBO, sphereEBO;
    glGenVertexArrays(1, &sphereVAO);
    glBindVertexArray(sphereVAO);
    glGenBuffers(1, &sphereVBO);
    glBindBuffer(GL_ARRAY_BUFFER, sphereVBO);
    glBufferData(GL_ARRAY_BUFFER, sphere.sizeofVertices() + sphere.sizeofNormals(), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sphere.sizeofVertices(), &sphere.vertices[0]);
    glBufferSubData(GL_ARRAY_BUFFER, sphere.sizeofVertices(), sphere.sizeofNormals(), &sphere.normals[0]);
    glGenBuffers(1, &sphereEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphere.sizeofIndices(), &sphere.indices[0], GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(sphere.sizeofVertices()));
    
    
    unsigned long cubeVerticesSize;
    float * cubeVertices = Mesh::generateCube(cubeVerticesSize, (MESH_NORMAL | MESH_TEXTURE));

    unsigned int cubeVAO, cubeVBO;
    glGenVertexArrays(1, &cubeVAO);
    glBindVertexArray(cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, cubeVerticesSize, cubeVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));

    unsigned int amount = 100000;
    glm::mat4* modelMatrices = new glm::mat4[amount];
    srand(system.getTime());
    float radius = 160.0;
    float offset = 50.0f;
    int max = 2 * offset;
    for (unsigned int i = 0; i < amount; i++) {
        glm::mat4 model = glm::mat4(1.0f);
        
        float angle = (float)i / (float)amount * 360.0f;
        float x = ((rand() % max) - offset + radius) * sin(angle);
        float y = (rand() % max) - offset;
        float z = ((rand() % max) - offset + radius) * cos(angle);
        model = glm::translate(model, glm::vec3(x, y, z));
        
        float sx = (rand() % 30) / 20.0f + 0.2;
        float sy = (rand() % 30) / 20.0f + 0.2;
        float sz = (rand() % 30) / 20.0f + 0.2;
        model = glm::scale(model, glm::vec3(sx, sy, sz));

        float rotAngle = (rand() % 360);
        model = glm::rotate(model, rotAngle, glm::vec3(0.4f, 0.6f, 0.8f));

        modelMatrices[i] = model;
    }
    
    unsigned int buffer;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    glBufferData(GL_ARRAY_BUFFER, amount * sizeof(glm::mat4), &modelMatrices[0], GL_STATIC_DRAW);
    
    glBindVertexArray(cubeVAO);
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
    glEnableVertexAttribArray(5);
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));

    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);
    
    float speed[amount];
    for (unsigned int i = 0; i < amount; i++) {
        speed[i] = (rand() % 10 + 2) / 160.0;
    }
    
    unsigned int speedBuffer;
    glGenBuffers(1, &speedBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, speedBuffer);
    glBufferData(GL_ARRAY_BUFFER, amount * sizeof(float), &speed[0], GL_STATIC_DRAW);
    
    glBindVertexArray(cubeVAO);
    glEnableVertexAttribArray(7);
    glVertexAttribPointer(7, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
    glVertexAttribDivisor(7, 1);
    
    
    shader.use();

    float lag = 0;
    float lastTime = system.getTime();
    float frameDelay = 1.f/60.f;
    float sphereSize = 20;
    
    Camera camera(glm::vec3(0.0f, 2.0f, 100.0f));
    camera.setSpeed(2.0);
    
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
            
            lag -= frameDelay;
        }
        
        glClearColor(0.01f, 0.01f, 0.01f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        shader.use();
        shader.setUniformMatrix4fv("view", camera.getViewMatrix());
        shader.setUniformMatrix4fv("projection", camera.getProjection(ratio));
        shader.setUniform3f("viewPos", camera.getPosition());
        shader.setUniform1f("time", system.getTime());
        
        
        shader.setUniform3f("material.ambient",  1.0f, 1.0f, 1.0f);
        shader.setUniform3f("material.diffuse",  1.1f, 1.1f, 1.1f);
        shader.setUniform3f("material.specular", 0.5f, 0.5f, 0.5f);
        shader.setUniform1f("material.shininess", 128.0f);
        
        shader.setUniform1i("totalPointLights", 3);
        
        glm::vec3 color = glm::vec3(.99, .88, .77);
        for(int i = 0; i < 3; i++) {
            std::string idx = std::to_string(i);
            shader.setUniform3f("pointLights[" + idx + "].position",glm::vec3(0.0, i*sphereSize - sphereSize, 0.0));
            shader.setUniform3f("pointLights[" + idx + "].light.ambient", color * 0.05f);
            shader.setUniform3f("pointLights[" + idx + "].light.diffuse", color * 0.7f);
            shader.setUniform3f("pointLights[" + idx + "].light.specular", color);
            shader.setUniform1f("pointLights[" + idx + "].constant.linear", 0.005);
            shader.setUniform1f("pointLights[" + idx + "].constant.quadratic", 0.00005);
        }
        
        glBindVertexArray(cubeVAO);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 36, amount);

        lightShader.use();
        lightShader.setUniformMatrix4fv("projection", camera.getProjection(ratio));
        lightShader.setUniformMatrix4fv("view", camera.getViewMatrix());
        lightShader.setUniform2f("screenSize", size.width, size.height);
        lightShader.setUniform1i("glow", 1);
        
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(sphereSize));
        lightShader.setUniformMatrix4fv("model", model);
        lightShader.setUniform3f("lightColor", glm::vec3(1., .6, .4));

        glBindVertexArray(sphereVAO);
        glDrawElements(GL_TRIANGLES, (int)sphere.indices.size(), GL_UNSIGNED_INT, 0);
        
        system.swapBuffer();
        system.pollEvents();
    }
    
    glDeleteVertexArrays(1, &sphereVAO);
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteBuffers(1, &sphereEBO);
    glDeleteBuffers(1, &sphereVBO);
    glDeleteBuffers(1, &cubeVBO);
}
