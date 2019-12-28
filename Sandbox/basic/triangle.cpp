//  Copyright © 2019 Subph. All rights reserved.
//

#include <iostream>
#include <cmath>

#include "basic.h"
#include "../define.h"
#include "../shader.h"
#include "../system/system.h"

void Basic::runTriangle() {
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    Shader shader = Shader();
    shader.addShaderFrom("shader/shader.vert", GL_VERTEX_SHADER);
    shader.addShaderFrom("shader/shader.frag", GL_FRAGMENT_SHADER);
    shader.bindFragDataLocation(0, "fragColor");
    shader.compile();
    
    GLfloat vertices[] = { // x, y, z, r, g, b, a
         0.0f,  0.5f, 0.f, 1.f, 0.f, 0.f, 1.f,
         0.5f, -0.5f, 0.f, 0.f, 1.f, 0.f, 1.f,
        -0.5f, -0.5f, 0.f, 0.f, 0.f, 1.f, 1.f
    };
    
    GLuint VAO, VBO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    GLint posAttrib = shader.getAttribLocation("position");
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)0);
    glEnableVertexAttribArray(posAttrib);

    GLint colAttrib = shader.getAttribLocation("color");
    glVertexAttribPointer(colAttrib, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(GLfloat), (void*)(3 * sizeof(GLfloat) ));
    glEnableVertexAttribArray(colAttrib);
    
    System &system = System::instance();
    while (system.getWindowState()) {
        if (system.getKeyState(key_esc)) system.closeWindow();
        
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        float alpha = (sin(system.getTime()) + 1.1) / 2;
        shader.setUniform4f("alpha", 1.0f, 1.0f, 1.0f, alpha);
        shader.use();

        glBindVertexArray(VAO);
        
        glDrawArrays(GL_TRIANGLES, 0, 3);
        
        system.swapBuffer();
        system.pollEvents();
    }

    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
}
