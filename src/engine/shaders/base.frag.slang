import base;

[shader("fragment")]
float4 fragMain(VertexOutput input) : SV_Target {
    return texSampler.Sample(input.uv);
}
