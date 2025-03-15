#version 330 core

// inputs
in vec3 frag_pos;
in vec2 text;
in vec3 norm;

// uniforms
uniform vec3 camera_pos;
uniform vec3 light_pos;
uniform sampler2D sampler;

// outputs
out vec4 color;

void main() {
    // object color
    vec4 col_tex = texture(sampler, text);

    vec3 col_white = vec3(1, 1, 1);

    // ambient
    vec3 Ia = 0.1 * col_white;

    // diffused
    vec3 n = normalize(norm);
    vec3 l = normalize(light_pos - frag_pos);

    float kd = 1;
    vec3 Id = kd * max(dot(n, l), 0) * col_white;

    // specular
    float ks = 1;
    vec3 v = normalize(camera_pos - frag_pos);
    vec3 r = 2 * dot(n, l) * n - l;
    vec3 Is = ks * pow(max(dot(r, v), 0), 32) * col_white;

    // apply light weight and color to fragment color
    color = vec4(Ia + Id + Is, 1) * col_tex;
}
