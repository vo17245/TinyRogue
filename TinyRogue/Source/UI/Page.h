#pragma once
#include <UI/UI.h>
#include <sol/sol.hpp>
#include "../ApplicationResource.h"
#include "UI/Render/DynamicStagingBuffer.h"
#include "UI/Render/TextureCache.h"
using namespace Aether;
class Page 
{
public:    
    ~Page()
    {
        if (m_Hierarchy)
        {
            m_Hierarchy.reset();
        }
    }
    Page(const Page&) = delete;
    Page& operator=(const Page&) = delete;
    Page(Page&&) = default;
    Page& operator=(Page&&) = default;
    Page(Borrow<Scene> scene,Borrow<UI::DynamicStagingBuffer> stagingBuffer,Borrow<UI::TextureCache> textureCache)
    :m_Scene(scene),m_TextureCache(textureCache){}
public:
    void OnRender(DeviceCommandBufferView commandBuffer,
                  DeviceRenderPassView renderPass,
                  DeviceFrameBufferView frameBuffer,
                  Vec2f screenSize)
    {
        m_Hierarchy->OnRender(commandBuffer, renderPass, frameBuffer, screenSize);
    }
    void OnEvent(Event& event)
    {
        m_Hierarchy->OnEvent(event);
    }
    static std::expected<Page,std::string> Load(const std::string& pagePath,Borrow<Scene> scene,
    Borrow<UI::DynamicStagingBuffer> stagingBuffer,
    Borrow<UI::TextureCache> textureCache)
    {
        Page Page(scene,stagingBuffer,textureCache);
      
        
        
        // create hierarchy
        Page.m_Hierarchy=std::make_unique<UI::Hierarchy>(Page.m_Scene);
        auto error=Page.InitHierarchy(pagePath);
        if(error)
        {
            return std::unexpected(error.value());
        }
        return Page;
    }    
    bool IsLoaded() const
    {
        return m_Hierarchy != nullptr && m_Scene != nullptr;
    }

private:
    std::optional<std::string> InitHierarchy(const std::string& hierarchyLuaPath)
    {
        // quad
        UI::QuadSystem* quadSystem = new UI::QuadSystem(m_TextureCache);
        quadSystem->SetCamera(&m_Hierarchy->GetCamera());
        quadSystem->renderer = ApplicationResource::s_Instance->renderer.get();
        m_Hierarchy->AddSystem(quadSystem);
        // text
        UI::TextSystem* textSystem = UI::TextSystem::Create(ApplicationResource::s_Instance->renderPass,
                                                            *ApplicationResource::s_Instance->renderResource.m_DescriptorPool);
        textSystem->SetDescriptorPool(ApplicationResource::s_Instance->renderResource.m_DescriptorPool.get());
        textSystem->SetCamera(&m_Hierarchy->GetCamera());
        textSystem->AddAssetDir("Assets");
        m_Hierarchy->AddSystem(textSystem);
        // mouse
        UI::MouseSystem* mouseSystem = new UI::MouseSystem();
        m_Hierarchy->AddSystem(mouseSystem);
        // input text
        UI::InputTextSystem* inputTextSystem = new UI::InputTextSystem();
        m_Hierarchy->AddSystem(inputTextSystem);
        // auto resize
        UI::AutoResizeSystem* autoResizeSystem=new UI::AutoResizeSystem();
        m_Hierarchy->AddSystem(autoResizeSystem);


        UI::Lua::HierarchyLoader loader;
        loader.PushNodeCreator<UI::Lua::GridNodeCreator>("grid");
        loader.PushNodeCreator<UI::Lua::QuadNodeCreator>("quad");
        loader.PushNodeCreator<UI::Lua::TextNodeCreator>("text");
        auto hierarchyStrOpt=Filesystem::ReadFileToString(hierarchyLuaPath);
        if(!hierarchyStrOpt)
        {
            return std::format("Failed to read hierarchy file: {}", hierarchyLuaPath) ;
        }
        auto& hierarchyStrValue = *hierarchyStrOpt;

        auto err=loader.LoadHierarchy(*m_Hierarchy, hierarchyStrValue);
        if(err)
        {
            return "Failed to load hierarchy: " + err.value();
        }



        autoResizeSystem->SetRoot(m_Hierarchy->GetRoot());
        {
            Event e=WindowResizeEvent(800,600);
            autoResizeSystem->OnEvent(e, m_Hierarchy->GetScene());
        }
        auto& camera = ApplicationResource::s_Instance->camera;
        auto& window=ApplicationResource::s_Instance->GetMainWindow();
        m_Hierarchy->RebuildLayout();
        return std::nullopt;
    }
    std::unique_ptr<UI::Hierarchy> m_Hierarchy;//means ui system in Aether
    Borrow<Scene> m_Scene;
    Borrow<UI::TextureCache> m_TextureCache;

};