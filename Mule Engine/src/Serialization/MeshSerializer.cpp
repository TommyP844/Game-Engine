#include "Serialization/MeshSerializer.h"
#include "Asset/AssetManager.h"
#include "Graphics/Mesh.h"
#include "Graphics/Vertex.h"

#define STB_IMAGE_WRITE_IMPLEMENTATION
#define TINYGLTF_IMPLEMENTATION
#include "tiny_gltf.h"

#include <glm.hpp>
#include <gtc/matrix_transform.hpp>

const std::string ATTRIBUTE_POSITION = "POSITION";
const std::string ATTRIBUTE_NORMAL = "NORMAL";
const std::string ATTRIBUTE_TANGENT = "TANGENT";

const std::string ATTRIBUTE_TEXCOORD_0 = "TEXCOORD_0";
const std::string ATTRIBUTE_TEXCOORD_1 = "TEXCOORD_1";
const std::string ATTRIBUTE_TEXCOORD_2 = "TEXCOORD_2";
const std::string ATTRIBUTE_TEXCOORD_3 = "TEXCOORD_3";

const std::string ATTRIBUTE_COLOR_0 = "COLOR_0";
const std::string ATTRIBUTE_COLOR_1 = "COLOR_1";
const std::string ATTRIBUTE_COLOR_2 = "COLOR_2";
const std::string ATTRIBUTE_COLOR_3 = "COLOR_3";

const std::string ATTRIBUTE_JOINTS_0 = "JOINTS_0";
const std::string ATTRIBUTE_JOINTS_1 = "JOINTS_1";
const std::string ATTRIBUTE_JOINTS_2 = "JOINTS_2";
const std::string ATTRIBUTE_JOINTS_3 = "JOINTS_3";

const std::string ATTRIBUTE_WEIGHTS_0 = "WEIGHTS_0";
const std::string ATTRIBUTE_WEIGHTS_1 = "WEIGHTS_1";
const std::string ATTRIBUTE_WEIGHTS_2 = "WEIGHTS_2";
const std::string ATTRIBUTE_WEIGHTS_3 = "WEIGHTS_3";

namespace Mule
{
    template<typename T>
    T* GetAccessorBuffer(const std::string& attribute_name, size_t& count, tinygltf::Model& model, tinygltf::Primitive& primitive);
    std::vector<AssetHandle> LoadNodeMeshes(tinygltf::Model& model, tinygltf::Mesh& mesh, const glm::mat4& offsetMatrix, WeakRef<GraphicsDevice> device);
    void TraverseNodes(const tinygltf::Node& gltfNode, MeshNode& node, MeshNode& parent, tinygltf::Model& model, WeakRef<GraphicsDevice> device);
    uint16_t* GetIndices(tinygltf::Primitive& primitive, tinygltf::Model& model, size_t& count);

    std::future<Ref<ModelDescription>> DeserializeModelAsync(WeakRef<GraphicsDevice> device, const fs::path& filepath, AssetHandle handle)
    {
        auto model = std::async(std::launch::async, [=]() {
            return DeserializeModel(device, filepath, handle);
            });

        return model;
    }

    Ref<ModelDescription> DeserializeModel(WeakRef<GraphicsDevice> device, const fs::path& filepath, AssetHandle handle)
    {
        AssetManager& manager = AssetManager::Get();

        tinygltf::TinyGLTF loader;
        tinygltf::Model model;
        std::string error, warning;
        if (!fs::exists(filepath))
        {
            MULE_LOG_ERROR("File doesnt exist: {0}", filepath.string());
            return nullptr;
        }
        bool ret = loader.LoadASCIIFromFile(&model, &error, &warning, filepath.string());
        if (!error.empty()) MULE_LOG_ERROR(error, "");
        if (!warning.empty()) MULE_LOG_WARNING(warning, "");
        if(!ret) return nullptr;

        auto modelDescription = ModelDescription::Create(handle, filepath);

        MeshNode root;
        root.OffsetMatrix = glm::identity<glm::mat4>();
        TraverseNodes(model.nodes[0], modelDescription->Root, root, model, device);
        
        return modelDescription;
    }

    template<typename T>
    T* GetAccessorBuffer(const std::string& attribute_name, size_t& count, tinygltf::Model& model, tinygltf::Primitive& primitive)
    {
        if (primitive.attributes.find(attribute_name) == primitive.attributes.end())
            return nullptr;

        auto& accessor = model.accessors[primitive.attributes[attribute_name]];
        auto& bufferView = model.bufferViews[accessor.bufferView];
        auto& buffer = model.buffers[bufferView.buffer];

        // Calculate the stride and offset
        T* data = (T*)(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);
        count = accessor.count;
        const size_t stride = accessor.ByteStride(bufferView) ? accessor.ByteStride(bufferView) / sizeof(T) : 3;
        if (accessor.ByteStride(bufferView) == -1 || accessor.ByteStride(bufferView) != sizeof(T))
        {
            MULE_LOG_ERROR("Model Accessor: {0} stride does not match type stride of {1}", attribute_name, std::string(typeid(T).name()));
        }

        return data;
    }

    uint16_t* GetIndices(tinygltf::Primitive& primitive, tinygltf::Model& model, size_t& count)
    {
        auto& accessor = model.accessors[primitive.indices];
        auto& bufferView = model.bufferViews[accessor.bufferView];
        auto& buffer = model.buffers[bufferView.buffer];

        // Calculate the stride and offset
        uint16_t* indices = (uint16_t*)(&buffer.data[bufferView.byteOffset + accessor.byteOffset]);
        count = accessor.count;
        if (accessor.ByteStride(bufferView) == -1 || accessor.ByteStride(bufferView) != sizeof(uint16_t))
        {
            MULE_LOG_ERROR("Model Index Accessor stride does not match type stride of {1}", std::string(typeid(uint16_t).name()));
        }

        return indices;
    }

    void TraverseNodes(const tinygltf::Node& gltfNode, MeshNode& node, MeshNode& parent, tinygltf::Model& model, WeakRef<GraphicsDevice> device)
    {
        auto& mesh = model.meshes[gltfNode.mesh];
        if (!gltfNode.matrix.empty())
        {
            for (int x = 0; x < 4; x++)
                for (int y = 0; y < 4; y++)
                    node.OffsetMatrix = gltfNode.matrix[y * 4 + x];
            node.OffsetMatrix = parent.OffsetMatrix * node.OffsetMatrix;
            // TODO: may need to transpose
        }
        else
        {
            node.OffsetMatrix = glm::identity<glm::mat4>();
        }
        node.Meshes = LoadNodeMeshes(model, mesh, node.OffsetMatrix, device);
        for (auto child : gltfNode.children)
        {
            MeshNode childNode;
            TraverseNodes(model.nodes[child], childNode, node, model, device);
            node.Children.push_back(childNode);
        }
    }

    std::vector<AssetHandle> LoadNodeMeshes(tinygltf::Model& model, tinygltf::Mesh& mesh, const glm::mat4& offsetMatrix, WeakRef<GraphicsDevice> device)
    {
        std::vector<AssetHandle> Meshes;

        for (auto& primitive : mesh.primitives) {
            // Access the position accessor
            size_t count = 0;
            glm::vec3* positions = GetAccessorBuffer<glm::vec3>(ATTRIBUTE_POSITION, count, model, primitive);
            glm::vec2* uv_0 = GetAccessorBuffer<glm::vec2>(ATTRIBUTE_TEXCOORD_0, count, model, primitive);
            glm::vec3* normals = GetAccessorBuffer<glm::vec3>(ATTRIBUTE_NORMAL, count, model, primitive);
            glm::vec3* tangents = GetAccessorBuffer<glm::vec3>(ATTRIBUTE_TANGENT, count, model, primitive);
            glm::vec4* colors = GetAccessorBuffer<glm::vec4>(ATTRIBUTE_COLOR_0, count, model, primitive);

            Buffer<float> vertexBuffer(count * sizeof(Vertex));
            for (size_t i = 0; i < count; ++i) {
                Vertex vertex;
                vertex.Position = positions[i];
                vertex.Normal = normals[i];
                vertex.Tangent = tangents ? tangents[i] : glm::vec3(0.f);
                vertex.TexCoord_0 = uv_0 ? uv_0[i] : glm::vec2(0.f);
                vertex.Color = colors ? colors[i] : glm::vec4(0.f);

                //MULE_LOG_VERBOSE("{0}, {1}, {2}", vertex.Position.x, vertex.Position.y, vertex.Position.z);
                //MULE_LOG_VERBOSE("{0}, {1}", vertex.TexCoord_0.x, vertex.TexCoord_0.y);
                //MULE_LOG_VERBOSE("{0}, {1}, {2}", vertex.Normal.x, vertex.Normal.y, vertex.Normal.z);
                //MULE_LOG_VERBOSE("{0}, {1}, {2}", vertex.Tangent.x, vertex.Tangent.y, vertex.Tangent.z);
                //MULE_LOG_VERBOSE("{0}, {1}, {2}, {3}", vertex.Color.x, vertex.Color.y, vertex.Color.z, vertex.Color.w);

                vertexBuffer.As<Vertex>(i) = vertex;
            }

            size_t indexCount = 0;
            uint16_t* indices = GetIndices(primitive, model, indexCount);
            Buffer<uint16_t> indexBuffer(indexCount);
            for (int i = 0; i < indexCount; i++)
            {
                indexBuffer[i] = indices[i];
            }

            MeshDescription meshDesc;
            meshDesc.Handle = Asset::GenerateHandle();
            meshDesc.Context = device->GetMainThreadContext();
            meshDesc.OffsetMatrix = offsetMatrix;
            meshDesc.Name = mesh.name;
            meshDesc.Vertices = vertexBuffer;
            meshDesc.Indices = indexBuffer;
            meshDesc.VertexStride = sizeof(Vertex);

            AssetManager& manager = AssetManager::Get();
            auto meshAsset = Mesh::Create(device, meshDesc);
            manager.InsertAsset(meshAsset);

            Meshes.push_back(meshAsset->Handle());
        }

        return Meshes;
    }
}
