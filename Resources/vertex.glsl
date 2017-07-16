uniform mat4 mvMatrix;
uniform mat4 mvpMatrix;
uniform mat3 normalMatrix;
uniform vec3 lightPosition;

attribute vec3 vertex;
attribute vec3 normal;
attribute vec2 texCoord;

varying mediump vec2 fragTexCoord;
varying mediump float diffuse;

void main()
{
    vec4 position4 = mvMatrix * vec4(vertex, 1.0);
    vec3 position3 = position4.xyz / position4.w;
    vec3 eyeNormal = normalize(normalMatrix * normal);
    vec3 lightDirection = normalize(lightPosition - position3);
    float diff = max(0.0, dot(eyeNormal, lightDirection));

    fragTexCoord = texCoord;
    diffuse = diff * 0.6 + 0.4;
    gl_Position = mvpMatrix * vec4(vertex, 1.0);
}
