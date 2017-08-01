precision highp float;

uniform highp sampler2D sampler;
uniform highp sampler2DShadow shadowSampler;

uniform vec4 lightingParams;

in highp vec2 fs_texCoord;
in highp vec3 fs_normal;
in highp vec3 fs_lightDirection;
in highp vec4 fs_shadowCoord;

out vec4 res_color;

void main()
{
    vec4 color = texture(sampler, fs_texCoord);

    float shadowFactor = textureProj(shadowSampler, fs_shadowCoord);

    float diffuse = max(0.0, dot(fs_normal, fs_lightDirection));
    color = color * (diffuse * lightingParams.y + lightingParams.x);

    vec3 reflection;
    float specular;
    if(diffuse != 0.0 && shadowFactor == 1.0)
    {
        reflection = normalize(reflect(-fs_lightDirection, fs_normal));
        specular = max(0.0, dot(fs_normal, reflection));
        specular = pow(specular, lightingParams.w) * lightingParams.z;
        color.rgb += vec3(specular, specular, specular);
    }
    res_color = vec4(color.xyz * (shadowFactor + 1.0) / 2.0, color.w);
}
