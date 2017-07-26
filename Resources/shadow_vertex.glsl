uniform mat4 mvpMatrix;

in vec3 vertex;

void main()
{
    gl_Position = mvpMatrix * vec4(vertex, 1.0);
}
