#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
  
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPosition;

void main()
{
	// Ambient Lighting
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * lightColor;

	// Diffuse Lighting
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(lightPosition - FragPos);

	float diff = max(dot(normal, lightDirection), 0.0);
	vec3 diffuse = diff * lightColor;

	// Calculate the resulting colour
	vec3 result = (ambient + diffuse) * objectColor;
    FragColor = vec4(result, 1.0);
}