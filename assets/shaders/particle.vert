#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec4 aColor;
layout (location = 2) in float aLifeFactor; 

uniform mat4 view;
uniform mat4 projection;

out vec4 ParticleColor;
out float LifeFactor;

void main() {

    gl_Position = projection * view * vec4(aPos, 1.0);
    gl_PointSize = mix(5.0, 25.0, aLifeFactor);

    ParticleColor = aColor;
    LifeFactor = aLifeFactor;
}

//6566
