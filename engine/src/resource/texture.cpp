#include "texture.hpp"

#include "../core/application.hpp"
#include "../core/color.hpp"
#include "../core/settings.hpp"
#include "../mtpch.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

namespace {
    std::unordered_map<std::string, mt::image_t>              s_LoadedImages;
    std::unordered_map<std::string, std::vector<mt::image_t>> s_LoadedAnimations;

    mt::u32                                                   s_CurrentMaxID = 0;
    std::unordered_map<std::string, mt::u32>                  s_TexturePathToID;
    std::unordered_map<mt::u32, mt::texture_t>                s_TextureIDToTexture;

    std::unordered_map<mt::u64, mt::u32>                      s_MiplevelDebugColors{
                             {0,  0x0bff00FF},
                             { 1, 0x23e600FF},
                             { 2, 0x45c200FF},
                             { 3, 0x73c400FF},
                             { 4, 0xc8d600FF},
                             { 5, 0xe25e00FF}
    };
} // namespace

namespace mt {

    image_t LoadImage(const std::string& Path) {
        if (s_LoadedImages.count(Path)) { return s_LoadedImages[Path]; }

        i32 Format, Width, Height;
        u8* Data8b{ stbi_load(Path.c_str(), &Width, &Height, &Format, STBI_rgb_alpha) };

        if (!Data8b) { throw std::invalid_argument("Could not load texture file '" + Path + "'."); }

        u32* Data32b = new u32[Width * Height];
        for (u64 ID = 0; ID < static_cast<u64>(Width * Height); ++ID) {
            Data32b[ID] = ToUint32(
                Data8b[ID * 4], Data8b[ID * 4 + 1], Data8b[ID * 4 + 2], Data8b[ID * 4 + 3]
            );
        }

        stbi_image_free(Data8b);

        s_LoadedImages[Path] =
            std::make_shared<ImageData>(Data32b, static_cast<u32>(Width), static_cast<u32>(Height));

        return s_LoadedImages[Path];
    }

    std::vector<image_t> LoadGif(const std::string& Path) {
        if (s_LoadedAnimations.count(Path)) { return s_LoadedAnimations[Path]; }

        std::vector<image_t> ImageFrames;

        i32                  DataLength;
        u8*                  FileData;
        {
            std::ifstream     File(Path);
            std::stringstream Stream;
            Stream << File.rdbuf();
            File.close();

            const std::string FileString = Stream.str();
            const char*       CStrData   = FileString.c_str();
            FileData                     = new u8[FileString.length()];
            std::copy(CStrData, CStrData + FileString.length(), FileData);
            DataLength = FileString.length();
        }

        if (!FileData) {
            throw std::runtime_error("Could not access contents of gif file '" + Path + "'.");
        }

        i32* Delays = nullptr;
        i32  Width, Height, Frames, Format;
        u8*  Data = stbi_load_gif_from_memory(
            FileData, DataLength, &Delays, &Width, &Height, &Frames, &Format, STBI_rgb_alpha
        );

        delete[] FileData;

        if (!Data) { throw std::runtime_error("Could not load gif file '" + Path + "'."); }

        const u64 ByteResolution = Width * Height * sizeof(u32);

        for (u64 Frame = 0; Frame < static_cast<u64>(Frames); ++Frame) {

            u32* Data32b = new u32[Width * Height];
            for (u64 ID = 0; ID < static_cast<u64>(Width * Height); ++ID) {
                u64 Loc     = (ByteResolution * Frame) + ID * 4;
                Data32b[ID] = ToUint32(Data[Loc], Data[Loc + 1], Data[Loc + 2], Data[Loc + 3]);
            }

            ImageFrames.push_back(std::make_shared<ImageData>(
                Data32b, static_cast<u32>(Width), static_cast<u32>(Height)
            ));
        }

        stbi_image_free(Data);

        std::reverse(ImageFrames.begin(), ImageFrames.end());
        s_LoadedAnimations[Path] = ImageFrames;
        return ImageFrames;
    }

    void ImageData::SaveImage(const std::string& Filename) const {
        u8* Data8b = new u8[m_Width * m_Height * 4];

        for (u64 ID = 0; ID < static_cast<u64>(m_Width * m_Height); ++ID) {
            auto [r, g, b, a]  = UnpackToArray(Data[ID]);
            Data8b[ID * 4]     = r;
            Data8b[ID * 4 + 1] = g;
            Data8b[ID * 4 + 2] = b;
            Data8b[ID * 4 + 3] = a;
        }

        if (!stbi_write_png(
                Filename.c_str(), m_Width, m_Height, STBI_rgb_alpha, Data8b, m_Width * sizeof(u32)
            )) {
            throw std::runtime_error("Could not save texture.");
        }

        delete[] Data8b;
    }

    texture_t ImageTexture::New(const std::string& Path, b8 IsTransparent, b8 IsDoublesided) {
        if (s_TexturePathToID.count(Path)) { return s_TextureIDToTexture[s_TexturePathToID[Path]]; }

        texture_t Texture = std::make_shared<ImageTexture>(
            LoadImage(Path), Path, s_CurrentMaxID, IsTransparent, IsDoublesided
        );
        s_TextureIDToTexture[s_CurrentMaxID] = Texture;
        s_TexturePathToID[Path]              = s_CurrentMaxID;

        s_CurrentMaxID++;
        return Texture;
    }

    u32 ImageTexture::Sample(const glm::vec2& UV, f32 Depth) const {
        u32 Loc =
            static_cast<u32>(m_Image->GetHeight() * (UV.y - i64(UV.y))) * m_Image->GetWidth() +
            static_cast<u32>(m_Image->GetWidth() * (UV.x - i64(UV.x)));

        return m_Image->Data[glm::clamp(Loc, 0U, static_cast<u32>(m_Image->GetResolution() - 1))];
    }

    void ImageTexture::Save(const std::string& Filename) const { m_Image->SaveImage(Filename); }

    texture_t
    MipmapTexture::New(const std::string& Path, u8 Miplevels, b8 IsTransparent, b8 IsDoublesided) {
        if (s_TexturePathToID.count(Path)) { return s_TextureIDToTexture[s_TexturePathToID[Path]]; }

        image_t      Level0    = LoadImage(Path);
        SDL_Surface* ImageSurf = SDL_CreateRGBSurfaceWithFormatFrom(
            static_cast<void*>(Level0->Data), Level0->GetWidth(), Level0->GetHeight(), sizeof(u32),
            Level0->GetWidth() * sizeof(u32), SDL_PIXELFORMAT_RGBA8888
        );

        if (!ImageSurf) {
            throw std::runtime_error(
                "Could not construct mipmap level 0. " + std::string(SDL_GetError())
            );
        }

        SDL_SetSurfaceBlendMode(ImageSurf, SDL_BLENDMODE_NONE);

        std::vector<image_t> Mipmap = { Level0 };

        for (u32 Level = 1; Level <= Miplevels; ++Level) {
            i32 Width  = static_cast<i32>(Level0->GetWidth() / glm::fastPow(2U, Level)),
                Height = static_cast<i32>(Level0->GetHeight() / glm::fastPow(2U, Level));

            SDL_Surface* SublevelSurf = SDL_CreateRGBSurfaceWithFormat(
                0, Width, Height, sizeof(u32), SDL_PIXELFORMAT_RGBA8888
            );

            if (!SublevelSurf || SDL_BlitScaled(ImageSurf, nullptr, SublevelSurf, nullptr) != 0) {
                throw std::runtime_error(
                    "Could not construct mipmap level " + std::to_string(Level) + ". " +
                    std::string(SDL_GetError())
                );
            }

            Mipmap.push_back(
                std::make_shared<ImageData>(static_cast<u32*>(SublevelSurf->pixels), Width, Height)
            );
        }

        texture_t Texture = std::make_shared<MipmapTexture>(
            Mipmap, Miplevels, Path, s_CurrentMaxID, IsTransparent, IsDoublesided
        );
        s_TextureIDToTexture[s_CurrentMaxID] = Texture;
        s_TexturePathToID[Path]              = s_CurrentMaxID;

        s_CurrentMaxID++;
        return Texture;
    }

    u32 MipmapTexture::Sample(const glm::vec2& UV, f32 Depth) const {
        u64 Miplevel = static_cast<u64>(Depth * m_Miplevels);

#ifndef NDEBUG
        if (GetShowMipmaps()) { return s_MiplevelDebugColors[Miplevel]; }
#endif

        image_t Texture = m_Mipmap[Miplevel];

        u32     Loc =
            static_cast<u32>(Texture->GetHeight() * (UV.y - i64(UV.y))) * Texture->GetWidth() +
            static_cast<u32>(Texture->GetWidth() * (UV.x - i64(UV.x)));

        // u32 Loc         = (static_cast<u32>(UV.x) % Texture->GetHeight()) * Texture->GetWidth() +
        //           (static_cast<u32>(UV.y) % Texture->GetWidth());
        return Texture->Data[glm::clamp(Loc, 0U, static_cast<u32>(Texture->GetResolution() - 1))];
    }

    void MipmapTexture::Save(const std::string& Filename) const {
        for (u64 ID = 0; ID < m_Mipmap.size(); ++ID) {
            m_Mipmap[ID]->SaveImage(Filename + std::to_string(ID));
        }
    }

    texture_t AnimatedTexture::New(
        const std::string& Path, f32 PlaybackSpeed, b8 IsTransparent, b8 IsDoublesided
    ) {
        if (s_TexturePathToID.count(Path)) { return s_TextureIDToTexture[s_TexturePathToID[Path]]; }

        texture_t Texture = std::make_shared<AnimatedTexture>(
            LoadGif(Path), PlaybackSpeed, Path, s_CurrentMaxID, IsTransparent, IsDoublesided
        );
        s_TextureIDToTexture[s_CurrentMaxID] = Texture;
        s_TexturePathToID[Path]              = s_CurrentMaxID;

        s_CurrentMaxID++;
        return Texture;
    }

    u32 AnimatedTexture::Sample(const glm::vec2& UV, f32 AnimationState) const {
        u64 Frame =
            glm::clamp(static_cast<u64>(AnimationState * m_Miplevels), (u64)0, (u64)m_Miplevels);

#ifndef NDEBUG
        if (GetShowMipmaps()) {
            return ToUint32(
                static_cast<u8>(255 * AnimationState),
                static_cast<u8>(255 * (1.f - AnimationState)),
                static_cast<u8>(255 * (std::sin(AnimationState) + 1.f * 0.5f)), 255
            );
        }
#endif

        image_t Texture = m_Mipmap[Frame];

        u32 Loc{ static_cast<u32>(Texture->GetHeight() * (UV.y - i64(UV.y))) * Texture->GetWidth() +
                 static_cast<u32>(Texture->GetWidth() * (UV.x - i64(UV.x))) };

        return Texture->Data[glm::clamp(Loc, 0U, static_cast<u32>(Texture->GetResolution() - 1))];
    }

    texture_t ColorTexture::New(
        const std::string& Name, const u32& Color, b8 IsTransparent, b8 IsDoublesided
    ) {
        if (s_TexturePathToID.count(Name)) { return s_TextureIDToTexture[s_TexturePathToID[Name]]; }

        texture_t Texture = std::make_shared<ColorTexture>(
            Color, Name, s_CurrentMaxID, IsTransparent, IsDoublesided
        );
        s_TextureIDToTexture[s_CurrentMaxID] = Texture;
        s_TexturePathToID[Name]              = s_CurrentMaxID;

        s_CurrentMaxID++;
        return Texture;
    }

    u32       ColorTexture::Sample(const glm::vec2& UV, f32 Depth) const { return m_Color; }

    texture_t GetTexture(const std::string& Name) {
        return s_TextureIDToTexture[s_TexturePathToID[Name]];
    }

    texture_t GetTexture(u32 ID) { return s_TextureIDToTexture[ID]; }

    texture_t GetDefaultTexture() {
        return ImageTexture::New(BUILTINS_DIR + "untextured.png", true, true);
    }

    texture_t GetDefaultSkyboxTexture() {
        return ImageTexture::New(BUILTINS_DIR + "skybox.png", false, false);
    }

} // namespace mt
