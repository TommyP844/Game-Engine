#pragma once

#include "Mule/Mule Assert.h"

// Util
#include "Mule/Ref.h"
#include "Mule/Util/Buffer.h"
#include "Mule/Log.h"
#include "Mule/Util/Singleton.h"
#include "Mule/Util/Timer.h"

// Mono
#include "Mule/Scripting/CSTypeMapper.h"
#include "Mule/Scripting/MonoEngine.h"
#include "Mule/Scripting/MonoEngineClass.h"

#include "Mule/Asset/Asset.h"
#include "Mule/ECS/Components.h"
#include "Mule/ECS/Entity.h"
#include "Mule/ECS/Scene.h"
#include "Mule/ECS/Guid.h"
#include "Mule/Event.h"
#include "Mule/Transform.h"
#include "Mule/Window.h"
#include "Mule/WindowEvent.h"
#include "Mule/Layer.h"
#include "Mule/Asset/AssetManager.h"

// Graphics
#include "Mule/Graphics/GraphicsDevice.h"
#include "Mule/Graphics/GraphicsTypes.h"
#include "Mule/Graphics/FrameBuffer.h"
#include "Mule/Graphics/RenderPass.h"
#include "Mule/Graphics/ImGuiMuleContext.h"
#include "Mule/Graphics/Material.h"
#include "Mule/Graphics/Shader.h"
#include "Mule/Graphics/SceneRenderer.h"

// Serialization
#include "Serialization/MeshSerializer.h"

#include "imgui.h"

