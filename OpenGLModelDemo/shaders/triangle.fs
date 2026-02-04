#version 330 core
in vec2 vsTexCoords;

out vec4 boje;

uniform sampler2D texture1;
uniform sampler2D texture2;

uniform float mixAmount = 0.0;

void main() {
	vec4 boxTexture = texture(texture1, vsTexCoords);
	vec4 aceTexture = texture(texture2, vsTexCoords);
	float finalAlpha = aceTexture.a + mixAmount;
	if (finalAlpha >= 1.0) {
		finalAlpha = 1.0;
	} else if (finalAlpha <= 0.0) {
		finalAlpha = 0.0;
	}
	vec4 mixedTextures = mix(boxTexture, aceTexture, finalAlpha);
	boje = mixedTextures;
}