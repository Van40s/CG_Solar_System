#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 FragPos; // Receive the fragment position from the vertex shader
in vec3 Normal; // Receive the normal from the vertex shader

uniform sampler2D texture_diffuse1;
uniform vec3 lightPos; // Position of the light source
uniform vec3 viewPos; // Position of the camera/viewer

void main()
{
    // Phong lighting calculations
    vec3 ambient = 0.1 * texture(texture_diffuse1, TexCoords).rgb; // You can adjust the ambient intensity here
    vec3 lightDir = normalize(lightPos - FragPos);
    vec3 normal = normalize(Normal);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diff * texture(texture_diffuse1, TexCoords).rgb; // You can adjust the diffuse intensity here

    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 2.0); // You can adjust the specular intensity and shininess here
    vec3 specular = vec3(0.5) * spec;

    vec3 lighting = ambient + diffuse + specular;

    FragColor = vec4(lighting, 1.0);
}
