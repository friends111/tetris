#version 330 core
layout(location = 0) in vec3 vertex_position;
layout(location = 1) in vec2 uv_coords;
uniform mat4 projection_matrix;
uniform mat4 model_matrix;
out vec4 pos;
out vec2 uv;

void main()
{
    gl_Position =  projection_matrix * model_matrix * vec4(vertex_position, 1.0);
    pos = vec4(vertex_position + 0.5, 1.0);
    uv = uv_coords;
}
