uniform sampler2D sampler;

varying mediump vec2 fragTexCoord;
varying mediump float diffuse;

void main()
{
    vec4 color = texture2D(sampler, fragTexCoord);
    gl_FragColor = vec4(color.xyz * diffuse, 1.0);
}
