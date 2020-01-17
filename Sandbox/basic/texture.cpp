//  Copyright © 2019 Subph. All rights reserved.
//

#include <iostream>

#include "basic.h"
#include "../define.h"
#include "../shader.h"
#include "../system/system.h"
#include "../libraries/stb_image.h"

void runTexture() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    Shader shader = Shader();
    shader.addShaderFrom("shader/basic/texture.vert", GL_VERTEX_SHADER);
    shader.addShaderFrom("shader/basic/texture.frag", GL_FRAGMENT_SHADER);
    shader.compile();
    
    float vertices[] = {
        // positions         // colors             // texture coords
         0.5f,  0.5f, 0.f,   1.f, 0.f, 0.f, 1.f,   1.f, 1.f, // top right
         0.5f, -0.5f, 0.f,   0.f, 1.f, 0.f, 1.f,   1.f, 0.f, // bottom right
        -0.5f, -0.5f, 0.f,   0.f, 0.f, 1.f, 1.f,   0.f, 0.f, // bottom left
        -0.5f,  0.5f, 0.f,   1.f, 1.f, 0.f, 1.f,   0.f, 1.f  // top left
    };
    
    unsigned int indices[] = {
        0, 1, 3, // first triangle
        1, 2, 3  // second triangle
    };
    
    unsigned int VAO, VBO, EBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 4, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(7 * sizeof(float)));
    glEnableVertexAttribArray(2);
    
    // texture
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    int width, height, colorChannels;
    stbi_set_flip_vertically_on_load(true);
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
    
    System &system = System::instance();
    while (system.getWindowState()) {
        if (system.getKeyState(key_esc)) system.closeWindow();
        
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        shader.use();

        glBindVertexArray(VAO);
        
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        system.swapBuffer();
        system.pollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &EBO);
    glDeleteBuffers(1, &VBO);
}
