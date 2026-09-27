#version 450

vec2 vertices[] = {
	vec2(0.0f, 0.0f),
	vec2(0.5f, 0.5f),
	vec2(1.0f, 0.0f)
};

layout(location = 0) out vec2 vTexCoords;
layout(location = 1) out flat uint vTextureID;

layout(push_constant) uniform PcData {
	uint textureID;
} uPc;

void main() {
	vec2 pos = vertices[gl_VertexIndex];
	vTextureID = uPc.textureID;
	vTexCoords = pos;
	gl_Position = vec4(pos * 2.0f - 1.0f, 0.0f, 1.0f);
}