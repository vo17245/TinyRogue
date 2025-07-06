#pragma once
#include <IO/Image.h>
#include <vector>
#include <string>
#include <optional>
#include <print>
#include <Atlas/Atlas.h>
#include <RectangleBinPack/MaxRectsBinPack.h>
struct CreateAtlasParam
{
    std::vector<std::string> sequencePaths;
    std::string root;               // output root
    std::string outputPath;         // realative to root
    float sequenceDuration = 0.25f; // in seconds
    bool linear = false;            // linear or srgb
};
bool BuildLayout(const std::vector<Image>& images, const Vec2u& binSize, /*out*/ std::vector<Vec2u>& pos)
{
    RectangleBinPack::MaxRectsBinPack maxRects(binSize.x(), binSize.y(), false);
    struct ImageSize
    {
        Vec2u size;
        uint32_t imageIndex;
    };
    std::vector<ImageSize> imageSizes;
    for (size_t i = 0; i < images.size(); ++i)
    {
        auto& image = images[i];
        imageSizes.emplace_back(ImageSize{Vec2u{image.GetWidth(), image.GetHeight()}, (uint32_t)i});
    }
    std::sort(imageSizes.begin(), imageSizes.end(), [](const ImageSize& a, const ImageSize& b) {
        return a.size.x() * a.size.y() > b.size.x() * b.size.y(); // Sort by area descending
    });
    pos.resize(images.size());
    for (auto& size : imageSizes)
    {
        auto res = maxRects.Insert(size.size.x(), size.size.y(), RectangleBinPack::MaxRectsBinPack::RectBestAreaFit);
        if (res.height == 0)
        {
            return false;
        }
        pos[size.imageIndex] = Vec2u{(uint32_t)res.x, (uint32_t)res.y};
    }
    return true;
}
std::optional<std::string> CreateAtlas(CreateAtlasParam& param)
{
    // ensure output dir
    std::string outputDir = param.root + "/" + param.outputPath;
    if (!Filesystem::Exists(outputDir))
    {
        Filesystem::CreateDirectories(outputDir);
    }
    if (!Filesystem::IsDirectory(outputDir))
    {
        return std::string("Output path is not a directory: ") + outputDir;
    }
    // load images
    std::vector<Image> images;
    for (auto path : param.sequencePaths)
    {
        auto imageEx = Image::LoadFromFile(path);
        if (!imageEx)
        {
            return std::string("Failed to load image: ") + path + " - " + imageEx.error();
        }
        images.push_back(std::move(imageEx.value()));
    }
    // build layout
    std::vector<Vec2u> imagePositions;
    uint32_t width = 16;
    uint32_t height = 16;
    {
        int scale = 1;
        while (true)
        {
            imagePositions.clear();
            if (BuildLayout(images, Vec2u{width, height}, imagePositions))
            {
                break;
            }
            if (scale)
            {
                width *= 2;
            }
            else
            {
                height *= 2;
            }
            scale = 1 - scale;
        }
    }
    // create image
    Image atlasImage = Image::CreateRgba8(width, height);
    // fill image
    for (size_t i = 0; i < images.size(); ++i)
    {
        auto& image = images[i];
        auto& pos = imagePositions[i];
        if (image.Empty())
        {
            continue;
        }
        for (uint32_t y = 0; y < image.GetHeight(); ++y)
        {
            for (uint32_t x = 0; x < image.GetWidth(); ++x)
            {
                uint8_t* srcPixel = image.GetData() + (y * image.GetRowBytes()) + (x * image.GetChannels());
                uint8_t* dstPixel = atlasImage.GetData() + ((pos.y() + y) * atlasImage.GetRowBytes()) + ((pos.x() + x) * atlasImage.GetChannels());
                std::copy(srcPixel, srcPixel + image.GetChannels(), dstPixel);
            }
        }
    }
    // save atlas image
    std::string atlasImagePath = outputDir + "/atlas.png";
    atlasImage.SaveToPngFile(atlasImagePath.c_str());
    // create atlas info
    AtlasInfo atlasInfo;
    if (param.linear)
    {
        atlasInfo.colorSpace = Resource::ColorSpace::LINEAR;
    }
    else
    {
        atlasInfo.colorSpace = Resource::ColorSpace::SRGB;
    }
    atlasInfo.imagePath = param.outputPath + "/atlas.png";
    atlasInfo.sequenceDuration = param.sequenceDuration;
    for (size_t i = 0; i < images.size(); ++i)
    {
        auto& image = images[i];
        auto& pos = imagePositions[i];
        AABB2D range{Vec2f{(float)pos.x(), (float)pos.y()}, Vec2f{(float)(pos.x() + image.GetWidth()), (float)(pos.y() + image.GetHeight())}};
        atlasInfo.sequenceRange.push_back(range);
    }
    // save atlas info
    std::string atlasInfoPath = outputDir + "/atlas.json";
    if (!atlasInfo.Save(atlasInfoPath))
    {
        return std::string("Failed to save atlas info: ") + atlasInfoPath;
    }
    return std::nullopt; // success
}
int HandleCreateAtlasAction(int argc, char** argv)
{
    if(argc<5)
    {
        std::print("Usage: %s create_atlas <output_path> <duration> <sequence_path1> <sequence_path2> ...\n", argv[0]);
        return 1;
    }
    CreateAtlasParam param;
    param.outputPath = argv[2];
    param.sequenceDuration = std::stof(argv[3]);
    for (int i = 4; i < argc; ++i)
    {
        param.sequencePaths.push_back(argv[i]);
    }

    auto err = CreateAtlas(param);
    if (err)
    {
        std::print("Error: {}", err.value());
        return 1;
    }
    std::print("Atlas created successfully at: {}\n", param.outputPath);
    return 0;
}