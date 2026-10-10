#version 450
#include "core.glslh"

layout(location = 0) out vec4 vFragColor;

layout(location = 0) in vec2 vTexCoords;

layout(location = 1) in flat uint vTextureID;
layout(location = 2) in flat uint vSamplerID;

void main() {
	vec4 texColor = texture(GetBindlessTextureFromID(vTextureID, vSamplerID), vTexCoords);
	vFragColor = texColor;
}