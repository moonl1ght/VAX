#include "pipelineManager.h"
#include "pipelineBuilder.h"
#include "shaderModuleBuilder.h"
#include "descriptorSetManager.h"
#include "vkEngine.h"

using namespace vax::vk;

bool vax::vk::PipelineManager::setup(const vax::vk::RenderPass& renderPass) {
    auto shaderBuilder = vax::ShaderModuleBuilder(SRC_PATH("engine/shaders/out/shader.vert.spv"));
    auto vertShaderModule = shaderBuilder.build(_device.get().vkDevice);

    shaderBuilder = vax::ShaderModuleBuilder(SRC_PATH("engine/shaders/out/shader.frag.spv"));
    auto fragShaderModule = shaderBuilder.build(_device.get().vkDevice);

    shaderBuilder = vax::ShaderModuleBuilder(SRC_PATH("engine/shaders/out/background.comp.spv"));
    auto backgroundShaderModule = shaderBuilder.build(_device.get().vkDevice);

    if (!vertShaderModule || !fragShaderModule || !backgroundShaderModule) {
        LOG_ERROR("Failed to build shader module!");
        return false;
    }

    vax::vk::ComputePipelineBuilder backgroundPipelineBuilder(_device.get());
    VkPipelineLayoutCreateInfo computeLayoutInfo{};
    computeLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    computeLayoutInfo.pNext = nullptr;
    auto drawBackgroundDescriptorSetLayout = _descriptorSetManager.get().getDrawBackgroundDescriptorSetLayout();
    computeLayoutInfo.pSetLayouts = &drawBackgroundDescriptorSetLayout;
    computeLayoutInfo.setLayoutCount = 1;
    if (!backgroundPipelineBuilder.setPipelineLayout(computeLayoutInfo)) {
        return false;
    }

    backgroundPipelineBuilder.setShaderStage(VK_SHADER_STAGE_COMPUTE_BIT, backgroundShaderModule.value(), "main");
    auto backgroundPipeline = backgroundPipelineBuilder.build();
    if (backgroundPipeline.has_value()) {
        _backgroundPipeline = std::move(*backgroundPipeline);
    }
    else {
        return false;
    }

    VkPipelineShaderStageCreateInfo vertShaderStageInfo{};
    vertShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    vertShaderStageInfo.stage = VK_SHADER_STAGE_VERTEX_BIT;
    vertShaderStageInfo.module = vertShaderModule.value();
    vertShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo fragShaderStageInfo{};
    fragShaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
    fragShaderStageInfo.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
    fragShaderStageInfo.module = fragShaderModule.value();
    fragShaderStageInfo.pName = "main";

    VkPipelineShaderStageCreateInfo shaderStages[] = { vertShaderStageInfo, fragShaderStageInfo };

    VkPipelineVertexInputStateCreateInfo vertexInputInfo{};

    auto bindingDescription = Vertex::getBindingDescription();
    auto attributeDescriptions = Vertex::getAttributeDescriptions();

    vertexInputInfo.vertexBindingDescriptionCount = 1;
    vertexInputInfo.vertexAttributeDescriptionCount = static_cast<uint32_t>(attributeDescriptions.size());
    vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
    vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions.data();
    vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
    // vertexInputInfo.vertexBindingDescriptionCount = 0;
    // vertexInputInfo.vertexAttributeDescriptionCount = 0;

    VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
    inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
    inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
    inputAssembly.primitiveRestartEnable = VK_FALSE;

    VkPipelineViewportStateCreateInfo viewportState{};
    viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
    viewportState.viewportCount = 1;
    viewportState.scissorCount = 1;

    VkPipelineRasterizationStateCreateInfo rasterizer{};
    rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
    rasterizer.depthClampEnable = VK_FALSE;
    rasterizer.rasterizerDiscardEnable = VK_FALSE;
    rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
    rasterizer.lineWidth = 1.0f;
    rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
    rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
    rasterizer.depthBiasEnable = VK_FALSE;

    VkPipelineMultisampleStateCreateInfo multisampling{};
    multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
    multisampling.sampleShadingEnable = VK_FALSE;
    multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

    VkPipelineDepthStencilStateCreateInfo depthStencil{};
    depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
    depthStencil.depthTestEnable = VK_TRUE;
    depthStencil.depthWriteEnable = VK_TRUE;
    depthStencil.depthCompareOp = VK_COMPARE_OP_LESS;
    depthStencil.depthBoundsTestEnable = VK_FALSE;
    depthStencil.stencilTestEnable = VK_FALSE;

    VkPipelineColorBlendAttachmentState colorBlendAttachment{};
    colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT |
        VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
    colorBlendAttachment.blendEnable = VK_FALSE;

    VkPipelineColorBlendStateCreateInfo colorBlending{};
    colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
    colorBlending.logicOpEnable = VK_FALSE;
    colorBlending.logicOp = VK_LOGIC_OP_COPY;
    colorBlending.attachmentCount = 1;
    colorBlending.pAttachments = &colorBlendAttachment;
    colorBlending.blendConstants[0] = 0.0f;
    colorBlending.blendConstants[1] = 0.0f;
    colorBlending.blendConstants[2] = 0.0f;
    colorBlending.blendConstants[3] = 0.0f;

    std::vector<VkDynamicState> dynamicStates = {
        VK_DYNAMIC_STATE_VIEWPORT,
        VK_DYNAMIC_STATE_SCISSOR
    };
    VkPipelineDynamicStateCreateInfo dynamicState{};
    dynamicState.sType = VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
    dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
    dynamicState.pDynamicStates = dynamicStates.data();

    VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
    pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
    // pipelineLayoutInfo.setLayoutCount = 1;
    auto globalDescriptorSetLayout = _descriptorSetManager.get().getGlobalDescriptorSetLayout();
    // auto objectDescriptorSetLayout = _descriptorSetManager->getObjectDescriptorSetLayout();
    // pipelineLayoutInfo.pSetLayouts = &globalDescriptorSetLayout;
    pipelineLayoutInfo.setLayoutCount = 1;
    std::vector<VkDescriptorSetLayout> setLayouts = {
        globalDescriptorSetLayout
        // objectDescriptorSetLayout
    };
    pipelineLayoutInfo.pSetLayouts = setLayouts.data();

    VkPushConstantRange pushConstantRange{};
    pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
    pushConstantRange.offset = 0;
    pushConstantRange.size = sizeof(DrawPushConstants);

    pipelineLayoutInfo.pushConstantRangeCount = 1;
    pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange;

    auto result = vkCreatePipelineLayout(
        _device.get().vkDevice, &pipelineLayoutInfo, nullptr, &_pipelineLayout
    );
    if (result != VK_SUCCESS) {
        Logger::getInstance().error("failed to create pipeline layout!");
        return false;
    }

    VkGraphicsPipelineCreateInfo pipelineInfo{};
    pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
    pipelineInfo.stageCount = 2;
    pipelineInfo.pStages = shaderStages;
    pipelineInfo.pVertexInputState = &vertexInputInfo;
    pipelineInfo.pInputAssemblyState = &inputAssembly;
    pipelineInfo.pViewportState = &viewportState;
    pipelineInfo.pRasterizationState = &rasterizer;
    pipelineInfo.pMultisampleState = &multisampling;
    pipelineInfo.pDepthStencilState = &depthStencil;
    pipelineInfo.pColorBlendState = &colorBlending;
    pipelineInfo.pDynamicState = &dynamicState;
    pipelineInfo.layout = _pipelineLayout;
    pipelineInfo.renderPass = renderPass.get_vk_render_pass();
    // pipelineInfo.renderPass = vkEngine->renderPassManager->getRenderPass();
    pipelineInfo.subpass = 0;
    pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;

    auto pipelineResult = vkCreateGraphicsPipelines(
        _device.get().vkDevice, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &_pipeline
    );
    if (pipelineResult != VK_SUCCESS) {
        Logger::getInstance().error("failed to create graphics pipeline!");
        return false;
    }

    vkDestroyShaderModule(_device.get().vkDevice, fragShaderModule.value(), nullptr);
    vkDestroyShaderModule(_device.get().vkDevice, vertShaderModule.value(), nullptr);
    vkDestroyShaderModule(_device.get().vkDevice, backgroundShaderModule.value(), nullptr);
    return true;
}