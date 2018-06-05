#version 330 core

// Take in the camera position to calculated specular angle / intensity
uniform vec3 viewPosition;

// --- Define a struct for the Light -----------------
struct Light {
	vec3 position;

	// Light components
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;
};

uniform Light light;

// --- Define a struct for the Material --------------
struct Material {
	sampler2D diffuse;
	sampler2D specular;
	sampler2D emission;
	float shininess;
};

uniform Material material;

uniform float emissionIntensity;
uniform float time;

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

void main()
{
	// --- Ambient Lighting -------------------------------------------------------
	vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;

	// --- Diffuse Lighting -------------------------------------------------------
	vec3 normal = normalize(Normal);
	vec3 lightDirection = normalize(light.position - FragPos);

	float diff = max(dot(normal, lightDirection), 0.0);
	vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;

	// --- Specular lighting ------------------------------------------------------
	// Calculate the view direction and corresponding reflect vector (along normal axis)
	vec3 viewDirection = normalize(viewPosition - FragPos);
	vec3 reflectDirection = reflect(-lightDirection, normal);

	// Calculate the specular component
	float specularPower = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);
	vec3 specular = light.specular * specularPower * vec3(texture(material.specular, TexCoords));


	// Add an emission map for giggles
	vec3 emission = emissionIntensity * vec3(texture(material.emission, TexCoords + vec2(0.0, time)));
	

	// result += (emissionIntensity * vec3(texture(material.emission, TexCoords + vec2(0.0, time))));

    FragColor = vec4(ambient + diffuse + specular + emission, 1.0);
}