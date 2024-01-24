#pragma once

#include "srpch.hpp"

#define DEFAULT_TEXTURE "../../engine/builtins/untextured.png"

namespace core {

class Texture {
 public:
  Texture() = default;
  ~Texture() = default;

  Texture(uint32_t* t_Data, unsigned int t_Width, unsigned int t_Height);

  const uint32_t* Data;

  const unsigned int& GetWidth() const;
  const unsigned int& GetHeight() const;
  void Save(const std::string t_Filename) const;

 private:
  unsigned int m_Width{0}, m_Height{0};
};

using texture_t = std::shared_ptr<Texture>;

const bool TextureExists(const std::string& t_Name);

void LoadTexture(const std::string& t_Path);

texture_t GetTexture(const std::string& t_Name);

}  // namespace core

namespace {
std::unordered_map<std::string, std::shared_ptr<core::Texture>>
    s_LoadedTextures;
}  // namespace
