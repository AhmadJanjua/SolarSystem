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
out vec3 frag_pos;
out vec2 text;
out vec3 norm;

void main() {
	vec4 position = Mo * Mb * vec4(scale * pos, 1.0);
	gl_Position = P * V * M * position;

	// out
	frag_pos = vec3(M * position);
	text = texture;
	norm = vec3(Mb * vec4(normal, 1));
}
