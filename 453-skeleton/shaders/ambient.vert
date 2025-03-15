#version 330 core
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 texture;
layout (location = 2) in vec3 normal;

// uniforms
uniform mat4 M;
uniform mat4 V;
uniform mat4 P;
uniform float scale;
uniform mat4 Mb;
uniform mat4 Mo;

// out variables
out vec2 text;

void main() {
	gl_Position = P * V * M * Mo * Mb * vec4(scale * pos, 1.0);

	// fragment position in world coordinates
	text = texture;
}
