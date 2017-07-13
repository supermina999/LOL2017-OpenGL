uniform mat4 transform;

attribute vec3 position;
attribute vec2 texCoord;

varying lowp vec2 fragTexCoord;

void main()
{
    fragTexCoord = texCoord;
    gl_Position = transform * vec4(position, 1.0);
}
