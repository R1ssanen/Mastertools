#include "texture.hpp"

#include "color.hpp"
#include "srpch.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

namespace core {

Texture::Texture(uint32_t* t_Data, unsigned int t_Width, unsigned int t_Height)
    : Data{t_Data}, m_Width{t_Width}, m_Height{t_Height} {}

const unsigned int& Texture::GetWidth() const {
  return m_Width;
}

const unsigned int& Texture::GetHeight() const {
  return m_Height;
}

const bool TextureExists(const std::string& t_Name) {
  return s_LoadedTextures.count(t_Name);
}

void Texture::Save(const std::string t_Filename) const {
  uint8_t* Data8b = new uint8_t[m_Width * m_Height * 4];
  std::fill_n(Data8b, m_Width * m_Height * 4, ToUint32(1.f, 0.f, 0.f));

  for (size_t i = 0; i < m_Width * m_Height; i++) {
    auto [r, g, b, a] = UnpackUint32(Data[i]);
    Data8b[i * 4] = r;
    Data8b[i * 4 + 1] = g;
    Data8b[i * 4 + 2] = b;
    Data8b[i * 4 + 3] = a;
  }

  if (!stbi_write_png(t_Filename.c_str(), m_Width, m_Height, STBI_rgb_alpha,
                      (void*)Data, m_Width * sizeof(uint32_t))) {
    throw std::runtime_error("Could not save texture.");
  }

  delete[] Data8b;
}

void LoadTexture(const std::string& t_Path) {
  if (TextureExists(t_Path)) {
    return;
  }

  int Format, Width, Height;
  uint8_t* Data8b{
      stbi_load(t_Path.c_str(), &Width, &Height, &Format, STBI_rgb_alpha)};

  if (!Data8b) {
    throw std::invalid_argument("Could not load texture file '" + t_Path +
                                "'.");
  }

  uint32_t* Data32b{new uint32_t[Width * Height]};
  for (int ID = 0; ID < Width * Height; ID++) {
    Data32b[ID] =
        glm::packUint4x8(glm::u8vec4(Data8b[ID * 4 + 3], Data8b[ID * 4 + 2],
                                     Data8b[ID * 4 + 1], Data8b[ID * 4]));
  }

  s_LoadedTextures[t_Path] =
      std::make_shared<core::Texture>(std::move(Data32b), Width, Height);

  stbi_image_free(Data8b);
}

texture_t GetTexture(const std::string& t_Name) {
  if (!TextureExists(t_Name)) {
    LoadTexture(t_Name);
  }

  return s_LoadedTextures.at(t_Name);
}

}  // namespace core
