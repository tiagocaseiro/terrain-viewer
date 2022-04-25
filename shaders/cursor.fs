#version 330 core
out vec4 FragColor;

uniform vec3 color;
uniform float radius;

in vec3 gridPosition;
in vec3 cursorPosition;

void main() {
    FragColor = vec4(vec3(1.0), 0.0);
    if (gridPosition.y == 0.0) {
        float length = length(cursorPosition - gridPosition);
        if (length <= radius) {
            FragColor = vec4(color, 0.8);
        }
    }
}