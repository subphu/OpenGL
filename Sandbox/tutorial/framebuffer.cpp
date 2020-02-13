//  Copyright © 2020 Subph. All rights reserved.
//

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <cmath>

#include "tutorial.h"
#include "define.h"
#include "shader/shader.h"
#include "camera/camera.h"
#include "system/system.h"
#include "libraries/stb_image.h"
#include "object/mesh.h"

unsigned int loadTexture(const char *path);

void runFramebuffer() {
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE); 
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    System &system = System::instance();
    Size<int> size = system.getFramebufferSize();
    float ratio = (float)size.width / (float)size.height;
    
    Shader shader = Shader();
    shader.addShaderFrom("shader/tutorial/shader.vert", GL_VERTEX_SHADER);
    shader.addShaderFrom("shader/tutorial/shader.frag", GL_FRAGMENT_SHADER);
    shader.bindFragDataLocation(0, "fragColor");
    shader.compile();
    
    Shader screenShader = Shader();
    screenShader.addShaderFrom("shader/tutorial/screen.vert", GL_VERTEX_SHADER);
    screenShader.addShaderFrom("shader/tutorial/screen.frag", GL_FRAGMENT_SHADER);
    screenShader.bindFragDataLocation(0, "fragColor");
    screenShader.compile();
    
    Shader lightShader = Shader();
    lightShader.addShaderFrom("shader/tutorial/light.vert", GL_VERTEX_SHADER);
    lightShader.addShaderFrom("shader/tutorial/light.frag", GL_FRAGMENT_SHADER);
    lightShader.bindFragDataLocation(0, "fragColor");
    lightShader.compile();
    
    
    unsigned long lightVerticesSize, cubeVerticesSize;
    float * lightVertices = Mesh::generateCube(lightVerticesSize);
    float * cubeVertices = Mesh::generateCube(cubeVerticesSize, (MESH_NORMAL | MESH_TEXTURE));
    
    float planeVertices[] = {
        -1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f,
         1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
        -1.0f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f,
        -1.0f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f,
         1.0f,  0.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f,
         1.0f,  0.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f
    };
    
    float quadVertices[] = {
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };
    
    glm::vec3 pointLightPositions[] = {
        glm::vec3( 0.0f,  0.2f,  2.0f),
        glm::vec3( 2.0f,  1.5f, -0.0f),
        glm::vec3(-2.0f,  1.0f, -0.0f),
        glm::vec3( 0.0f,  0.5f, -2.0f)
    };
    
    glm::vec3 pointLightColors[] = {
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f)
    };
    
    GLuint cubeVAO, cubeVBO;
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
    
    GLuint planeVAO, planeVBO;
    glGenVertexArrays(1, &planeVAO);
    glBindVertexArray(planeVAO);
    glGenBuffers(1, &planeVBO);
    glBindBuffer(GL_ARRAY_BUFFER, planeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(planeVertices), planeVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
    
    GLuint quadVAO, quadVBO;
    glGenVertexArrays(1, &quadVAO);
    glBindVertexArray(quadVAO);
    glGenBuffers(1, &quadVBO);
    glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
    
    GLuint lightVAO, lightVBO;
    glGenVertexArrays(1, &lightVAO);
    glBindVertexArray(lightVAO);
    glGenBuffers(1, &lightVBO);
    glBindBuffer(GL_ARRAY_BUFFER, lightVBO);
    glBufferData(GL_ARRAY_BUFFER, lightVerticesSize, lightVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    
    unsigned int cubeTexture = loadTexture("resources/texture/container.jpg");
    unsigned int floorTexture = loadTexture("resources/texture/brickwall.jpg");

    shader.use();
    shader.setUniform1i("useTexture", true);
    shader.setUniform1i("material.textureDiffuse", 0);

    screenShader.use();
    screenShader.setUniform1i("texture0", 0);

    
    
    unsigned int framebuffer;
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    
    unsigned int textureColorbuffer;
    glGenTextures(1, &textureColorbuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, size.width, size.height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorbuffer, 0);
    
    unsigned int RBO;
    glGenRenderbuffers(1, &RBO);
    glBindRenderbuffer(GL_RENDERBUFFER, RBO);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, size.width, size.height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, RBO);
    
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        LOG "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" ENDL;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    
    
    float lag = 0;
    float lastTime = system.getTime();
    float frameDelay = 1.f/60.f;
    
    int totalPointLights = 4;
    int filterNo = 1;
    bool prevKeyZ = false;
    bool prevKeyX = false;

    Camera camera(glm::vec3(0.0f, 2.0f, 8.0f));
    
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
            filterNo = prevKeyZ && !currentKeyZ ? (filterNo+4) % 5 : filterNo;
            prevKeyZ = currentKeyZ;
            
            bool currentKeyX = system.getKeyState(key_x);
            filterNo = prevKeyX && !currentKeyX ? (filterNo+1) % 5 : filterNo;
            prevKeyX = currentKeyX;
            
            lag -= frameDelay;
        }
        
        
        glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
        glEnable(GL_DEPTH_TEST);
        
        
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        shader.use();
        shader.setUniform3f("viewPos", camera.getPosition());
        shader.setUniformMatrix4fv("view", camera.getViewMatrix());
        shader.setUniformMatrix4fv("projection", camera.getProjection(ratio));
        
        shader.setUniform3f("dirLight.direction", -0.2f, -1.0f, -0.3f);
        shader.setUniform3f("dirLight.light.ambient", 0.05f, 0.05f, 0.05f);
        shader.setUniform3f("dirLight.light.diffuse", 0.4f, 0.4f, 0.4f);
        shader.setUniform3f("dirLight.light.specular", 0.5f, 0.5f, 0.5f);
        
        shader.setUniform1i("totalPointLights", totalPointLights);
        for(int i = 0; i < totalPointLights; i++) {
            std::string idx = std::to_string(i);
            shader.setUniform3f("pointLights[" + idx + "].position", pointLightPositions[i]);
            shader.setUniform3f("pointLights[" + idx + "].light.ambient", pointLightColors[i] * 0.05f);
            shader.setUniform3f("pointLights[" + idx + "].light.diffuse", pointLightColors[i] * 0.8f);
            shader.setUniform3f("pointLights[" + idx + "].light.specular", pointLightColors[i]);
            shader.setUniform1f("pointLights[" + idx + "].constant.linear", 0.09);
            shader.setUniform1f("pointLights[" + idx + "].constant.quadratic", 0.032);
        }
        
        glBindVertexArray(cubeVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cubeTexture);
        
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(-1.0f, 0.5f, -1.0f));
        shader.setUniformMatrix4fv("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(2.0f, 0.5f, 0.0f));
        shader.setUniformMatrix4fv("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        
        
        glBindVertexArray(planeVAO);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        model = glm::mat4(1.0f);
        model = glm::scale(model, glm::vec3(10.0f));
        shader.setUniformMatrix4fv("model", model);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        
        lightShader.use();
        lightShader.setUniformMatrix4fv("projection", camera.getProjection(ratio));
        lightShader.setUniformMatrix4fv("view", camera.getViewMatrix());
        lightShader.setUniform2f("screenSize", size.width, size.height);
        
        glBindVertexArray(lightVAO);
        for (unsigned int i = 0; i < 4; i++) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f));
            lightShader.setUniformMatrix4fv("model", model);
            lightShader.setUniform3f("lightColor", pointLightColors[i]);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glDisable(GL_DEPTH_TEST);
        glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        
        screenShader.use();
        screenShader.setUniform1i("useFilter", filterNo);
        glBindVertexArray(quadVAO);
        glBindTexture(GL_TEXTURE_2D, textureColorbuffer);
        glDrawArrays(GL_TRIANGLES, 0, 6);

        
        system.swapBuffer();
        system.pollEvents();
        
        lag += system.getTime() - lastTime;
        lastTime = system.getTime();
    }
    
    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &planeVAO);
    glDeleteVertexArrays(1, &quadVAO);
    glDeleteVertexArrays(1, &lightVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &planeVBO);
    glDeleteBuffers(1, &quadVBO);
    glDeleteBuffers(1, &lightVBO);
}

unsigned int loadTexture(char const * path) {
    unsigned int texture;
    glGenTextures(1, &texture);

    int width, height, colorChannels;
    unsigned char *data = stbi_load(path, &width, &height, &colorChannels, 0);
    if (!data) {
        LOG "Texture failed to load at path:" SPACE path ENDL;
        stbi_image_free(data);
        return texture;
    }
    
    GLenum format = GL_RGB;
    if      (colorChannels == 1) format = GL_RED;
    else if (colorChannels == 4) format = GL_RGBA;

    glBindTexture(GL_TEXTURE_2D, texture);
    glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    stbi_image_free(data);

    return texture;
}
