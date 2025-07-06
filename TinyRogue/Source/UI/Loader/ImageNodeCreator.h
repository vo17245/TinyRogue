#pragma once
#include <UI/UI.h>
using namespace Aether;
struct ImageNodeCreator
{
    std::expected<UI::Node*, std::string> operator()(UI::Hierarchy& hierarchy,
                                                     const sol::table& luaNode)
    {
        UI::Node* node = hierarchy.CreateNode();
        sol::object luaWidth = luaNode["width"];
        auto widthOpt = UI::Lua::ParseNodeSize(luaWidth);
        if (!widthOpt)
        {
            return std::unexpected("Invalid width value in image node");
        }
        auto& width = *widthOpt;
        sol::object luaHeight = luaNode["height"];
        auto heightOpt = UI::Lua::ParseNodeSize(luaHeight);
        if (!heightOpt)
        {
            return std::unexpected("Invalid height value in image node");
        }
        auto& height = *heightOpt;
        std::optional<std::string> imagePath;
        auto luaPath = luaNode["path"];
        if (luaPath.get_type() != sol::type::string)
        {
            return std::unexpected("Image path must be a string in image node");
        }
        imagePath = luaPath;
        if (width.type == UI::Lua::NodeSizeType::Relative || height.type == UI::Lua::NodeSizeType::Relative)
        {
            auto& arc = hierarchy.AddComponent<UI::AutoResizeComponent>(node);
            if (width.type == UI::Lua::NodeSizeType::Relative)
            {
                arc.width = width.size / 100.0f;
            }
            if (height.type == UI::Lua::NodeSizeType::Relative)
            {
                arc.height = height.size / 100.0f;
            }
        }
        auto& base = hierarchy.AddComponent<UI::BaseComponent>(node);
        base.size = Vec2f(width.size, height.size);
        

        return node;
    }
};