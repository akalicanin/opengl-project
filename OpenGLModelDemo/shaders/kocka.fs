#version 330 core

in vec2 vs_TexCoords;

uniform sampler2D u_tex_Kutija;
uniform sampler2D u_tex_As;

uniform float u_MixAmount;

out vec4 fs_Colors;

void main() {
	vec4 tex_Kutija = texture(u_tex_Kutija, vs_TexCoords);
	vec4 tex_As = texture(u_tex_As, vs_TexCoords);
	float finalniAlpha = tex_As.a + u_MixAmount;
	if (finalniAlpha >= 1.0f) {
		finalniAlpha = 1.0f;
	} 
	else if (finalniAlpha <= 0.0f) {
		finalniAlpha = 0.0f;
	}
	vec4 miksTekstura = mix(tex_Kutija, tex_As, finalniAlpha);
	fs_Colors = miksTekstura;
}