#pragma once
#include "Core/Math/Def.h"
#include "Render/RenderApi/DeviceFrameBuffer.h"
#include "Render/RenderApi/DeviceRenderPass.h"
#include "Render/RenderApi/DeviceTexture.h"
#include "Render/Vulkan/DescriptorSet.h"
#include "Render/Vulkan/GraphicsCommandPool.h"
#include "Render/Vulkan/Pipeline.h"
#include "Render/Vulkan/PipelineLayout.h"
#include "Render/Vulkan/RenderContext.h"
#include "Render/Vulkan/ShaderModule.h"
#include "UI/Hierarchy/Component/Base.h"
#include "UI/Hierarchy/Component/Text.h"
#include "UI/Render/DynamicStagingBuffer.h"
#include "UI/Render/Quad.h"
#include "UI/Render/TextureCache.h"
#include "Window/Layer.h"
#include "Window/Window.h"
#include "Window/WindowContext.h"
#include "Render/Vulkan/GlobalRenderContext.h"
#include "Render/Mesh/VkMesh.h"
#include "Render/Mesh/Geometry.h"
#include "Render/Shader/Compiler.h"
#include "Render/Utils.h"
#include <print>
#include <variant>
#include "UI/Render/Renderer.h"
#include "UI/Render/Filter/Gamma.h"
#include "UI/Render/LoadTextureToLinear.h"
#include "Resource/Finder.h"
#include "Window/WindowEvent.h"
#include "ApplicationResource.h"
#include <UI/Hierarchy/Hierarchy.h>
#include <UI/Hierarchy/System/Quad.h>
#include <UI/Hierarchy/Loader/HierarchyLuaLoader.h>
#include <UI/Hierarchy/Loader/NodeCreator.h>
#include <UI/Hierarchy/Loader/BuiltinXmlNodeCreator.h>
#include <UI/Hierarchy/System/Text.h>
#include <UI/Hierarchy/System/Mouse.h>
#include <UI/Hierarchy/System/InputText.h>
#include <UI/Hierarchy/Loader/BuiltinLuaNodeCreator.h>
#include <UI/Hierarchy/System/AutoResize.h>
#include <miniaudio/miniaudio.h>
#include <fstream>
#include "UI/Page.h"
using namespace Aether;
class TerrainEditorLayer : public Layer
{
public:
    ~TerrainEditorLayer()
    {
    }
    virtual void OnRender(
        vk::RenderPass& renderPass,
        vk::FrameBuffer& framebuffer,
        vk::GraphicsCommandBuffer& commandBuffer) override
    {
        auto& renderer = *ApplicationResource::s_Instance->renderer;
        auto& camera = ApplicationResource::s_Instance->camera;
        camera.target = Vec2f(m_ScreenSize.x() / 2, m_ScreenSize.y() / 2);
        std::vector<VkClearValue> clearValues(2);
        clearValues[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};
        commandBuffer.BeginRenderPass(ApplicationResource::s_Instance->renderPass.GetVk(),
                                      ApplicationResource::s_Instance->frameBuffer.GetVk(),
                                      clearValues);
        m_Page->OnRender(commandBuffer, renderPass, framebuffer,
                             m_ScreenSize);
        commandBuffer.EndRenderPass();
    }
    virtual void OnAttach(Window* window) override
    {
        m_Scene=CreateScope<Scene>();
        m_TextureCache=CreateScope<UI::TextureCache>(*ApplicationResource::s_Instance->renderResource.m_StagingBuffer);
        InitAudio();
        Debug::Log::Debug("TerrainEditorLayer Attach");
        m_ScreenSize.x() = window->GetSize().x();
        m_ScreenSize.y() = window->GetSize().y();
        auto levelEx=Page::Load("Assets/Pages/terrain_editor.lua",m_Scene.get(),
        *ApplicationResource::s_Instance->renderResource.m_StagingBuffer,
                                m_TextureCache.get());
        if(!levelEx)
        {
            Debug::Log::Error("Failed to load level: {}", levelEx.error());
            return;
        }
        m_Page = std::move(levelEx.value());
       
    }
    

    virtual void OnEvent(Event& event) override
    {
        m_Page->OnEvent(event);
    }
    virtual void OnDetach() override
    {
        ma_engine_uninit(&m_AudioEngine);
        m_Page.reset();
    }
private:
    bool InitAudio()
    {
        auto result = ma_engine_init(NULL, &m_AudioEngine);
        if (result != MA_SUCCESS) {
            Debug::Log::Error("Failed to initialize audio engine: {}", (int)(result));
            return false;
        }
        return true;
    }
    Vec2f m_ScreenSize;
    ma_engine m_AudioEngine = {0};
    std::optional<Page> m_Page;
    std::unique_ptr<Scene> m_Scene;
    std::unique_ptr<UI::TextureCache> m_TextureCache;
};
