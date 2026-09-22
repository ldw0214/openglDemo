#include "Shader.h"
using namespace MiniEngine;

namespace MiniEngine {
    const char* const errorVertSrc = R"(
             #version 460 core
             layout (location = 0) in vec3 aPos;
             layout (location = 1) in vec3 aNormal;
             layout (location = 2) in vec2 aTexCoords;

             uniform mat4 model;
             uniform mat4 view;
             uniform mat4 projection;
             out vec2 TexCoords;
             void main() {
                 TexCoords = aTexCoords;
                 gl_Position =  projection * view * model * vec4(aPos, 1.0);
             }
         )";
    const char* const errorFragSrc = R"(
            #version 460 core
            in vec2 TexCoords;
            out vec4 FragColor;
            void main() {
                FragColor = vec4(1.0, 0.0, 0.0, 1.0);
            }
        )";
}