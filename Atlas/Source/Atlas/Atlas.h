#pragma once
#include "Render/RenderApi/DeviceTexture.h"
#include <Render/RenderApi.h>
#include <expected>
#include <nlohmann/json.hpp>
#include <Core/Serialization.h>
#include <format>
#include <Resource/Resource.h>
using namespace Aether;
struct AABB2D
{
    Vec2i min;
    Vec2i max;
};
namespace Aether
{
template <>
struct ToJson<AABB2D>
{
    Json operator()(const AABB2D& t)
    {
        Json json;
        json["min"] = ToJson<Vec2i>()(t.min);
        json["max"] = ToJson<Vec2i>()(t.max);
        return json;
    }
};
template <>
struct FromJson<AABB2D>
{
    std::expected<AABB2D, std::string> operator()(const Json& json)
    {
        try
        {
            auto json_min = json["min"];
            auto minEx= FromJson<Vec2i>()(json_min);
            if (!minEx.has_value())
            {
                return std::unexpected<std::string>(std::format("failed to load min: {}", minEx.error()));
            }
            auto json_max = json["max"];
            auto maxEx = FromJson<Vec2i>()(json_max);
            if (!maxEx.has_value())
            {
                return std::unexpected<std::string>(std::format("failed to load max: {}", maxEx.error()));
            }
            AABB2D aabb;
            aabb.min = minEx.value();
            aabb.max = maxEx.value();
            return aabb;
            
        }
        catch(...)
        {
            return std::unexpected<std::string>("unknown error");
        }
        
    }
};
} // namespace Aether

struct AtlasInfo
{
    Resource::ColorSpace colorSpace;
    std::string imagePath;
    /**
     * sequenceRange in this basis:
     * 0------> image width
     * |
     * |
     * |
     * V
     * image height
    */
    std::vector<AABB2D> sequenceRange; 
    float sequenceDuration = 0.25f;    // in seconds
    static std::optional<AtlasInfo> Load(const std::string_view path);
    bool Save(const std::string_view path);
};
namespace Aether
{
template <>
struct ToJson<AtlasInfo>
{
    Json operator()(const AtlasInfo& t)
    {
        Json json;
        json["colorSpace"] = ToJson<Resource::ColorSpace>()(t.colorSpace);
        json["imagePath"] = t.imagePath;
        json["sequenceRange"] = ToJson<std::vector<AABB2D>>{}(t.sequenceRange);
        json["sequenceDuration"] = t.sequenceDuration;
        return json;
    }
};
template <>
struct FromJson<AtlasInfo>
{
    std::expected<AtlasInfo, std::string> operator()(const Json& json)
    {
        
        try
        {
            AtlasInfo info;
            if (!json.is_object())
            {
                return std::unexpected<std::string>("json is not an object");
            }
            auto& json_colorSpace = json["colorSpace"];
            auto colorSpaceEx=FromJson<Resource::ColorSpace>{}(json_colorSpace);
            if(!colorSpaceEx)
            {
                return std::unexpected<std::string>(std::format("failed to load colorSpace: {}", colorSpaceEx.error()));
            }
            info.colorSpace = colorSpaceEx.value();
            auto& json_imagePath = json["imagePath"];
            auto imagePathEx= FromJson<std::string>{}(json_imagePath);
            if (!imagePathEx)
            {
                return std::unexpected<std::string>(std::format("failed to load imagePath: {}", imagePathEx.error()));
            }
            info.imagePath = std::move(imagePathEx.value());
            auto& json_sequenceRange = json["sequenceRange"];
            auto sequenceRangeEx = FromJson<std::vector<AABB2D>>{}(json_sequenceRange);
            if (!sequenceRangeEx)
            {
                return std::unexpected<std::string>(std::format("failed to load sequenceRange: {}", sequenceRangeEx.error()));
            }
            info.sequenceRange = std::move(sequenceRangeEx.value());
            auto& json_sequenceDuration = json["sequenceDuration"];
            float sequenceDuration = json_sequenceDuration.get<float>();
            info.sequenceDuration=sequenceDuration;

            return info;

        }
        catch (...)
        {
            return std::unexpected<std::string>("unknown error");
        }
    }
};
} // namespace Aether
struct Atlas
{
    AtlasInfo info;
    DeviceTexture texture;
};