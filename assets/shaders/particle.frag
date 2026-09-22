#version 460 core

in vec4 ParticleColor;
in float LifeFactor;

out vec4 FragColor;

// GLSL 没有内置 PI，必须自己定义
const float PI = 3.14159265359;

// 点到线段的距离
float distToSegment(vec2 p, vec2 a, vec2 b) {
    vec2 pa = p - a;
    vec2 ba = b - a;
    float h = clamp(dot(pa, ba) / dot(ba, ba), 0.0, 1.0);
    return length(pa - ba * h);
}

void main() {
    // 点精灵局部坐标，中心移到 (0,0)
    vec2 p = gl_PointCoord - vec2(0.5);
    float r = length(p);
    if (r > 0.5) discard;

    // 折叠成 6 重对称的半扇区 [0, PI/6]
    float a = atan(p.y, p.x);
    a = abs(mod(a + PI / 6.0, PI / 3.0) - PI / 6.0);

    // 扇区局部坐标：u 沿主干方向，v 为横向
    float u = r * cos(a);
    float v = r * sin(a);
    vec2 uv = vec2(u, v);

    // ---- 主干：从 (0,0) 到 (0.48, 0) ----
    float d = length(vec2(max(u - 0.48, 0.0), v));

    // ---- 三对侧枝 ----
    for (int i = 0; i < 3; i++) {
        float fi = float(i);
        float ui = 0.14 + fi * 0.11;
        float L  = 0.10 - fi * 0.02;
        vec2 base = vec2(ui, 0.0);
        vec2 tip  = vec2(ui + L * 0.5, L * 0.866);
        d = min(d, distToSegment(uv, base, tip));
    }

    // ---- 线条粗细 + 柔边 ----
    float thickness = 0.012;
    float shape = 1.0 - smoothstep(thickness, thickness + 0.015, d);
    if (shape <= 0.0) discard;

    // ---- 雪花配色 ----
    float life = clamp(LifeFactor, 0.0, 1.0);

    float t = smoothstep(0.0, 0.45, r);
    vec3 color = mix(vec3(1.0, 1.0, 1.0), vec3(0.55, 0.80, 1.0), t);
    color *= 0.7 + 0.3 * life;

    float alpha = ParticleColor.a * shape * life;
    FragColor = vec4(color, alpha);
}