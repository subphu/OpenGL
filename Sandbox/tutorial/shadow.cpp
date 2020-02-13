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
#include "define.h"
#include "shader/shader.h"
#include "camera/camera.h"
#include "system/system.h"
#include "libraries/stb_image.h"
#include "object/mesh.h"

float ratio = 0.0;
int totalPointLights = 4;
glm::vec3 pointLightPositions[4];
glm::vec3 pointLightPosInitial[] = {
    glm::vec3( 0.0f,  0.0f,  0.0f),
    glm::vec3( 3.0f, -1.2f, -2.3f),
    glm::vec3(-1.6f,  1.4f,  1.0f),
    glm::vec3( 0.0f,  0.0f,  3.5f)
};

glm::vec3 cubePositions[] = {
    glm::vec3( 0.0f,  0.0f,  2.0f),
    glm::vec3(-3.5f, -3.2f, -3.5f),
    glm::vec3( 3.3f, -3.6f, -3.5f),
    glm::vec3( 1.5f,  3.6f, -3.5f),
    glm::vec3( 3.5f,  0.2f, -1.5f),
    glm::vec3(-2.3f,  2.0f, -1.5f)
};
glm::vec3 pointLightColors[] = {
    glm::vec3(1.0f, 1.0f, 1.0f),
    glm::vec3(1.0f, 0.0f, 0.3f),
    glm::vec3(0.0f, 1.0f, 0.1f),
    glm::vec3(0.1f, 0.0f, 1.0f)
};

void setLightUniform(Shader* shader);
void drawCubes(Shader* shader);
void drawLightcubes(Shader* shader, Camera* camera);

void runShadow() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glEnable(GL_FRAMEBUFFER_SRGB);
    
    System &system = System::instance();
    Size<int> size = system.getFramebufferSize();
    ratio = (float)size.width / (float)size.height;
    
    Shader shader = Shader();
    shader.addShaderFrom("shader/tutorial/shadow.vert", GL_VERTEX_SHADER);
    shader.addShaderFrom("shader/tutorial/shadow.frag", GL_FRAGMENT_SHADER);
    shader.compile();
    
    Shader depthShader = Shader();
    depthShader.addShaderFrom("shader/tutorial/depth.vert", GL_VERTEX_SHADER);
    depthShader.addShaderFrom("shader/tutorial/depth.geom", GL_GEOMETRY_SHADER);
    depthShader.addShaderFrom("shader/tutorial/depth.frag", GL_FRAGMENT_SHADER);
    depthShader.compile();
    
    Shader lightShader = Shader();
    lightShader.addShaderFrom("shader/tutorial/light.vert", GL_VERTEX_SHADER);
    lightShader.addShaderFrom("shader/tutorial/light.frag", GL_FRAGMENT_SHADER);
    lightShader.compile();
    
    Size<int> shadowSize = {512, 512};
    float shadowRatio = (float)shadowSize.width / (float)shadowSize.height;
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    
    unsigned int depthCubemap[totalPointLights];
    for (int i = 0; i < totalPointLights; i++) {
        glGenTextures(1, &depthCubemap[i]);
        glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap[i]);
        for (unsigned int j = 0; j < 6; j++)
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + j, 0, GL_DEPTH_COMPONENT, shadowSize.width, shadowSize.height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap[i], 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    
    shader.use();
    shader.setUniform1i("material.textureDiffuse", 9);
    
    unsigned long memorySize;
    float * vertices = Mesh::generateCube(memorySize, MESH_NORMAL);
    
    GLuint VBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glBindVertexArray(cubeVAO);
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, memorySize, vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    GLuint lightVAO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    float lag = 0;
    float lastTime = system.getTime();
    float frameDelay = 1.f/60.f;

    Camera camera(glm::vec3(0.0f, 0.0f, 4.0f));
    
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
        
        float angle = system.getTime() * 50;
        pointLightPositions[0] = glm::vec3(0.0, sin(angle / 50) * 2, 0.0);
        for (int i = 1; i < totalPointLights; i++) {
            glm::mat4 rotation = glm::mat4(1.0f);
            rotation = glm::rotate(rotation,  glm::radians(angle), glm::vec3(i == 3, i == 2, i == 1));
            pointLightPositions[i] = glm::vec3(rotation * glm::vec4(pointLightPosInitial[i], 1.0f));
        }
        
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        float minDistance = 0.10f;
        float maxDistance = 50.0f;
        glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), shadowRatio, minDistance, maxDistance);
        glViewport(0, 0, shadowSize.width, shadowSize.height);
        depthShader.use();
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        for (int i = 0; i < totalPointLights; i++) {
            glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap[i], 0);
            glClear(GL_DEPTH_BUFFER_BIT);
            glm::vec3 lightPos = pointLightPositions[i];
            depthShader.setUniformMatrix4fv("shadowMatrices[0]", shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
            depthShader.setUniformMatrix4fv("shadowMatrices[1]", shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
            depthShader.setUniformMatrix4fv("shadowMatrices[2]", shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)));
            depthShader.setUniformMatrix4fv("shadowMatrices[3]", shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)));
            depthShader.setUniformMatrix4fv("shadowMatrices[4]", shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
            depthShader.setUniformMatrix4fv("shadowMatrices[5]", shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f)));
            depthShader.setUniform1f("maxDistance", maxDistance);
            depthShader.setUniform3f("lightPos", lightPos);
            glBindVertexArray(cubeVAO);
            drawCubes(&depthShader);
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        glViewport(0, 0, size.width, size.height);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        shader.use();
        shader.setUniform3f("viewPos", camera.getPosition());
        shader.setUniformMatrix4fv("projection", camera.getProjection(ratio));
        shader.setUniformMatrix4fv("view", camera.getViewMatrix());
        shader.setUniform1f("maxDistance", maxDistance);
        setLightUniform(&shader);
        
        for (int i = 0; i < totalPointLights; i++) {
            shader.setUniform1i("depthMap[" + std::to_string(i) + "]", i);
            glActiveTexture(GL_TEXTURE0 + i);
            glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap[i]);
        }
        glBindVertexArray(cubeVAO);
        drawCubes(&shader);
        
        glBindVertexArray(lightVAO);
        drawLightcubes(&lightShader, &camera);
        
        system.swapBuffer();
        system.pollEvents();
    }
    
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);
}

void setLightUniform(Shader* shader) {
    shader->setUniform3f("material.ambient",  1.0f, 1.0f, 1.0f);
    shader->setUniform3f("material.diffuse",  1.0f, 1.0f, 1.0f);
    shader->setUniform3f("material.specular", 0.5f, 0.5f, 0.5f);
    shader->setUniform1f("material.shininess", 4.0f);
    
    shader->setUniform1i("totalPointLights", totalPointLights);
    
    shader->setUniform3f("dirLight.direction", -0.2f, -1.0f, -0.3f);
    shader->setUniform3f("dirLight.light.ambient", 0.05f, 0.05f, 0.05f);
    shader->setUniform3f("dirLight.light.diffuse", 0.4f, 0.4f, 0.4f);
    shader->setUniform3f("dirLight.light.specular", 0.5f, 0.5f, 0.5f);
    
    for(int i = 0; i < totalPointLights; i++) {
        std::string idx = std::to_string(i);
        shader->setUniform3f("pointLights[" + idx + "].position", pointLightPositions[i]);
        shader->setUniform3f("pointLights[" + idx + "].light.ambient", pointLightColors[i] * 0.05f);
        shader->setUniform3f("pointLights[" + idx + "].light.diffuse", pointLightColors[i] * 0.7f);
        shader->setUniform3f("pointLights[" + idx + "].light.specular", pointLightColors[i]);
        shader->setUniform1f("pointLights[" + idx + "].constant.linear", 0.14);
        shader->setUniform1f("pointLights[" + idx + "].constant.quadratic", 0.07);
    }
}

void drawCubes(Shader* shader) {
    glm::mat4 model = glm::mat4(1.0f);
    model = glm::scale(model, glm::vec3(10.0f));
    shader->setUniformMatrix4fv("model", model);
    shader->setUniform1i("invertNormal", true);
    glCullFace(GL_FRONT);
    glDrawArrays(GL_TRIANGLES, 0, 36);
    glCullFace(GL_BACK);
    shader->setUniform1i("invertNormal", false);

    for (unsigned int i = 0; i < 6; i++) {
        float angle = 20.0f * i;
        model = glm::mat4(1.0f);
        model = glm::translate(model, cubePositions[i]);
        model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
        shader->setUniformMatrix4fv("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}


void drawLightcubes(Shader* lightShader, Camera* camera) {
    lightShader->use();
    lightShader->setUniformMatrix4fv("projection", camera->getProjection(ratio));
    lightShader->setUniformMatrix4fv("view", camera->getViewMatrix());
    
    for (unsigned int i = 0; i < 4; i++) {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, pointLightPositions[i]);
        model = glm::scale(model, glm::vec3(0.2f));
        lightShader->setUniformMatrix4fv("model", model);
        lightShader->setUniform3f("lightColor", pointLightColors[i]);
        glDrawArrays(GL_TRIANGLES, 0, 36);
    }
}
