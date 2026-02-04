#version 330 core

// specular - shinyness, diffuse - regular. material lights
struct Material {
	sampler2D texture_diffuse1;
	sampler2D texture_specular1;
	
	float shininess;
};

// point lights. small, fragments
struct PointLight {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	vec3 position;

	float constant;
	float linear;
	float quadratic;
};

// directional light (global light)
struct DirectionLight {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	vec3 direction;
};

// flashlight, two cones
// inner cone is 100% intensity, outer cone fading
struct SpotLight {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	vec3 position;
	vec3 direction;

	float innerCosAngle;
	float outerCosAngle;

	float constant;
	float linear;
	float quadratic;

	// koristi se za flashlight
	bool on;
};

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec4 diffuseTex, vec4 specularTex);

vec3 calcDirectionLight(DirectionLight light, vec3 normal, vec3 fragPos, vec4 diffuseTex, vec4 specularTex);

vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec4 diffuseTex, vec4 specularTex);

uniform Material material;

#define NO_OF_PLIGHTS 28
uniform PointLight pointLights[NO_OF_PLIGHTS];
uniform DirectionLight directionLight;
uniform SpotLight spotLight;

in vec3 Normal;
in vec3 FragPos;
in vec2 TexCoords;

out vec4 FragColor;

void main() {

	// textures from maps
	vec4 diffuseTex = texture(material.texture_diffuse1, TexCoords);
	vec4 specularTex = texture(material.texture_specular1, TexCoords);

	vec3 directionalLighting = calcDirectionLight(directionLight, Normal, FragPos, diffuseTex, specularTex);

	vec3 pointLighting = vec3(0.0);
	for (int i = 0; i < NO_OF_PLIGHTS; i++) {
		pointLighting += calcPointLight(pointLights[i], Normal, FragPos, diffuseTex, specularTex);
	}

	vec3 spotLighting = calcSpotLight(spotLight, Normal, FragPos, diffuseTex, specularTex);

	// final phong light
	vec3 phong = directionalLighting + pointLighting + spotLighting;
	FragColor = vec4(phong, 1.0f);

}

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec4 diffuseTex, vec4 specularTex) {
	
	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * pow(distance, 2));

	vec3 ambient;
	ambient = light.ambient * vec3(diffuseTex);

	vec3 diffuse;
	vec3 nNormal = normalize(normal);
	vec3 directionToLight = normalize(light.position - fragPos);
	float diffuseComponent = max(dot(directionToLight, nNormal), 0.0);
	diffuse = light.diffuse * diffuseComponent * vec3(diffuseTex);

	vec3 specular;
	vec3 directionToCamera = normalize(-fragPos);
	vec3 perfectRayReflection = reflect(-directionToLight, nNormal);
	float specularComponent = pow(max(dot(perfectRayReflection, directionToCamera), 0.0), material.shininess);
	specular = light.specular * specularComponent * vec3(specularTex);

	vec3 phong = (ambient + diffuse + specular) * attenuation;
	return phong;
}

vec3 calcDirectionLight(DirectionLight light, vec3 normal, vec3 fragPos, vec4 diffuseTex, vec4 specularTex) {
	vec3 ambient;
	ambient = light.ambient * vec3(diffuseTex);

	vec3 diffuse;
	vec3 nNormal = normalize(normal);
	vec3 directionToLight = normalize(-light.direction);
	float diffuseComponent = max(dot(directionToLight, nNormal), 0.0);
	diffuse = light.diffuse * diffuseComponent * vec3(diffuseTex);

	vec3 specular;
	vec3 directionToCamera = normalize(-fragPos);
	vec3 perfectRayReflection = reflect(-directionToLight, nNormal);
	float specularComponent = pow(max(dot(perfectRayReflection, directionToCamera), 0.0), material.shininess);
	specular = light.specular * specularComponent * vec3(specularTex);

	vec3 phong = ambient + diffuse + specular;
	return phong;
}

vec3 calcSpotLight(SpotLight light, vec3 normal, vec3 fragPos, vec4 diffuseTex, vec4 specularTex) {

	if (light.on == false) {
		return vec3(0.0);
	}

	vec3 ambient;
	ambient = light.ambient * vec3(diffuseTex);
	
	vec3 diffuse;
	vec3 nNormal = normalize(normal);
	vec3 directionToLight = normalize(light.position - fragPos);
	float diffuseComponent = max(dot(directionToLight, nNormal), 0.0);
	diffuse = light.diffuse * diffuseComponent * vec3(diffuseTex);

	vec3 specular;
	vec3 directionToCamera = normalize(-fragPos);
	vec3 perfectRayReflection = reflect(-directionToLight, nNormal);
	float specularComponent = pow(max(dot(perfectRayReflection, directionToCamera), 0.0), material.shininess);
	specular = light.specular * specularComponent * vec3(specularTex);

	float intensity;
	float fragAngle = dot(normalize(light.direction), normalize(-directionToLight));
	float softening = (fragAngle - light.outerCosAngle) / (light.innerCosAngle - light.outerCosAngle);
	intensity = clamp(softening, 0.0, 1.0);

	float distance = length(light.position - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * distance * distance);

	vec3 phong = (ambient + (diffuse + specular) * intensity) * attenuation;
	return phong;
}