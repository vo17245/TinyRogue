#pragma once
#include <Page/Page.h>
#include <Debug/Log.h>
class EditorPageBehavior : public PageBehaviorI
{
public:
    void OnInit(Page& page) override
    {
        Debug::Log::Debug("EditorPageBehavior OnInit");
        auto& hierarchy = page.GetHierarchy();
        auto* panel = hierarchy.GetNodeById("file_button_panel");
        
        assert(panel);
        { // file button
            auto* node = hierarchy.GetNodeById("btn:file");
            m_FileButton=node;
            assert(node);

            {
                auto& base = panel->GetComponent<UI::BaseComponent>();
                base.z = 10;
            }
            auto& mc = node->AddComponent<UI::MouseComponent>();
            mc.onClick = [panel, node, &hierarchy]() {
                auto& visible = panel->GetComponent<UI::VisibilityRequestComponent>();
                visible.visible = !visible.visible;
                visible.processed = false;
                auto& base = node->GetComponent<UI::BaseComponent>();
                auto& panelBase = panel->GetComponent<UI::BaseComponent>();
                panelBase.position = base.position + Vec2f(0, base.size.y());
                hierarchy.RequireRebuildLayout();
            };
        }
        { // file button panel

            auto* addImage = hierarchy.GetNodeById("btn:add_image");
            assert(addImage);
            auto& mc = addImage->AddComponent<UI::MouseComponent>();
            mc.onClick = [addImage,panel]() {
                auto& visible = panel->GetComponent<UI::VisibilityRequestComponent>();
                if (!visible.visible)
                {
                    return;
                }
                auto pathEx = UI::SyncSelectFile();
                Debug::Log::Debug("select path: {}", pathEx.has_value() ? pathEx.value() : "null");
            };
        }
        m_TerrainView = hierarchy.GetNodeById("terrain_view");
    }
    void OnEvent(Page& page, Event& event) override
    {
        if(std::holds_alternative<WindowResizeEvent>(event))
        {
            auto& resizeEvent= std::get<WindowResizeEvent>(event);
            assert(m_TerrainView);
            assert(m_FileButton);
            auto& base = m_TerrainView->GetComponent<UI::BaseComponent>();
            auto& btnBase= m_FileButton->GetComponent<UI::BaseComponent>();
            base.size.y()= resizeEvent.GetHeight() - btnBase.size.y();
        }
    }

private:
    UI::Node* m_TerrainView = nullptr;
    UI::Node* m_FileButton=nullptr;
};