import base;

[shader("vertex")]
VertexOutput vertMain(VertexInput input) {
    VertexOutput output;
    output.position = mul(ubo.proj, mul(ubo.view, mul(pushConstants.model, float4(input.position, 1.0))));
    output.normal = input.normal;
    output.uv = input.uv;
    return output;
}
