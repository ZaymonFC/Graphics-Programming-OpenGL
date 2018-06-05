#version 330 core

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
  
uniform vec3 objectColor;
uniform vec3 lightColor;
uniform vec3 lightPosition;

// Take in the camera position to calculated specular angle / intensity
uniform vec3 viewPosition;

// Define a struct for the material
//struct Material {
//	vec3 ambient;
//	vec3 diffuse;
//	vec3 specular;
//	float shininess;
//};

uniform Material material;

void main()
{
	// Ambient Lighting -------------------------------------------------------
	float ambientStrength = 0.1;
	vec3 ambient = ambientStrength * lightColor;

	// Diffuse Lighting -------------------------------------------------------
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(lightPosition - FragPos);

	float diff = max(dot(normal, lightDirection), 0.0);
	vec3 diffuse = diff * lightColor;

	// Specular lighting ------------------------------------------------------
	float specularStrength = 0.5;

	// Calculate the view direction and corresponding reflect vector along
	// the normal axis
	vec3 viewDirection = normalize(viewPosition - FragPos);
	vec3 reflectDirection = reflect(-lightDirection, normal);

	// Calculate the specular component
	float specular = pow(max(dot(viewDirection, reflectDirection), 0.0), 16);

	// Calculate the resulting colour
	vec3 result = (ambient + diffuse + specular) * objectColor;
    FragColor = vec4(result, 1.0);
}