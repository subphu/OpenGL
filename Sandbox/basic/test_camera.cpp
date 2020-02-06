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
#include "../libraries/stb_image.h"
#include "../object/mesh.h"

Camera camera(glm::vec3(0.0f, 0.0f, 5.0f));

void runTestCamera() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    System &system = System::instance();
    Size<int> size = system.getFramebufferSize();
    
    Shader shader = Shader();
    shader.addShaderFrom("shader/basic/perspective.vert", GL_VERTEX_SHADER);
    shader.addShaderFrom("shader/basic/perspective.frag", GL_FRAGMENT_SHADER);
    shader.bindFragDataLocation(0, "fragColor");
    shader.compile();
    
    unsigned long memorySize;
    float * vertices = Mesh::generateCube(memorySize, MESH_TEXTURE);

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
    
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, memorySize, vertices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    int width, height, colorChannels;
    unsigned char *data = stbi_load("resources/texture/container.jpg", &width, &height, &colorChannels, 0);
    if (data) {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    } else {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
    
    shader.use();
    shader.setUniform1i("texture0", 0);
    
    float lag = 0;
    float lastTime = system.getTime();
    float frameDelay = 1.f/60.f;

    auto scroll = [](float xoffset, float yoffset) { camera.zoom(yoffset); };

    system.disableCursor();
    system.setScrollCallback(scroll);

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
        
        shader.use();

        float ratio = (float)size.width / (float)size.height;
        shader.setUniformMatrix4fv("projection", camera.getProjection(ratio));
        shader.setUniformMatrix4fv("view", camera.getViewMatrix());


        glBindVertexArray(VAO);
        for (unsigned int i = 0; i < 10; i++) {
            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, cubePositions[i]);
            float angle = 20.0f * i;
            model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
            shader.setUniformMatrix4fv("model", model);

            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        
        system.swapBuffer();
        system.pollEvents();
        
        lag += system.getTime() - lastTime;
        lastTime = system.getTime();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}
