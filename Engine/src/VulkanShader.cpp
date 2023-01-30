#include "mulepch.h"

#include "Mule/Renderer/Vulkan/VulkanShader.h"
#include "Mule/Renderer/RenderAPI.h"
#include "Mule/Renderer/UniformBuffer.h"
#include "Mule/Util/Log.h"

#include <fstream>

namespace Mule
{
    VulkanShader::VulkanShader()
        :
        mPipeline(),
        mPipelineLayout(),
        mPushConstants({ 0 }),
        mPushConstantData(NULL)
    {
    }
    
    VulkanShader::~VulkanShader()
    {
        VkDevice device = RenderAPI::GetDevice();
        vkDestroyPipeline(device, mPipeline, nullptr);
        vkDestroyPipelineLayout(device, mPipelineLayout, nullptr);
        if (mPushConstantData)
        {
            delete[] mPushConstantData;
        }
    }

    Ref<VulkanShader> VulkanShader::Create(const ShaderDescription& shaderDesc)
	{
		std::vector<VkPipelineShaderStageCreateInfo> shaderStages;
        std::vector<VkShaderModule> modules;

        for (auto& shader : shaderDesc.Attachments)
        {
            std::vector<char> source = LoadSource(shader.Path);
            VkShaderModule module = LoadModule(source);
            VkPipelineShaderStageCreateInfo info = CreateShaderStageInfo(shader.Stage, module);
            shaderStages.push_back(info);
            modules.push_back(module);
        }

        VkPipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;

        // If the user passes in an empty struct we want to make sure we dont use default binding
        if (shaderDesc.VertexLayout.Elements.size() == 0)
        {
            vertexInputInfo.vertexBindingDescriptionCount = 0;
            vertexInputInfo.vertexAttributeDescriptionCount = 0;
        }
        else
        {
            VertexDescData desc = GetDescriptions(shaderDesc.VertexLayout);

            vertexInputInfo.vertexBindingDescriptionCount = 1;
            vertexInputInfo.pVertexBindingDescriptions = &desc.VertexBindingInfo;
            vertexInputInfo.vertexAttributeDescriptionCount = (uint32_t)desc.Attribs.size();
            vertexInputInfo.pVertexAttributeDescriptions = desc.Attribs.data();
        }

        VkPipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
        inputAssembly.topology = VkPrimitiveTopology::VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
        inputAssembly.primitiveRestartEnable = VK_FALSE;
        inputAssembly.pNext = NULL;

        VkPipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
        viewportState.viewportCount = 1;
        viewportState.scissorCount = 1;

        VkPipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = VkPolygonMode::VK_POLYGON_MODE_FILL;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = VkCullModeFlagBits::VK_CULL_MODE_BACK_BIT;
        rasterizer.frontFace = VkFrontFace::VK_FRONT_FACE_COUNTER_CLOCKWISE;
        rasterizer.depthBiasEnable = VK_FALSE;

        VkPipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = RenderAPI::GetSamples();

        VkPipelineDepthStencilStateCreateInfo depthStencil{};
        depthStencil.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
        depthStencil.depthTestEnable = VK_TRUE;
        depthStencil.depthWriteEnable = VK_TRUE;
        depthStencil.depthCompareOp = VkCompareOp::VK_COMPARE_OP_LESS;
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        depthStencil.stencilTestEnable = VK_FALSE;

        VkPipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = VkColorComponentFlagBits::VK_COLOR_COMPONENT_R_BIT | VkColorComponentFlagBits::VK_COLOR_COMPONENT_G_BIT | VkColorComponentFlagBits::VK_COLOR_COMPONENT_B_BIT | VkColorComponentFlagBits::VK_COLOR_COMPONENT_A_BIT;
        colorBlendAttachment.blendEnable = VK_FALSE;

        VkPipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = VkLogicOp::VK_LOGIC_OP_COPY;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;

        std::vector<VkDynamicState> dynamicStates = {
            VkDynamicState::VK_DYNAMIC_STATE_VIEWPORT,
            VkDynamicState::VK_DYNAMIC_STATE_SCISSOR
        };
        VkPipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_DYNAMIC_STATE_CREATE_INFO;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        pipelineLayoutInfo.pNext = nullptr;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        if (shaderDesc.Buffers.size() > 0)
        {
            std::vector<VkDescriptorSetLayout> layouts;
            for (Ref<UniformBuffer> buffer : shaderDesc.Buffers)
            {
                auto descSet = buffer->GetDescriptorSetLayout();
                
                layouts.push_back(descSet);
            }

            pipelineLayoutInfo.setLayoutCount = (uint32_t)shaderDesc.Buffers.size();
            pipelineLayoutInfo.pSetLayouts = &layouts[0];
        }

        VkDevice device = RenderAPI::GetDevice();

        Ref<VulkanShader> shader = MakeRef<VulkanShader>();

        vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &shader->mPipelineLayout);

        auto pushConstants = GenPushConstants(shaderDesc.Attachments);

        shader->mPushConstants = pushConstants;
        if (pushConstants.size() > 0)
        {
            pipelineLayoutInfo.pushConstantRangeCount = pushConstants.size();
            pipelineLayoutInfo.pPushConstantRanges = &pushConstants[0];
        }
        else
        {
            pipelineLayoutInfo.pushConstantRangeCount = 0;
            pipelineLayoutInfo.pPushConstantRanges = nullptr;
        }

        VkGraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
        pipelineInfo.stageCount = (uint32_t)shaderStages.size();
        pipelineInfo.pStages = shaderStages.data();
        pipelineInfo.pVertexInputState = &vertexInputInfo;
        pipelineInfo.pInputAssemblyState = &inputAssembly;
        pipelineInfo.pViewportState = &viewportState;
        pipelineInfo.pRasterizationState = &rasterizer;
        pipelineInfo.pMultisampleState = &multisampling;
        pipelineInfo.pDepthStencilState = &depthStencil;
        pipelineInfo.pColorBlendState = &colorBlending;
        pipelineInfo.pDynamicState = &dynamicState;
        pipelineInfo.layout = shader->mPipelineLayout;
        VkRenderPass renderPass = RenderAPI::GetRenderPass();
        pipelineInfo.renderPass = renderPass;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
        
        vkCreateGraphicsPipelines(device, VK_NULL_HANDLE, 1, &pipelineInfo, nullptr, &shader->mPipeline);

        for (auto& module : modules)
        {
            vkDestroyShaderModule(device, module, nullptr);
        }   

		return shader;
	}


    void VulkanShader::BindPushConstant(ShaderStage stage, void* data)
    {
        auto commandBuffer = RenderAPI::GetActiveCommandBuffer();
        for (const auto& constant : mPushConstants)
        {
            if (constant.stageFlags == GetShaderStage(stage))
            {
                vkCmdPushConstants(commandBuffer, mPipelineLayout, (uint32_t)constant.stageFlags, 0, constant.size, data);
                break;
            }
        }
        
    }

    void VulkanShader::Bind()
    {
        VkCommandBuffer buffer = RenderAPI::GetActiveCommandBuffer();
        vkCmdBindPipeline(buffer, VK_PIPELINE_BIND_POINT_GRAPHICS, mPipeline);

        VkExtent2D extent = RenderAPI::GetSwapChainExtent();

        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)extent.width;
        viewport.height = (float)extent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        vkCmdSetViewport(buffer, 0, 1, &viewport);

        VkRect2D scissor{};
        scissor.offset = VkOffset2D{ 0, 0 };
        scissor.extent = extent;
        vkCmdSetScissor(buffer, 0, 1, &scissor);
    }

    void VulkanShader::BindBuffer(Ref<UniformBuffer> uniformBuffer)
    {
    }

    VkPipelineLayout VulkanShader::GetPipelineLayout()
    {
        return mPipelineLayout;
    }
       
	VkPipelineShaderStageCreateInfo VulkanShader::CreateShaderStageInfo(ShaderStage stage, VkShaderModule module)
	{
        VkShaderStageFlagBits shaderStage{};
		switch (stage)
		{
		case ShaderStage::FRAGMENT: shaderStage = VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT; break;
		case ShaderStage::VERTEX: shaderStage = VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT; break;
		case ShaderStage::TESSELATION_CONTROL: shaderStage = VkShaderStageFlagBits::VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT; break;
		case ShaderStage::TESSELATION_EVALUATION: shaderStage = VkShaderStageFlagBits::VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT; break;
		case ShaderStage::GEOMETRY: shaderStage = VkShaderStageFlagBits::VK_SHADER_STAGE_GEOMETRY_BIT; break;
		case ShaderStage::COMPUTE: shaderStage = VkShaderStageFlagBits::VK_SHADER_STAGE_COMPUTE_BIT; break;

		}
		VkPipelineShaderStageCreateInfo shaderStageInfo{};
		shaderStageInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStageInfo.stage = shaderStage;
		shaderStageInfo.module = module;
        const char* name = "main";
		shaderStageInfo.pName = name;
        
		return shaderStageInfo;
	}
	
    VkShaderModule VulkanShader::LoadModule(const std::vector<char>& code)
	{
        VkShaderModuleCreateInfo createInfo{};
        createInfo.sType = VkStructureType::VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());
        createInfo.pNext = NULL;

        VkDevice device = RenderAPI::GetDevice();
        VkShaderModule shaderModule;
        vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule);

        return shaderModule;
	}
    
    std::vector<char> VulkanShader::LoadSource(const std::filesystem::path& filepath)
    {
        std::fstream file(filepath, std::ios_base::binary | std::ios_base::in);
        if (!file.is_open())
        {
            LOG_ERR(L"Failed to open file \"{0}\"", filepath.wstring());
            return {};
        }
        std::string line;
        std::vector<char> source;
        
        file.seekg(0, std::ios::end);
        int length = file.tellg();
        source.resize(length);
        file.seekg(0, std::ios::beg);

        file.read(&source[0], length);

        file.close();
        return source;
    }
    
    VulkanShader::VertexDescData VulkanShader::GetDescriptions(const VertexLayout& layout)
    {
        std::vector<VkVertexInputAttributeDescription> attribs;
        uint32_t vertexSize = 0;
        uint32_t locationIndex = 0;
        for (const auto& elem : layout.Elements)
        {
            uint32_t size = GetTypeSize(elem.type) * elem.count;
            uint32_t locationOffset = size >> 16u;

            VkVertexInputAttributeDescription attrib;

            attrib.binding = 0;
            attrib.location = locationIndex;
            attrib.format = GetFormatFromType(elem.type);
            attrib.offset = vertexSize;

            attribs.push_back(attrib);

            if (locationOffset > 1)
            {
                locationIndex += locationOffset;
            }
            else
            {
                locationIndex += 1;
            }
            vertexSize += size;
        }

        VkVertexInputBindingDescription inputDesc;
        inputDesc.binding = 0;
        inputDesc.stride = vertexSize;
        inputDesc.inputRate = VkVertexInputRate::VK_VERTEX_INPUT_RATE_VERTEX;

        VertexDescData desc;

        desc.VertexBindingInfo = inputDesc;
        desc.Attribs = attribs;

        return desc;
    }
    
    VkFormat VulkanShader::GetFormatFromType(Type type)
    {
        switch (type)
        {
        case Type::INT_16: return VkFormat::VK_FORMAT_R16_UINT; break;
        case Type::UINT_16: return VkFormat::VK_FORMAT_R16_SINT; break;
        case Type::INT_32: return VkFormat::VK_FORMAT_R32_SINT; break;
        case Type::UINT_32: return VkFormat::VK_FORMAT_R32_UINT; break;
        case Type::FLOAT_32: return VkFormat::VK_FORMAT_R32_SFLOAT; break;
        case Type::VEC2_I32: return VkFormat::VK_FORMAT_R32G32_SINT; break;
        case Type::VEC2_UI32: return VkFormat::VK_FORMAT_R32G32_UINT; break;
        case Type::VEC2_F32: return VkFormat::VK_FORMAT_R32G32_SFLOAT; break;
        case Type::INT_64: return VkFormat::VK_FORMAT_R64_SINT; break;
        case Type::UINT_64: return VkFormat::VK_FORMAT_R64_UINT; break;
        case Type::FLOAT_64: return VkFormat::VK_FORMAT_R64_SFLOAT; break;
        case Type::VEC3_I32: return VkFormat::VK_FORMAT_R32G32B32_SINT; break;
        case Type::VEC3_UI32: return VkFormat::VK_FORMAT_R32G32B32_UINT; break;
        case Type::VEC3_F32: return VkFormat::VK_FORMAT_R32G32B32_SFLOAT; break;
        case Type::VEC4_I32: return VkFormat::VK_FORMAT_R32G32B32A32_SINT; break;
        case Type::VEC4_UI32: return VkFormat::VK_FORMAT_R32G32B32A32_UINT; break;
        case Type::VEC4_F32: return VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT; break;
            //TODO: Verify these
        case Type::MAT2X2_32F: return VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT; break;
        case Type::MAT3X3_32F: return VkFormat::VK_FORMAT_R32G32B32_SFLOAT; break;
        case Type::MAT4X4_32F: return VkFormat::VK_FORMAT_R32G32B32A32_SFLOAT; break;
        }
        LOG_ERR(L"Invalid shader format");
        return VkFormat();
    }

    std::vector<VkPushConstantRange> VulkanShader::GenPushConstants(const std::vector<ShaderAttachment>& attachments)
    {
        std::vector<VkPushConstantRange> constants;

        int offset = 0;
        for (auto& attachment : attachments)
        {
            if (attachment.PushConstantSize == 0)
                continue;

            VkPushConstantRange pushConst;

            pushConst.offset = offset; // TODO: may need to revisit
            pushConst.size = attachment.PushConstantSize;
            pushConst.stageFlags = GetShaderStage(attachment.Stage);

            offset += attachment.PushConstantSize;

            constants.push_back(pushConst);
        }

        return constants;
    }

    VkShaderStageFlagBits VulkanShader::GetShaderStage(ShaderStage stage)
    {
        switch (stage)
        {
        case ShaderStage::VERTEX: return VkShaderStageFlagBits::VK_SHADER_STAGE_VERTEX_BIT;
        case ShaderStage::FRAGMENT: return VkShaderStageFlagBits::VK_SHADER_STAGE_FRAGMENT_BIT;
        case ShaderStage::GEOMETRY: return VkShaderStageFlagBits::VK_SHADER_STAGE_GEOMETRY_BIT;
        case ShaderStage::TESSELATION_CONTROL: return VkShaderStageFlagBits::VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
        case ShaderStage::TESSELATION_EVALUATION: return VkShaderStageFlagBits::VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
        }
        return VkShaderStageFlagBits();
    }

}