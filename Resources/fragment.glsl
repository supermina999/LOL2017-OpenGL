uniform sampler2D sampler;
uniform vec4 lightingParams;

varying mediump vec2 varyingTexCoord;
varying mediump vec3 varyingNormal;
varying mediump vec3 varyingLightDirection;

void main()
{
    vec4 color = texture2D(sampler, varyingTexCoord);
    float diffuse = max(0.0, dot(varyingNormal, varyingLightDirection));
    color = color * (diffuse * lightingParams.y + lightingParams.x);

    vec3 reflection;
    float specular;
    if(diffuse != 0.0)
    {
        reflection = normalize(reflect(-varyingLightDirection, varyingNormal));
        specular = max(0.0, dot(varyingNormal, reflection));
        specular = pow(specular, lightingParams.w) * lightingParams.z;
        color.rgb += vec3(specular, specular, specular);
    }
    gl_FragColor = color;
}
