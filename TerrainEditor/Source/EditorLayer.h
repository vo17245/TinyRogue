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
#include <fstream>
#include "Page/Page.h"
#include "EditorPageBehavior.h"
using namespace Aether;
class EditorLayer : public Layer
{
public:
    ~EditorLayer()
    {
    }
    virtual void OnRender(
        vk::RenderPass& renderPass,
        vk::FrameBuffer& framebuffer,
        vk::GraphicsCommandBuffer& commandBuffer) override
    {
        auto& renderer = *ApplicationResource::s_Instance->renderer;
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
        Debug::Log::Debug("EditorLayer Attach");
        m_ScreenSize.x() = window->GetSize().x();
        m_ScreenSize.y() = window->GetSize().y();
        auto& appResource=*ApplicationResource::s_Instance;
        auto levelEx=Page::Load("Assets/Pages/terrain_editor.lua",m_Scene.get(),
        *ApplicationResource::s_Instance->renderResource.m_StagingBuffer,
                                m_TextureCache.get(),
                                appResource.renderer.get(),
                                appResource.renderResource.m_DescriptorPool.get(),
                                appResource.renderPass,
                                appResource.camera,
                                CreateScope<EditorPageBehavior>());
        if(!levelEx)
        {
            Debug::Log::Error("Failed to load level: {}", levelEx.error());
            return;
        }
        m_Page = std::move(levelEx.value());
        {
            Event e=WindowResizeEvent(window->GetSize().x(),window->GetSize().y());
            
            m_Page->OnEvent(e);
        }
       
    }
    virtual void OnEvent(Event& event) override
    {
        m_Page->OnEvent(event);
        if(std::holds_alternative<WindowResizeEvent>(event))
        {
            auto& resizeEvent = std::get<WindowResizeEvent>(event);
            m_ScreenSize.x() = resizeEvent.GetWidth();
            m_ScreenSize.y() = resizeEvent.GetHeight();
        }
    }
    virtual void OnDetach() override
    {
        m_Page.reset();
    }
    virtual void OnUpdate(float deltaSec)override
    {
        m_Page->OnUpdate(deltaSec);
    }
    virtual void OnFrameBegin() override
    {
        m_Page->OnFrameBegin();
    }
private:
   
    Vec2f m_ScreenSize;
    std::optional<Page> m_Page;
    std::unique_ptr<Scene> m_Scene;
    std::unique_ptr<UI::TextureCache> m_TextureCache;
};
