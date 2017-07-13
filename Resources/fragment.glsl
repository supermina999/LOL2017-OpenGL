uniform sampler2D sampler;

varying lowp vec2 fragTexCoord;

void main()
{
    gl_FragColor = texture2D(sampler, fragTexCoord);
}
