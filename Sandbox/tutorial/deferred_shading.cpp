//  Copyright © 2020 Subph. All rights reserved.
//

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>
#include <cmath>
#include <unistd.h>

#include "tutorial.h"
#include "define.h"
#include "shader/shader.h"
#include "camera/camera.h"
#include "system/system.h"
#include "libraries/stb_image/stb_image.h"
#include "object/mesh.h"

using namespace tutorial;

unsigned int loadTexture(const char *path);

void Tutorial::runDeferredShading() {
    System &system = System::instance();
    Size<int> size = system.getFramebufferSize();
    float ratio = (float)size.width / (float)size.height;
    
    Shader gBufferShader = Shader();
    gBufferShader.addShaderFrom("shader/tutorial/g_buffer.vert", GL_VERTEX_SHADER);
    gBufferShader.addShaderFrom("shader/tutorial/g_buffer.frag", GL_FRAGMENT_SHADER);
    gBufferShader.compile();
    
    Shader screenShader = Shader();
    screenShader.addShaderFrom("shader/tutorial/deferred_shading.vert", GL_VERTEX_SHADER);
    screenShader.addShaderFrom("shader/tutorial/deferred_shading.frag", GL_FRAGMENT_SHADER);
    screenShader.compile();
    
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
    glBufferData(GL_ARRAY_BUFFER, sphere.sizeofVertices() + sphere.sizeofNormals() + sphere.sizeofTexCoords(), NULL, GL_STATIC_DRAW);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sphere.sizeofVertices(), &sphere.vertices[0]);
    glBufferSubData(GL_ARRAY_BUFFER, sphere.sizeofVertices(), sphere.sizeofNormals(), &sphere.normals[0]);
    glBufferSubData(GL_ARRAY_BUFFER, sphere.sizeofVertices() + sphere.sizeofNormals(), sphere.sizeofTexCoords(), &sphere.texCoords[0]);
    glGenBuffers(1, &sphereEBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sphere.sizeofIndices(), &sphere.indices[0], GL_STATIC_DRAW);
    
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)(sphere.sizeofVertices()));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)(sphere.sizeofVertices() + sphere.sizeofNormals()));
    
    
    unsigned int cubeTexture = loadTexture("resources/texture/container.jpg");
    
    gBufferShader.use();
    gBufferShader.setUniform1i("textureDiffuse", 0);
    
    screenShader.use();
    screenShader.setUniform1i("gPosition", 0);
    screenShader.setUniform1i("gNormal", 1);
    screenShader.setUniform1i("gAlbedoSpec", 2);
    
    
    unsigned int gBuffer;
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    unsigned int gPosition, gNormal, gAlbedoSpec;
    
    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, size.width, size.height, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);
    
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, size.width, size.height, 0, GL_RGB, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);
    
    glGenTextures(1, &gAlbedoSpec);
    glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, size.width, size.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedoSpec, 0);
    
    unsigned int attachments[3] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2 };
    glDrawBuffers(3, attachments);
    
    float lag = 0;
    float lastTime = system.getTime();
    float frameDelay = 1.f/60.f;
    
    int totalLights = 4;
    
    glm::vec3 lightPositions[] = {
        glm::vec3( 0.0f,  0.2f,  1.5f),
        glm::vec3( 1.5f,  0.8f, -0.0f),
        glm::vec3(-1.5f,  0.8f, -0.0f),
        glm::vec3( 0.0f,  0.5f, -1.5f)
    };
    
    glm::vec3 lightColors[] = {
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(0.0f, 0.0f, 1.0f),
        glm::vec3(0.0f, 1.0f, 0.0f),
        glm::vec3(1.0f, 0.0f, 0.0f)
    };

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
            
            lag -= frameDelay;
        }
        
//        glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
        glClearColor(0.f, 0.f, 0.f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        // geometry pass
        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
        glDepthMask(GL_TRUE);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glEnable(GL_CULL_FACE);
        glDisable(GL_BLEND);
        gBufferShader.use();
        gBufferShader.setUniformMatrix4fv("view", camera.getViewMatrix());
        gBufferShader.setUniformMatrix4fv("projection", camera.getProjection(ratio));
        
        glBindVertexArray(sphereVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cubeTexture);
        
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(.0f, 0.0f, .0f));
        gBufferShader.setUniformMatrix4fv("model", model);
        glDrawElements(GL_TRIANGLES, (int)sphere.indices.size(), GL_UNSIGNED_INT, 0);

        glDepthMask(GL_FALSE);
        glDisable(GL_DEPTH_TEST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        
        // lighting pass
        glEnable(GL_BLEND);
        glBlendEquation(GL_FUNC_ADD);
        glBlendFunc(GL_ONE, GL_ONE);
        glClear(GL_COLOR_BUFFER_BIT);
        glCullFace(GL_FRONT);

        screenShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, gAlbedoSpec);
        
        screenShader.setUniform3f("viewPos", camera.getPosition());
        screenShader.setUniformMatrix4fv("projection", camera.getProjection(ratio));
        screenShader.setUniformMatrix4fv("view", camera.getViewMatrix());
        glBindVertexArray(sphereVAO);
        for(int i = 0; i < totalLights; i++) {
            std::string idx = std::to_string(i);
            const float constant = 1.0;
            const float linear = 0.7;
            const float quadratic = 1.8;
            const float maxBrightness = fmax(fmax(lightColors[i].r, lightColors[i].g), lightColors[i].b);
            float radius = (-linear + std::sqrt(linear * linear - 4 * quadratic * (constant - (256.0f / 5.0f) * maxBrightness))) / (2.0f * quadratic);
            screenShader.setUniform1f("light.linear", linear);
            screenShader.setUniform1f("light.quadratic", quadratic);
            screenShader.setUniform1f("light.radius", radius);
            screenShader.setUniform3f("light.position", lightPositions[i]);
            screenShader.setUniform3f("light.color", lightColors[i]);
            screenShader.setUniform2f("screenSize", size.width, size.height);
            
            model = glm::mat4(1.0f);
            model = glm::translate(model, lightPositions[i]);
            model = glm::scale(model, glm::vec3(radius));
            screenShader.setUniformMatrix4fv("model", model);
            glDrawElements(GL_TRIANGLES, (int)sphere.indices.size(), GL_UNSIGNED_INT, 0);
        }
//        glBindVertexArray(quadVAO);
//        glDrawArrays(GL_TRIANGLES, 0, 6);
//
//        glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
//        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
//        glBlitFramebuffer(0, 0, size.width, size.height, 0, 0, size.width, size.height, GL_DEPTH_BUFFER_BIT, GL_NEAREST);
//        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // render light
        lightShader.use();
        lightShader.setUniformMatrix4fv("projection", camera.getProjection(ratio));
        lightShader.setUniformMatrix4fv("view", camera.getViewMatrix());
        glBindVertexArray(sphereVAO);
        glCullFace(GL_BACK);
        for (unsigned int i = 0; i < totalLights; i++) {
            model = glm::mat4(1.0f);
            model = glm::translate(model, lightPositions[i]);
            model = glm::scale(model, glm::vec3(0.1f));
            lightShader.setUniformMatrix4fv("model", model);
            lightShader.setUniform3f("lightColor", lightColors[i]);
            glDrawElements(GL_TRIANGLES, (int)sphere.indices.size(), GL_UNSIGNED_INT, 0);
        }
        
        system.swapBuffer();
        system.pollEvents();
    }
}
