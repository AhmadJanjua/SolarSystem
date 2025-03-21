#version 330 core

in vec2 text;

uniform sampler2D sampler;

out vec4 color;

void main() {
	color = texture(sampler, text);
}
