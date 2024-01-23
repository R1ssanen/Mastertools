#include "texture.hpp"

#include "srpch.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

namespace core {

Texture::Texture(uint32_t* t_Data, size_t t_Width, size_t t_Height)
    : Data{t_Data}, m_Width{t_Width}, m_Height{t_Height} {}

const size_t& Texture::GetWidth() const {
  return m_Width;
}

const size_t& Texture::GetHeight() const {
  return m_Height;
}

void LoadTexture(const std::string& t_Path) {
  if (s_LoadedTextures.count(t_Path)) {
    return;
  }

  int Width, Height, Format;
  uint8_t* Data8b{
      stbi_load(t_Path.c_str(), &Width, &Height, &Format, STBI_rgb_alpha)};

  if (!Data8b) {
    throw std::invalid_argument("Can't load texture file '" + t_Path + "'.");
  }

  uint32_t* Data{new uint32_t[Width * Height]};
  for (size_t i = 0; i < static_cast<size_t>(Width * Height); i += 4) {
    Data[i] = glm::packUint4x8(
        glm::u8vec4(Data8b[i], Data8b[i + 1], Data8b[i + 2], Data8b[i + 3]));
  }

  stbi_image_free(Data8b);
  s_LoadedTextures[t_Path] = std::make_shared<Texture>(Data, Width, Height);
  SaveTexture(t_Path, "at_loading.png");
  delete[] Data;
}

void SaveTexture(const std::string& t_TextureName,
                 const std::string& t_Filename) {
  if (!s_LoadedTextures.count(t_TextureName)) {
    throw std::invalid_argument("Cannot save texture '" + t_TextureName +
                                "', it is nonexistant.");
  }

  const auto Texture{s_LoadedTextures.at(t_TextureName)};
  stbi_write_png(t_Filename.c_str(), Texture->GetWidth(), Texture->GetHeight(),
                 STBI_rgb_alpha, Texture->Data,
                 Texture->GetWidth() * sizeof(uint32_t));
}

texture_t GetTexture(const std::string& t_Name) {
  if (!s_LoadedTextures.count(t_Name)) {
    LoadTexture(t_Name);
  }

  return s_LoadedTextures.at(t_Name);
}

}  // namespace core
