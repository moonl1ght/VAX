import layoutAndBindings;
import sharedUtils;

[shader("vertex")]
VertexOutput vertMain(VertexInput input) {
    VertexOutput output;
    output.position = mul(ubo.proj, mul(ubo.view, mul(pushConstants.model, float4(input.position, 1.0))));
    output.normal = input.normal;
    output.tangent = input.tangent;
    output.uv = input.uv;
    output.uv2 = input.uv2;
    output.packedColor = input.packedColor;
    return output;
}
