#version 460 core

in vec4 ParticleColor;
in float LifeFactor;

out vec4 FragColor;

void main() {
    vec2 center = gl_PointCoord - vec2(0.5);
    float dist = length(center);


    if (dist > 0.5) discard;


    float alpha = 1.0 - smoothstep(0.4, 0.5, dist);


    vec3 color = ParticleColor.rgb * (0.5 + 0.5 * LifeFactor);
    float finalAlpha = ParticleColor.a * alpha * LifeFactor;

    FragColor = vec4(color, finalAlpha);
}



//6566