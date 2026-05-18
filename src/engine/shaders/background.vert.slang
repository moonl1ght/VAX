import layoutAndBindings;

struct Coordinates {
    float4 position : SV_Position;
    float2 uv : TEXCOORD0;
};

[shader("vertex")]
Coordinates vertMain(VertexInput input) {
    Coordinates output;
    output.position = float4(input.position, 1.0);
    output.uv = input.uv;
    return output;
}
