#version 460 core

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

out vec4 FragColor;


uniform vec3 lightPos;         
uniform vec3 viewPos;           
uniform vec3 objectColor = vec3(1.0, 1.0, 1.0);  
uniform sampler2D texture1;     
uniform bool useTexture = true;

void main() {

    vec3 ambient = 0.2 * objectColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = 0.5 * diff * objectColor;

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, halfwayDir), 0.0), 32.0);
    vec3 specular = 0.5 * spec * vec3(1.0);

    vec3 result = ambient + diffuse + specular;

    if (useTexture) {
        vec4 texColor = texture(texture1, TexCoords);
        result *= texColor.rgb;
    }

    FragColor = vec4(result, 1.0);
}