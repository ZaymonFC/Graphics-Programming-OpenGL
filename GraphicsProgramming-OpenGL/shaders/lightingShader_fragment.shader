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
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	sampler2D texture_normal1;
	sampler2D emission;
	float shininess;
};

uniform sampler2D shadowMap;

uniform Material material;

uniform float emissionIntensity;
uniform float time;

out vec4 FragColor;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;
in vec4 FragPosLightSpace;

float ShadowCalculation(vec4 fragPosLightSpace)
{
	// perform perspective divide
	vec3 projCoords = fragPosLightSpace.xyz / fragPosLightSpace.w;
	// transform to [0,1] range
	projCoords = projCoords * 0.5 + 0.5;
	// get closest depth value from light's perspective (using [0,1] range fragPosLight as coords)
	float closestDepth = texture(shadowMap, projCoords.xy).r;
	// get depth of current fragment from light's perspective
	float currentDepth = projCoords.z;

	float bias = 0.005;

	// check whether current frag pos is in shadow
	float shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;

	return shadow;
}

void main()
{
	// Calculate the normal from the normal map
	vec3 normal = texture(material.texture_normal1, TexCoords).rgb;
	// Transform the normal vector to range [-1, 1]
	normal = normalize(normal * 2.0 - 1.0);

	// --- Ambient Lighting -------------------------------------------------------
	vec3 ambient = light.ambient * texture(material.texture_diffuse1, TexCoords).rgb;

	// --- Diffuse Lighting -------------------------------------------------------
	normal = normalize((Normal + normal)/2);
	vec3 lightDirection = normalize(light.position - FragPos);

	float diff = max(dot(normal, lightDirection), 0.0);
	vec3 diffuse = light.diffuse * diff * texture(material.texture_diffuse1, TexCoords).rgb;

	// --- Specular lighting ------------------------------------------------------
	// Calculate the view direction and corresponding reflect vector (along normal axis)
	vec3 viewDirection = normalize(viewPosition - FragPos);
	vec3 reflectDirection = reflect(-lightDirection, normal);

	// Calculate the specular component
	float specularPower = pow(max(dot(viewDirection, reflectDirection), 0.0), material.shininess);
	vec3 specular = light.specular * specularPower * vec3(texture(material.texture_specular1, TexCoords));


	// Add an emission map for giggles
	vec3 emission = emissionIntensity * vec3(texture(material.emission, TexCoords + vec2(0.0, time)));
	

	// result += (emissionIntensity * vec3(texture(material.emission, TexCoords + vec2(0.0, time))));

	// Calculate shadow
	float shadow = ShadowCalculation(FragPosLightSpace);

    FragColor = vec4(ambient + ((1.0 - shadow) * (diffuse + specular)) + emission, 1.0);
}