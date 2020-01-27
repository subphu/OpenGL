//  Copyright © 2019 Subph. All rights reserved.
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

void runLighting() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    System &system = System::instance();
    Size<int> size = system.getWindowSize();
    float ratio = (float)size.width / (float)size.height;
    
    Shader shader = Shader();
    shader.addShaderFrom("shader/basic/phong.vert", GL_VERTEX_SHADER);
    shader.addShaderFrom("shader/basic/phong.frag", GL_FRAGMENT_SHADER);
    shader.bindFragDataLocation(0, "fragColor");
    shader.compile();
    
    Shader lightShader = Shader();
    lightShader.addShaderFrom("shader/basic/light.vert", GL_VERTEX_SHADER);
    lightShader.addShaderFrom("shader/basic/light.frag", GL_FRAGMENT_SHADER);
    lightShader.bindFragDataLocation(0, "fragColor");
    lightShader.compile();
    
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

    glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
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
            
            lag -= frameDelay;
        }
        
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glm::vec3 lightColor = glm::vec3(1.f);
        lightColor.x = fmax(0.3, sin(system.getTime() * 2.0f));
        lightColor.y = fmax(0.3, sin(system.getTime() * 0.7f));
        lightColor.z = fmax(0.3, sin(system.getTime() * 1.3f));
        
        shader.use();
        shader.setUniform3f("lightPos", lightPos);
        
        shader.setUniform3f("light.ambient", lightColor * glm::vec3(0.1f));
        shader.setUniform3f("light.diffuse", lightColor * glm::vec3(0.5f));
        shader.setUniform3f("light.specular", 1.0f, 1.0f, 1.0f);
        
        shader.setUniform3f("material.ambient",  1.0f, 1.0f, 1.0f);
        shader.setUniform3f("material.diffuse",  1.0f, 1.0f, 1.0f);
        shader.setUniform3f("material.specular", 0.5f, 0.5f, 0.5f);
        shader.setUniform1f("material.shininess", 32.0f);

        shader.setUniformMatrix4fv("projection", camera.getProjection(ratio));
        shader.setUniformMatrix4fv("view", camera.getViewMatrix());
        
        glm::mat4 model = glm::mat4(1.0f);
        shader.setUniformMatrix4fv("model", model);

        glBindVertexArray(cubeVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        lightShader.use();
        lightShader.setUniform3f("lightColor", lightColor);
        lightShader.setUniformMatrix4fv("projection", camera.getProjection(ratio));
        lightShader.setUniformMatrix4fv("view", camera.getViewMatrix());
        lightShader.setUniform2f("screenSize", size.width, size.height);
        
        model = glm::mat4(1.0f);
        model = glm::translate(model, lightPos);
        model = glm::scale(model, glm::vec3(0.1f));
        lightShader.setUniformMatrix4fv("model", model);

        glBindVertexArray(lightVAO);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        system.swapBuffer();
        system.pollEvents();
        
        lag += system.getTime() - lastTime;
        lastTime = system.getTime();
    }
    
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &VBO);
}
