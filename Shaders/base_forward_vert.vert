#version 450

vec2 vertices[6] = {
	vec2(0.0f, 0.0f), // Bottom-left
	vec2(1.0f, 0.0f), // Bottom-right
	vec2(0.0f, 1.0f), // Top-left

	vec2(0.0f, 1.0f), // Top-left
	vec2(1.0f, 0.0f), // Bottom-right
	vec2(1.0f, 1.0f)  // Top-right
};

layout(location = 0) out vec2 vTexCoords;

layout(location = 1) out flat uint vTextureID;
layout(location = 2) out flat uint vSamplerID;

layout(push_constant) uniform PcData {
	uint textureID;
	uint samplerID;
} uPc;

void main() {
	vec2 pos = vertices[gl_VertexIndex];
	vTextureID = uPc.textureID;
	vSamplerID = uPc.samplerID;
	vTexCoords = pos;
	gl_Position = vec4(pos * 2.0f - 1.0f, 0.0f, 1.0f);
}