#pragma once

#include "srpch.hpp"

namespace core
{

class ImageData
{
  public:
    ~ImageData() = default;
    ImageData(uint32_t* t_Data, unsigned t_Width, unsigned t_Height)
        : Data{t_Data}, m_Width{t_Width}, m_Height{t_Height}, m_Resolution{t_Width * t_Height}
    {
    }

    const unsigned& GetWidth() const { return m_Width; }
    const unsigned& GetHeight() const { return m_Height; }
    const size_t& GetResolution() const { return m_Resolution; }

    void SaveImage(const std::string& t_Filename) const;

    uint32_t* Data;

  private:
    const unsigned m_Width, m_Height;
    const size_t m_Resolution;
};

using image_t = std::shared_ptr<ImageData>;

image_t LoadImage(const std::string& t_Path);

class Texture
{
  public:
    ~Texture() = default;
    Texture(const std::string& t_Name, bool t_IsTransparent, bool t_IsDoublesided)
        : m_Name{t_Name}, m_IsTransparent{t_IsTransparent}, m_IsDoublesided{t_IsDoublesided}
    {
    }

    const std::string& GetName() const { return m_Name; }
    const bool& IsTransparent() const { return m_IsTransparent; }
    const bool& IsDoublesided() const { return m_IsDoublesided; }

    virtual uint32_t Sample(const glm::vec2& t_UV, float t_Depth) const = 0;
    virtual void Save(const std::string& t_Filename) const = 0;

  private:
    std::string m_Name;
    bool m_IsTransparent = false, m_IsDoublesided = false;
};

using texture_t = std::shared_ptr<Texture>;

class ImageTexture : public Texture
{
  public:
    ~ImageTexture() = default;

    ImageTexture(image_t t_Image, const std::string& t_Name, bool t_IsTransparent, bool t_IsDoublesided)
        : Texture(t_Name, t_IsTransparent, t_IsDoublesided), m_Image{t_Image}
    {
    }

    static texture_t New(const std::string& t_Path, bool t_IsTransparent = false, bool t_IsDoublesided = false);

    uint32_t Sample(const glm::vec2& t_UV, float t_Depth) const override;
    void Save(const std::string& t_Filename) const override;

  private:
    image_t m_Image;
};

class MipmapTexture : public Texture
{
  public:
    ~MipmapTexture() = default;

    MipmapTexture(const std::vector<image_t>& t_Mipmap, uint8_t t_Miplevels, const std::string& t_Name,
                  bool t_IsTransparent, bool t_IsDoublesided)
        : Texture(t_Name, t_IsTransparent, t_IsDoublesided), m_Mipmap{t_Mipmap}, m_Miplevels{t_Miplevels}
    {
    }

    static texture_t New(const std::string& t_Path, uint8_t t_Miplevels, bool t_IsTransparent = false,
                         bool t_IsDoublesided = false);

    uint32_t Sample(const glm::vec2& t_UV, float t_Depth) const override;
    void Save(const std::string& t_Filename) const override;

  private:
    std::vector<image_t> m_Mipmap;
    uint8_t m_Miplevels;
};

class ColorTexture : public Texture
{
  public:
    ~ColorTexture() = default;

    ColorTexture(const uint32_t& t_Color, const std::string& t_Name, bool t_IsTransparent, bool t_IsDoublesided)
        : Texture(t_Name, t_IsTransparent, t_IsDoublesided), m_Color{t_Color}
    {
    }

    static texture_t New(const std::string& t_Name, const uint32_t& t_Color, bool t_IsTransparent = false,
                         bool t_IsDoublesided = false);

    uint32_t Sample(const glm::vec2& t_UV, float t_Depth) const override;
    void Save(const std::string& t_Filename) const override;

  private:
    const uint32_t m_Color;
};

texture_t GetTexture(const std::string& t_Name);

texture_t GetDefaultTexture();

} // namespace core
