#include "texture.hpp"

#include "color.hpp"
#include "srpch.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image_write.h"

namespace {
core::texture_t s_DefaultTexture{
    core::NewTexture("../../engine/builtins/untextured.png", true, false)};
}

namespace core {

Texture::Texture(uint32_t* t_Data,
                 const std::string& t_Name,
                 unsigned int t_Width,
                 unsigned int t_Height,
                 bool t_Transparent,
                 bool t_CullBackfaces)
    : Data{t_Data},
      m_Name{t_Name},
      m_Width{t_Width},
      m_Height{t_Height},
      m_Transparent{t_Transparent},
      m_CullBackfaces{t_CullBackfaces} {}

unsigned int Texture::GetLocation(const glm::vec2& t_UV) {
  return static_cast<unsigned int>(m_Height * (t_UV.y - long(t_UV.y))) *
             m_Width +
         static_cast<unsigned int>(m_Width * (t_UV.x - long(t_UV.x)));
}

const uint32_t& Texture::Sample(const glm::vec2& t_UV) {
  return Data[glm::clamp(GetLocation(t_UV), 0U, m_Width * m_Height - 1)];
};

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

const texture_t& NewTexture(const std::string& t_Path,
                            bool t_Transparent,
                            bool t_CullBackfaces) {
  if (s_LoadedTextures.count(t_Path)) {
    return s_LoadedTextures[t_Path];
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
      std::make_shared<core::Texture>(std::move(Data32b), t_Path, Width, Height,
                                      t_Transparent, t_CullBackfaces);

  stbi_image_free(Data8b);

  return s_LoadedTextures[t_Path];
}

const texture_t& GetTexture(const std::string& t_Name) {
  return s_LoadedTextures[t_Name];
}

const texture_t& GetDefaultTexture() {
  return s_DefaultTexture;
}

}  // namespace core
