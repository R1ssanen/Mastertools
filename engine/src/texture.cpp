#include "texture.hpp"

#include "application.hpp"
#include "color.hpp"
#include "glm/gtx/fast_exponential.hpp"
#include "settings.hpp"
#include "srpch.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

namespace
{
std::unordered_map<std::string, core::image_t> s_LoadedImages;
std::unordered_map<std::string, core::texture_t> s_LoadedTextures;
std::unordered_map<size_t, uint32_t> s_MiplevelDebugColors{{0, 0x0bff00FF}, {1, 0x23e600FF}, {2, 0x45c200FF},
                                                           {3, 0x73c400FF}, {4, 0xc8d600FF}, {5, 0xe25e00FF}};
} // namespace

namespace core
{

image_t LoadImage(const std::string& t_Path)
{
    if (s_LoadedImages.count(t_Path))
    {
        return s_LoadedImages[t_Path];
    }

    int Format, Width, Height;
    uint8_t* Data8b{stbi_load(t_Path.c_str(), &Width, &Height, &Format, STBI_rgb_alpha)};

    if (!Data8b)
    {
        throw std::invalid_argument("Could not load texture file '" + t_Path + "'.");
    }

    uint32_t* Data32b{new uint32_t[Width * Height]};
    for (int ID = 0; ID < (Width * Height); ID++)
    {
        Data32b[ID] = ToUint32(Data8b[ID * 4], Data8b[ID * 4 + 1], Data8b[ID * 4 + 2], Data8b[ID * 4 + 3]);
    }

    stbi_image_free(Data8b);

    s_LoadedImages[t_Path] =
        std::make_shared<ImageData>(Data32b, static_cast<unsigned>(Width), static_cast<unsigned>(Height));

    return s_LoadedImages[t_Path];
}

void ImageData::SaveImage(const std::string& t_Filename) const
{
    uint8_t* Data8b = new uint8_t[GetWidth() * GetHeight() * 4];

    for (size_t ID = 0; ID < GetWidth() * GetHeight(); ID++)
    {
        auto [r, g, b, a] = UnpackToArray(Data[ID]);
        Data8b[ID * 4] = r;
        Data8b[ID * 4 + 1] = g;
        Data8b[ID * 4 + 2] = b;
        Data8b[ID * 4 + 3] = a;
    }

    if (!stbi_write_png(t_Filename.c_str(), GetWidth(), GetHeight(), STBI_rgb_alpha, Data8b,
                        GetWidth() * sizeof(uint32_t)))
    {
        throw std::runtime_error("Could not save texture.");
    }

    delete[] Data8b;
}

texture_t ImageTexture::New(const std::string& t_Path, bool t_IsTransparent, bool t_IsDoublesided)
{
    if (s_LoadedTextures.count(t_Path))
    {
        return s_LoadedTextures[t_Path];
    }

    s_LoadedTextures[t_Path] =
        std::make_shared<ImageTexture>(LoadImage(t_Path), t_Path, t_IsTransparent, t_IsDoublesided);
    return s_LoadedTextures[t_Path];
}

uint32_t ImageTexture::Sample(const glm::vec2& t_UV, float t_Depth) const
{
    unsigned Loc{static_cast<unsigned>(m_Image->GetHeight() * (t_UV.y - long(t_UV.y))) * m_Image->GetWidth() +
                 static_cast<unsigned>(m_Image->GetWidth() * (t_UV.x - long(t_UV.x)))};

    return m_Image->Data[glm::clamp(static_cast<size_t>(Loc), 0UL, m_Image->GetResolution() - 1)];
}

void ImageTexture::Save(const std::string& t_Filename) const { m_Image->SaveImage(t_Filename); }

texture_t MipmapTexture::New(const std::string& t_Path, uint8_t t_Miplevels, bool t_IsTransparent, bool t_IsDoublesided)
{
    if (s_LoadedTextures.count(t_Path))
    {
        return s_LoadedTextures[t_Path];
    }

    image_t Level0 = LoadImage(t_Path);
    SDL_Surface* ImageSurf = SDL_CreateRGBSurfaceWithFormatFrom(
        static_cast<void*>(Level0->Data), Level0->GetWidth(), Level0->GetHeight(), sizeof(uint32_t),
        Level0->GetWidth() * sizeof(uint32_t), SDL_PIXELFORMAT_RGBA8888);

    if (!ImageSurf)
    {
        throw std::runtime_error("Could not construct mipmap level 0. " + std::string(SDL_GetError()));
    }

    SDL_SetSurfaceBlendMode(ImageSurf, SDL_BLENDMODE_NONE);

    std::vector<image_t> Mipmap{Level0};

    for (size_t Level = 1; Level <= t_Miplevels; Level++)
    {
        int Width = static_cast<int>(Level0->GetWidth() / glm::fastPow(2UL, Level)),
            Height = static_cast<int>(Level0->GetHeight() / glm::fastPow(2UL, Level));

        SDL_Surface* SublevelSurf =
            SDL_CreateRGBSurfaceWithFormat(0, Width, Height, sizeof(uint32_t), SDL_PIXELFORMAT_RGBA8888);

        if (!SublevelSurf || SDL_BlitScaled(ImageSurf, nullptr, SublevelSurf, nullptr) != 0)
        {
            throw std::runtime_error("Could not construct mipmap level " + std::to_string(Level) + ". " +
                                     std::string(SDL_GetError()));
        }

        Mipmap.push_back(std::make_shared<ImageData>(static_cast<uint32_t*>(SublevelSurf->pixels), Width, Height));
    }

    s_LoadedTextures[t_Path] =
        std::make_shared<MipmapTexture>(Mipmap, t_Miplevels, t_Path, t_IsTransparent, t_IsDoublesided);

    // s_LoadedTextures[t_Path]->Save("mipmap");
    return s_LoadedTextures[t_Path];
}

uint32_t MipmapTexture::Sample(const glm::vec2& t_UV, float t_Depth) const
{
    size_t Miplevel = static_cast<size_t>(t_Depth * m_Miplevels);

    if (GetShowMipmaps())
    {
        return s_MiplevelDebugColors[Miplevel];
    }

    image_t Texture = m_Mipmap[Miplevel];

    unsigned Loc{static_cast<unsigned>(Texture->GetHeight() * (t_UV.y - long(t_UV.y))) * Texture->GetWidth() +
                 static_cast<unsigned>(Texture->GetWidth() * (t_UV.x - long(t_UV.x)))};

    return Texture->Data[glm::clamp(static_cast<size_t>(Loc), 0UL, Texture->GetResolution() - 1)];
}

void MipmapTexture::Save(const std::string& t_Filename) const
{
    for (size_t ID = 0; ID < m_Mipmap.size(); ID++)
    {
        m_Mipmap[ID]->SaveImage(t_Filename + std::to_string(ID));
    }
}

texture_t ColorTexture::New(const std::string& t_Name, const uint32_t& t_Color, bool t_IsTransparent,
                            bool t_IsDoublesided)
{
    if (s_LoadedTextures.count(t_Name))
    {
        return s_LoadedTextures[t_Name];
    }

    s_LoadedTextures[t_Name] = std::make_shared<ColorTexture>(t_Color, t_Name, t_IsTransparent, t_IsDoublesided);
    return s_LoadedTextures[t_Name];
}

uint32_t ColorTexture::Sample(const glm::vec2& t_UV, float t_Depth) const { return m_Color; }

void ColorTexture::Save(const std::string& t_Filename) const { return; }

texture_t GetTexture(const std::string& t_Name) { return s_LoadedTextures[t_Name]; }

texture_t GetDefaultTexture() { return MipmapTexture::New(BUILTINS_DIR + "untextured.png", 4, true, true); }

} // namespace core
