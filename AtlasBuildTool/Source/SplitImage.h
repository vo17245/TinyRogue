#pragma once
#include <IO/Image.h>
#include <vector>
#include <string>
#include <optional>
#include <print>
#include <Atlas/Atlas.h>
#include <RectangleBinPack/MaxRectsBinPack.h>

struct SplitImageParam
{
    std::string imagePath;
    std::string outputDir;
    Vec2u rectSize;
};

int SplitImage(const SplitImageParam& param)
{
    // ensure output dir
    if(!Filesystem::Exists(param.outputDir))
    {
        Filesystem::CreateDirectories(param.outputDir);
    }
    if(!Filesystem::IsDirectory(param.outputDir))
    {
        std::print("Output path is not a directory: {}\n", param.outputDir);
        return 1;
    }
    // load image
    auto imageEx=Image::LoadFromFile(param.imagePath);
    if(!imageEx)
    {
        return 1;
    }
    auto& image = *imageEx;
    uint32_t rows=(uint32_t)(image.GetHeight()/param.rectSize.y());
    uint32_t cols=(uint32_t)(image.GetWidth()/param.rectSize.x());
    if(rows*cols==0)
    {
        std::print("Image size is too small for the given rect size.\n");
        return 1;
    }
    auto getPixel=[](Image& image,uint32_t x,uint32_t y)->Vec4u 
    {
        if(x >= image.GetWidth() || y >= image.GetHeight())
        {
            return Vec4u(0, 0, 0, 0);
        }
        auto* data= image.GetData();
        return Vec4u(data[(y * image.GetWidth() + x) * 4 + 0],
                     data[(y * image.GetWidth() + x) * 4 + 1],
                     data[(y * image.GetWidth() + x) * 4 + 2],
                     data[(y * image.GetWidth() + x) * 4 + 3]);
    };
    auto setPixel=[](Image& image,uint32_t x,uint32_t y,const Vec4u& pixel)->void
    {
        auto* data= image.GetData();
        if(x < image.GetWidth() && y < image.GetHeight())
        {
            data[(y * image.GetWidth() + x) * 4 + 0] = pixel.x();
            data[(y * image.GetWidth() + x) * 4 + 1] = pixel.y();
            data[(y * image.GetWidth() + x) * 4 + 2] = pixel.z();
            data[(y * image.GetWidth() + x) * 4 + 3] = pixel.w();
        }
    };
    for(size_t row=0;row<rows;++row)
    {
        for(size_t col=0;col<cols;++col)
        {
            Image subImage=Image::CreateRgba8(param.rectSize.x(), param.rectSize.y());
            for(size_t y=0;y<param.rectSize.y();++y)
            {
                for(size_t x=0;x<param.rectSize.x();++x)
                {
                    setPixel(subImage,x, y, getPixel(image,col*param.rectSize.x()+x, row*param.rectSize.y()+y));
                }
            }

            auto imageName=std::format("image_{}_{}.png", row, col);
            // save
            auto outputPath = std::format("{}/{}", param.outputDir, imageName);
            if(!subImage.SaveToPngFile(outputPath.c_str()))
            {
                std::print("Failed to save image: {}\n", outputPath);
                return 1;
            }
        }
    }
    return 0;
}

inline int HandleSplitImageAction(int argc,char** argv)
{
    if(argc < 6)
    {
        std::print("Usage: {} split_image <image_path> <output_dir> <rect_width> <rect_height>\n", argv[0]);
        return 1;
    }
    
    SplitImageParam param;
    param.imagePath = argv[2];
    param.outputDir = argv[3];
    param.rectSize.x() = std::stoi(argv[4]);
    param.rectSize.y() = std::stoi(argv[5]);
    return SplitImage(param);
}