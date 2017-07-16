uniform mat4 mvMatrix;
uniform mat4 mvpMatrix;
uniform mat3 normalMatrix;
uniform vec3 lightPosition;

attribute vec3 vertex;
attribute vec3 normal;
attribute vec2 texCoord;

varying mediump vec2 varyingTexCoord;
varying mediump vec3 varyingNormal;
varying mediump vec3 varyingLightDirection;

void main()
{
    vec4 position4 = mvMatrix * vec4(vertex, 1.0);
    vec3 position3 = position4.xyz / position4.w;
    varyingNormal = normalize(normalMatrix * normal);
    varyingLightDirection = normalize(lightPosition - position3);
    varyingTexCoord = texCoord;

    gl_Position = mvpMatrix * vec4(vertex, 1.0);
}
