//  Copyright © 2020 Subph. All rights reserved.
//

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <cmath>

#include "basic.h"
#include "../define.h"
#include "../shader.h"
#include "../camera.h"
#include "../system/system.h"
#include "../object/mesh.h"

void runMultipleLight() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    System &system = System::instance();
    Size<int> size = system.getWindowSize();
    float ratio = (float)size.width / (float)size.height;
    
    Shader shader = Shader();
    shader.addShaderFrom("shader/basic/shader.vert", GL_VERTEX_SHADER);
    shader.addShaderFrom("shader/basic/shader.frag", GL_FRAGMENT_SHADER);
    shader.bindFragDataLocation(0, "fragColor");
    shader.compile();
    
    Shader lightShader = Shader();
    lightShader.addShaderFrom("shader/basic/light.vert", GL_VERTEX_SHADER);
    lightShader.addShaderFrom("shader/basic/light.frag", GL_FRAGMENT_SHADER);
    lightShader.bindFragDataLocation(0, "fragColor");
    lightShader.compile();
    
    unsigned long memorySize;
    float * vertices = Mesh::createCube(memorySize, MESH_NORMAL);

    glm::vec3 cubePositions[] = {
        glm::vec3( 0.0f,  0.0f,  0.0f),
        glm::vec3( 2.0f,  5.0f, -15.0f),
        glm::vec3(-1.5f, -2.2f, -2.5f),
        glm::vec3(-3.8f, -2.0f, -12.3f),
        glm::vec3( 2.4f, -0.4f, -3.5f),
        glm::vec3(-1.7f,  3.0f, -7.5f),
        glm::vec3( 1.3f, -2.0f, -2.5f),
        glm::vec3( 1.5f,  2.0f, -2.5f),
        glm::vec3( 1.5f,  0.2f, -1.5f),
        glm::vec3(-1.3f,  1.0f, -1.5f)
    };
    
    glm::vec3 pointLightPositions[] = {
        glm::vec3( 0.7f,  0.2f,  2.0f),
        glm::vec3( 2.3f, -3.3f, -4.0f),
        glm::vec3(-4.0f,  2.0f, -12.0f),
        glm::vec3( 0.0f,  0.0f, -3.0f)
    };
    
    glm::vec3 pointLightColors[] = {
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(0.3f, 0.3f, 1.0f),
        glm::vec3(0.0f, 1.0f, 0.1f),
        glm::vec3(1.0f, 0.0f, 0.3f)
    };
    
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
    
    int totalPointLights = 4;
    int totalSpotLights = 1;
    bool prevKeyZ = false;

    Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));
    
    system.disableCursor();

    while (system.getWindowState()) {
        if (system.getKeyState(key_esc)) system.closeWindow();
        
        while (lag > frameDelay) {
            glm::vec3 movement = glm::vec3(0.f, 0.f, 0.f);
            movement.x = system.getKeyState(key_d) - system.getKeyState(key_a);
            movement.y = system.getKeyState(key_q) - system.getKeyState(key_e);
            movement.z = system.getKeyState(key_w) - system.getKeyState(key_s);
            camera.move(movement);
            
            glm::vec2 delta = system.getCursorMovement();
            camera.turn(delta);
            
            bool currentKeyZ = system.getKeyState(key_z);
            totalSpotLights = prevKeyZ && !currentKeyZ ? !totalSpotLights : totalSpotLights;
            prevKeyZ = currentKeyZ;
            
            lag -= frameDelay;
        }
        
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        shader.use();
        shader.setUniform3f("viewPos", camera.getPosition());
        
        shader.setUniform3f("material.ambient",  1.0f, 1.0f, 1.0f);
        shader.setUniform3f("material.diffuse",  1.0f, 1.0f, 1.0f);
        shader.setUniform3f("material.specular", 0.5f, 0.5f, 0.5f);
        shader.setUniform1f("material.shininess", 32.0f);
        
        shader.setUniform1i("totalPointLights", totalPointLights);
        shader.setUniform1i("totalSpotLights", totalSpotLights);
        
        shader.setUniform3f("dirLight.direction", -0.2f, -1.0f, -0.3f);
        shader.setUniform3f("dirLight.light.ambient", 0.05f, 0.05f, 0.05f);
        shader.setUniform3f("dirLight.light.diffuse", 0.4f, 0.4f, 0.4f);
        shader.setUniform3f("dirLight.light.specular", 0.5f, 0.5f, 0.5f);
        
        for(int i = 0; i < totalPointLights; i++) {
            std::string idx = std::to_string(i);
            shader.setUniform3f("pointLights[" + idx + "].position", pointLightPositions[i]);
            shader.setUniform3f("pointLights[" + idx + "].light.ambient", pointLightColors[i] * 0.05f);
            shader.setUniform3f("pointLights[" + idx + "].light.diffuse", pointLightColors[i] * 0.8f);
            shader.setUniform3f("pointLights[" + idx + "].light.specular", pointLightColors[i]);
            shader.setUniform1f("pointLights[" + idx + "].constant.linear", 0.09);
            shader.setUniform1f("pointLights[" + idx + "].constant.quadratic", 0.032);
        }
        
        shader.setUniform3f("spotLights[0].position", camera.getPosition());
        shader.setUniform3f("spotLights[0].direction", camera.getFront());
        shader.setUniform3f("spotLights[0].light.ambient", 0.0f, 0.0f, 0.0f);
        shader.setUniform3f("spotLights[0].light.diffuse", 1.0f, 1.0f, 1.0f);
        shader.setUniform3f("spotLights[0].light.specular", 1.0f, 1.0f, 1.0f);
        shader.setUniform1f("spotLights[0].constant.linear", 0.09);
        shader.setUniform1f("spotLights[0].constant.quadratic", 0.032);
        shader.setUniform1f("spotLights[0].cutOff", glm::cos(glm::radians(12.5f)));
        shader.setUniform1f("spotLights[0].outerCutOff", glm::cos(glm::radians(15.0f)));
        
        shader.setUniformMatrix4fv("projection", camera.getProjection(ratio));
        shader.setUniformMatrix4fv("view", camera.getViewMatrix());

        glBindVertexArray(cubeVAO);
        for (unsigned int i = 0; i < 10; i++) {
            float angle = 20.0f * i;
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            shader.setUniformMatrix4fv("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        
        lightShader.use();
        lightShader.setUniformMatrix4fv("projection", camera.getProjection(ratio));
        lightShader.setUniformMatrix4fv("view", camera.getViewMatrix());
        
        glBindVertexArray(lightVAO);
        for (unsigned int i = 0; i < 4; i++) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f));
            lightShader.setUniformMatrix4fv("model", model);
            lightShader.setUniform3f("lightColor", pointLightColors[i]);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        
        system.swapBuffer();
        system.pollEvents();
        
        lag += system.getTime() - lastTime;
        lastTime = system.getTime();
    }
    
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);
}
