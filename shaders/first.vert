#version 450

layout(location = 0) out vec2 fragUV;

vec2 positions[6] = vec2[](
    vec2(-1.0, -1.0),
    vec2( 1.0, -1.0),
    vec2(-1.0,  1.0),
    vec2(-1.0,  1.0),
    vec2( 1.0, -1.0),
    vec2( 1.0,  1.0)
);

layout(push_constant) uniform PushConstants
{
    mat4 view;
    mat4 _;
} push_constants;

void main()
{
    vec2 pos = positions[gl_VertexIndex];
    gl_Position = push_constants.view * vec4(pos, 0.0, 1.0);
    fragUV = pos * 0.5 + 0.5;
}
