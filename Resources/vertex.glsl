uniform mat4 mvMatrix;
uniform mat4 mvpMatrix;
uniform mat3 normalMatrix;
uniform vec3 lightPosition;

in vec3 vertex;
in vec3 normal;
in vec2 texCoord;

out highp vec2 fs_texCoord;
out highp vec3 fs_normal;
out highp vec3 fs_lightDirection;

void main()
{
    vec4 position4 = mvMatrix * vec4(vertex, 1.0);
    vec3 position3 = position4.xyz / position4.w;
    fs_normal = normalize(normalMatrix * normal);
    fs_lightDirection = normalize(lightPosition - position3);
    fs_texCoord = texCoord;

    gl_Position = mvpMatrix * vec4(vertex, 1.0);
}
