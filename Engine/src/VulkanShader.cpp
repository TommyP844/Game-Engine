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
        vk::Device device = RenderAPI::GetDevice();
        device.destroyPipeline(mPipeline);
        device.destroyPipelineLayout(mPipelineLayout);
        if (mPushConstantData)
        {
            delete[] mPushConstantData;
        }
    }

    Ref<VulkanShader> VulkanShader::Create(const ShaderDescription& shaderDesc)
	{
		std::vector<vk::PipelineShaderStageCreateInfo> shaderStages;
        std::vector<vk::ShaderModule> modules;

        for (auto& shader : shaderDesc.Attachments)
        {
            std::vector<char> source = LoadSource(shader.Path);
            vk::ShaderModule module = LoadModule(source);
            vk::PipelineShaderStageCreateInfo info = CreateShaderStageInfo(shader.Stage, module);
            shaderStages.push_back(info);
            modules.push_back(module);
        }

        vk::PipelineVertexInputStateCreateInfo vertexInputInfo{};
        vertexInputInfo.sType = vk::StructureType::ePipelineVertexInputStateCreateInfo;

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

        vk::PipelineInputAssemblyStateCreateInfo inputAssembly{};
        inputAssembly.sType = vk::StructureType::ePipelineInputAssemblyStateCreateInfo;
        inputAssembly.topology = vk::PrimitiveTopology::eTriangleList;
        inputAssembly.primitiveRestartEnable = VK_FALSE;
        inputAssembly.pNext = NULL;

        vk::PipelineViewportStateCreateInfo viewportState{};
        viewportState.sType = vk::StructureType::ePipelineViewportStateCreateInfo;
        viewportState.viewportCount = 1;
        viewportState.pViewports = NULL;
        viewportState.scissorCount = 1;
        viewportState.pScissors = NULL;
        viewportState.pNext = NULL;

        vk::PipelineRasterizationStateCreateInfo rasterizer{};
        rasterizer.sType = vk::StructureType::ePipelineRasterizationStateCreateInfo;
        rasterizer.depthClampEnable = VK_FALSE;
        rasterizer.rasterizerDiscardEnable = VK_FALSE;
        rasterizer.polygonMode = vk::PolygonMode::eFill;
        rasterizer.lineWidth = 1.0f;
        rasterizer.cullMode = vk::CullModeFlagBits::eNone;
        rasterizer.frontFace = vk::FrontFace::eClockwise;
        rasterizer.depthBiasEnable = VK_FALSE;

        vk::PipelineMultisampleStateCreateInfo multisampling{};
        multisampling.sType = vk::StructureType::ePipelineMultisampleStateCreateInfo;
        multisampling.sampleShadingEnable = VK_FALSE;
        multisampling.rasterizationSamples = RenderAPI::GetSamples();

        vk::PipelineDepthStencilStateCreateInfo depthStencil{};
        depthStencil.sType = vk::StructureType::ePipelineDepthStencilStateCreateInfo;
        depthStencil.depthTestEnable = VK_TRUE;
        depthStencil.depthWriteEnable = VK_TRUE;
        depthStencil.depthCompareOp = vk::CompareOp::eLess;
        depthStencil.depthBoundsTestEnable = VK_FALSE;
        depthStencil.stencilTestEnable = VK_FALSE;

        vk::PipelineColorBlendAttachmentState colorBlendAttachment{};
        colorBlendAttachment.colorWriteMask = vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA;
        colorBlendAttachment.blendEnable = VK_FALSE;

        vk::PipelineColorBlendStateCreateInfo colorBlending{};
        colorBlending.sType = vk::StructureType::ePipelineColorBlendStateCreateInfo;
        colorBlending.logicOpEnable = VK_FALSE;
        colorBlending.logicOp = vk::LogicOp::eCopy;
        colorBlending.attachmentCount = 1;
        colorBlending.pAttachments = &colorBlendAttachment;
        colorBlending.blendConstants[0] = 0.0f;
        colorBlending.blendConstants[1] = 0.0f;
        colorBlending.blendConstants[2] = 0.0f;
        colorBlending.blendConstants[3] = 0.0f;

        std::vector<vk::DynamicState> dynamicStates = {
            vk::DynamicState::eScissor,
            vk::DynamicState::eViewport,
        };
        vk::PipelineDynamicStateCreateInfo dynamicState{};
        dynamicState.sType = vk::StructureType::ePipelineDynamicStateCreateInfo;
        dynamicState.dynamicStateCount = static_cast<uint32_t>(dynamicStates.size());
        dynamicState.pDynamicStates = dynamicStates.data();

        vk::PipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = vk::StructureType::ePipelineLayoutCreateInfo;
        pipelineLayoutInfo.pNext = nullptr;
        pipelineLayoutInfo.setLayoutCount = 0;
        pipelineLayoutInfo.pSetLayouts = nullptr;
        if (shaderDesc.Buffers.size() > 0)
        {
            std::vector<vk::DescriptorSetLayout> layouts;
            for (Ref<UniformBuffer> buffer : shaderDesc.Buffers)
            {
                auto descSet = buffer->GetDescriptorSetLayout();
                
                layouts.push_back(descSet);
            }



            pipelineLayoutInfo.setLayoutCount = shaderDesc.Buffers.size();
            pipelineLayoutInfo.pSetLayouts = &layouts[0];
        }

        Ref<VulkanShader> shader = MakeRef<VulkanShader>();

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
        vk::Device device = RenderAPI::GetDevice();

        shader->mPipelineLayout = device.createPipelineLayout(pipelineLayoutInfo);
        
        if (!shader->mPipelineLayout)
        {
            LOG_ERR(L"Failed to create shader");
        }

        vk::GraphicsPipelineCreateInfo pipelineInfo{};
        pipelineInfo.sType = vk::StructureType::eGraphicsPipelineCreateInfo;
        pipelineInfo.stageCount = shaderStages.size();
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
        vk::RenderPass renderPass = RenderAPI::GetRenderPass();
        pipelineInfo.renderPass = renderPass;
        pipelineInfo.subpass = 0;
        pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
        
        

        shader->mPipeline = device.createGraphicsPipeline(VK_NULL_HANDLE, pipelineInfo).value;

        for (auto& module : modules)
        {
            device.destroyShaderModule(module);
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
        vk::CommandBuffer buffer = RenderAPI::GetActiveCommandBuffer();
        buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, mPipeline);

        vk::Extent2D extent = RenderAPI::GetSwapChainExtent();

        vk::Viewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = (float)extent.width;
        viewport.height = (float)extent.height;
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        buffer.setViewport(0, viewport);

        vk::Rect2D scissor{};
        scissor.offset = vk::Offset2D{ 0, 0 };
        scissor.extent = extent;
        buffer.setScissor(0, scissor);
    }

    void VulkanShader::BindBuffer(Ref<UniformBuffer> uniformBuffer)
    {
    }

    vk::PipelineLayout VulkanShader::GetPipelineLayout()
    {
        return mPipelineLayout;
    }
       
	vk::PipelineShaderStageCreateInfo VulkanShader::CreateShaderStageInfo(ShaderStage stage, vk::ShaderModule module)
	{
        vk::ShaderStageFlagBits shaderStage{};
		switch (stage)
		{
		case ShaderStage::FRAGMENT: shaderStage = vk::ShaderStageFlagBits::eFragment; break;
		case ShaderStage::VERTEX: shaderStage = vk::ShaderStageFlagBits::eVertex; break;
		case ShaderStage::TESSELATION_CONTROL: shaderStage = vk::ShaderStageFlagBits::eTessellationControl; break;
		case ShaderStage::TESSELATION_EVALUATION: shaderStage = vk::ShaderStageFlagBits::eTessellationEvaluation; break;
		case ShaderStage::GEOMETRY: shaderStage = vk::ShaderStageFlagBits::eGeometry; break;
		case ShaderStage::COMPUTE: shaderStage = vk::ShaderStageFlagBits::eCompute; break;

		}
		vk::PipelineShaderStageCreateInfo shaderStageInfo{};
		shaderStageInfo.sType = vk::StructureType::ePipelineShaderStageCreateInfo;
		shaderStageInfo.stage = shaderStage;
		shaderStageInfo.module = module;
		shaderStageInfo.pName = "main";
        
		return shaderStageInfo;
	}
	
    vk::ShaderModule VulkanShader::LoadModule(const std::vector<char>& code)
	{

        vk::ShaderModuleCreateInfo createInfo{};
        createInfo.sType = vk::StructureType::eShaderModuleCreateInfo;
        createInfo.codeSize = code.size();
        createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

        vk::Device device = RenderAPI::GetDevice();
        vk::ShaderModule shaderModule = device.createShaderModule(createInfo);
        

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
        std::vector<vk::VertexInputAttributeDescription> attribs;
        uint32_t vertexSize = 0;
        uint32_t locationIndex = 0;
        for (const auto& elem : layout.Elements)
        {
            uint32_t size = GetTypeSize(elem.type) * elem.count;
            uint32_t locationOffset = size >> 16u;

            vk::VertexInputAttributeDescription attrib;

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

        vk::VertexInputBindingDescription inputDesc;
        inputDesc.binding = 0;
        inputDesc.stride = vertexSize;
        inputDesc.inputRate = vk::VertexInputRate::eVertex;

        VertexDescData desc;

        desc.VertexBindingInfo = inputDesc;
        desc.Attribs = attribs;

        return desc;
    }
    
    vk::Format VulkanShader::GetFormatFromType(Type type)
    {
        switch (type)
        {
        case Type::INT_16: return vk::Format::eR16Uint; break;
        case Type::UINT_16: return vk::Format::eR16Sint; break;
        case Type::INT_32: return vk::Format::eR32Sint; break;
        case Type::UINT_32: return vk::Format::eR32Uint; break;
        case Type::FLOAT_32: return vk::Format::eR32G32Sfloat; break;
        case Type::VEC2_I32: return vk::Format::eR32G32Sint; break;
        case Type::VEC2_UI32: return vk::Format::eR32G32Uint; break;
        case Type::VEC2_F32: return vk::Format::eR32G32Sfloat; break;
        case Type::INT_64: return vk::Format::eR64Sint; break;
        case Type::UINT_64: return vk::Format::eR64Uint; break;
        case Type::FLOAT_64: return vk::Format::eR64Sfloat; break;
        case Type::VEC3_I32: return vk::Format::eR32G32B32Sint; break;
        case Type::VEC3_UI32: return vk::Format::eR32G32B32Uint; break;
        case Type::VEC3_F32: return vk::Format::eR32G32B32Sfloat; break;
        case Type::VEC4_I32: return vk::Format::eR32G32B32A32Sint; break;
        case Type::VEC4_UI32: return vk::Format::eR32G32B32A32Uint; break;
        case Type::VEC4_F32: return vk::Format::eR32G32B32A32Sfloat; break;
            //TODO: Verify these
        case Type::MAT2X2_32F: return vk::Format::eR32G32B32A32Sfloat; break;
        case Type::MAT3X3_32F: return vk::Format::eR32G32B32Sfloat; break;
        case Type::MAT4X4_32F: return vk::Format::eR32G32B32A32Sfloat; break;
        }
        LOG_ERR(L"Invalid shader format");
        return vk::Format();
    }

    std::vector<vk::PushConstantRange> VulkanShader::GenPushConstants(const std::vector<ShaderAttachment>& attachments)
    {
        std::vector<vk::PushConstantRange> constants;

        int offset = 0;
        for (auto& attachment : attachments)
        {
            if (attachment.PushConstantSize == 0)
                continue;

            vk::PushConstantRange pushConst;

            pushConst.offset = offset; // TODO: may need to revisit
            pushConst.size = attachment.PushConstantSize;
            pushConst.stageFlags = GetShaderStage(attachment.Stage);

            offset += attachment.PushConstantSize;

            constants.push_back(pushConst);
        }

        return constants;
    }

    vk::ShaderStageFlagBits VulkanShader::GetShaderStage(ShaderStage stage)
    {
        switch (stage)
        {
        case ShaderStage::VERTEX: return vk::ShaderStageFlagBits::eVertex;
        case ShaderStage::FRAGMENT: return vk::ShaderStageFlagBits::eFragment;
        case ShaderStage::GEOMETRY: return vk::ShaderStageFlagBits::eGeometry;
        case ShaderStage::TESSELATION_CONTROL: return vk::ShaderStageFlagBits::eTessellationControl;
        case ShaderStage::TESSELATION_EVALUATION: return vk::ShaderStageFlagBits::eTessellationEvaluation;
        }
        return vk::ShaderStageFlagBits();
    }

}