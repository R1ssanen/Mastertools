#pragma once

#include "srpch.hpp"

namespace core {

class Texture {
 public:
  Texture() = default;
  ~Texture() = default;

  Texture(uint32_t* t_Data, size_t t_Width, size_t t_Height);

  const uint32_t* Data;

  const size_t& GetWidth() const;
  const size_t& GetHeight() const;

 private:
  size_t m_Width{0}, m_Height{0};
};

using texture_t = std::shared_ptr<Texture>;

void LoadTexture(const std::string& t_Path);

void SaveTexture(const std::string& t_TextureName,
                 const std::string& t_FileName);

texture_t GetTexture(const std::string& t_Name);

}  // namespace core

namespace {
std::unordered_map<std::string, std::shared_ptr<core::Texture>>
    s_LoadedTextures;
}  // namespace
