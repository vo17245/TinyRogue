#include "Entry/Application.h"
#include "ApplicationResource.h"
#include <print>
#include "UpdateLayer.h"
#include "EditorLayer.h"
#include <Audio/Audio.h>
using namespace Aether;

class AtlasEditor : public Application
{
public:
    virtual void OnInit(Window& window) override
    {
        ApplicationResource::Init(window.GetSize(),window.GetFinalTexture());
        auto& appResource = *ApplicationResource::s_Instance;
        appResource.SetMainWindow(&window);
        m_MainWindow = &window;
        PushLayer<UpdateLayer>();
        PushLayer<EditorLayer>();
        
    }
    template<typename T>
    void PushLayer()
    {
        m_Layers.push_back(new T());
        m_MainWindow->PushLayer(m_Layers.back());
    }
    virtual void OnShutdown()override
    {
        for(auto* layer:m_Layers)
        {
            m_MainWindow->PopLayer(layer);
            delete layer;
        }
        ApplicationResource::Destroy();
    }
    virtual void OnFrameBegin()override
    {
        ApplicationResource::s_Instance->renderResource.m_DescriptorPool->Clear();
        ApplicationResource::s_Instance->renderer->OnFrameBegin();

    }
    virtual const char* GetName() const override
    {
        return "AtlasEditor";
    }
private:
    std::vector<Layer*> m_Layers;
    Window* m_MainWindow;
};
DEFINE_APPLICATION(AtlasEditor);
