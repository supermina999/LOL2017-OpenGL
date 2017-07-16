uniform sampler2D sampler;

varying mediump vec2 varyingTexCoord;
varying mediump vec3 varyingNormal;
varying mediump vec3 varyingLightDirection;

void main()
{
    vec4 color = texture2D(sampler, varyingTexCoord);
    float diffuse = max(0.0, dot(varyingNormal, varyingLightDirection));
    float colorCoef = diffuse * 0.6 + 0.4;
    gl_FragColor = vec4(color.xyz * colorCoef, 1.0);
}
