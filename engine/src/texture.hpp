#pragma once

#include "mtpch.hpp"

namespace core {

    class ImageData {
      public:

        ~ImageData() = default;

        ImageData(u32* Data, u32 Width, u32 Height)
            : Data{ Data }, m_Width{ Width }, m_Height{ Height },
              m_Resolution{ Width * Height } { }

        const u32& GetWidth() const { return m_Width; }

        const u32& GetHeight() const { return m_Height; }

        const u64&   GetResolution() const { return m_Resolution; }

        void            SaveImage(const std::string& Filename) const;

        u32*       Data;

      private:

        const u32 m_Width, m_Height;
        const u64   m_Resolution;
    };

    using image_t = std::shared_ptr<ImageData>;

    image_t              LoadImage(const std::string& Path);

    std::vector<image_t> LoadGif(const std::string& Path);

    class Texture {
      public:

        virtual ~Texture() = default;

        Texture(
            const std::string& Name, u32& ID, b8 IsTransparent, b8 IsDoublesided
        )
            : m_Name{ Name }, m_ID{ ID }, m_IsTransparent{ IsTransparent },
              m_IsDoublesided{ IsDoublesided } { }

        const std::string& GetName() const { return m_Name; }

        const u32&    GetID() const { return m_ID; }

        const b8&        IsTransparent() const { return m_IsTransparent; }

        const b8&        IsDoublesided() const { return m_IsDoublesided; }

        virtual u32   Sample(const glm::vec2& UV, f32 Scalar) const = 0;
        virtual void       Save(const std::string& Filename) const           = 0;

      private:

        std::string m_Name;
        u32    m_ID;
        b8        m_IsTransparent, m_IsDoublesided;
    };

    using texture_t = std::shared_ptr<Texture>;

    class ImageTexture : public Texture {
      public:

        ImageTexture(
            image_t Image, const std::string& Name, u32 ID, b8 IsTransparent,
            b8 IsDoublesided
        )
            : Texture(Name, ID, IsTransparent, IsDoublesided), m_Image{ Image } { }

        static texture_t
        New(const std::string& Path, b8 IsTransparent = false, b8 IsDoublesided = false);

        u32 Sample(const glm::vec2& UV, f32 Depth) const override;
        void     Save(const std::string& Filename) const override;

      protected:

        image_t m_Image;
    };

    class MipmapTexture : public Texture {
      public:

        MipmapTexture(
            const std::vector<image_t>& Mipmap, u8 Miplevels, const std::string& Name,
            u32 ID, b8 IsTransparent, b8 IsDoublesided
        )
            : Texture(Name, ID, IsTransparent, IsDoublesided), m_Mipmap{ Mipmap },
              m_Miplevels{ Miplevels } { }

        static texture_t
                 New(const std::string& Path, u8 Miplevels, b8 IsTransparent = false,
                     b8 IsDoublesided = false);

        u32 Sample(const glm::vec2& UV, f32 Depth) const override;
        void     Save(const std::string& Filename) const override;

      protected:

        std::vector<image_t> m_Mipmap;
        u8              m_Miplevels;
    };

    class AnimatedTexture
        : public MipmapTexture { // animated texture uses same structure as mipmapped texture
      public:

        AnimatedTexture(
            const std::vector<image_t>& ImageFrames, f32 PlaybackSpeed,
            const std::string& Name, u32 ID, b8 IsTransparent, b8 IsDoublesided
        )
            : MipmapTexture(
                  ImageFrames, static_cast<u8>(ImageFrames.size()), Name, ID,
                  IsTransparent, IsDoublesided
              ),
              m_PlaybackSpeed{ PlaybackSpeed } { }

        static texture_t
                 New(const std::string& Path, f32 PlaybackSpeed, b8 IsTransparent = false,
                     b8 IsDoublesided = false);

        u32 Sample(const glm::vec2& UV, f32 AnimationState) const override;

      private:

        f32 m_PlaybackSpeed;
    };

    class ColorTexture : public Texture {
      public:

        ~ColorTexture() = default;

        ColorTexture(
            const u32& Color, const std::string& Name, u32 ID, b8 IsTransparent,
            b8 IsDoublesided
        )
            : Texture(Name, ID, IsTransparent, IsDoublesided), m_Color{ Color } { }

        static texture_t
        New(const std::string& Name, const u32& Color, b8 IsTransparent = false,
            b8 IsDoublesided = false);

        u32 Sample(const glm::vec2& UV, f32 Depth) const override;

        void     Save(const std::string&) const override { return; }

      private:

        const u32 m_Color;
    };

    texture_t GetTexture(const std::string& Name);
    texture_t GetTexture(u32 ID);

    texture_t GetDefaultTexture();
    texture_t GetDefaultSkyboxTexture();

} // namespace core
