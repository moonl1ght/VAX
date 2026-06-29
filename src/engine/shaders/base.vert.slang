import layoutAndBindings;
import sharedUtils;
import uniforms;

[shader("vertex")]
VertexOutput vertMain(uniform DrawPushConstants pushConstants, VertexInput input, uint instanceIndex : SV_VulkanInstanceID) {
    VertexOutput output;
    InstanceData instance = instances[instanceIndex];
    float4x4 modelMatrix = instance.model;
    if ((pushConstants.flags & ObjectFlags::PrecomputedMVP) != 0) {
        output.position = mul(modelMatrix, float4(input.position, 1.0));
    } else {
        output.position = mul(ubo.proj, mul(ubo.view, mul(modelMatrix, float4(input.position, 1.0))));
    }
    output.tangent = input.tangent;
    output.worldPosition = mul(modelMatrix, float4(input.position, 1.0)).xyz;
    output.normal = input.normal;
    output.uv = input.uv;
    output.uv2 = input.uv2;
    output.packedColor = input.packedColor;
    output.instanceIndex = instanceIndex;
    return output;
}
