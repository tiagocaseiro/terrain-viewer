#version 330 core
layout(location = 0) in vec3 aPos;

uniform mat4 grid_model;
uniform mat4 cursor_model;
uniform mat4 view;
uniform mat4 projection;

out vec3 gridPosition;
out vec3 cursorPosition;

void main() {
    gl_Position    = projection * view * grid_model * vec4(aPos, 1.0);
    gridPosition   = vec3(grid_model * vec4(aPos, 1.0));
    gridPosition.y = 0;
    cursorPosition = vec3(cursor_model * vec4(vec3(0.0), 1.0));
}