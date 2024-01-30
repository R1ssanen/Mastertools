#pragma once

#include "srpch.hpp"

namespace core {

class Texture {
 public:
  Texture() = default;
  ~Texture() { delete[] Data; }

  Texture(uint32_t* t_Data,
          const std::string& t_Name,
          unsigned int t_Width,
          unsigned int t_Height,
          bool t_Transparent,
          bool t_CullBackfaces);

  const uint32_t* const Data;

  const unsigned int& GetWidth() const { return m_Width; }
  const unsigned int& GetHeight() const { return m_Height; }
  const bool& CullBackfaces() const { return m_CullBackfaces; }
  const std::string& GetName() const { return m_Name; }
  const bool& IsTransparent() const { return m_Transparent; }

  unsigned int GetLocation(const glm::vec2& t_UV);
  const uint32_t& Sample(const glm::vec2& t_UV);
  void Save(const std::string t_Filename) const;

 private:
  std::string m_Name;
  unsigned int m_Width{0}, m_Height{0};
  unsigned int m_Size{0};

  bool m_Transparent{false}, m_CullBackfaces{true};
};

using texture_t = std::shared_ptr<Texture>;

const texture_t& NewTexture(const std::string& t_Path,
                            bool t_Transparent,
                            bool t_CullBackfaces);

const texture_t& GetTexture(const std::string& t_Name);

const texture_t& GetDefaultTexture();

}  // namespace core

namespace {
std::unordered_map<std::string, std::shared_ptr<core::Texture>>
    s_LoadedTextures;
}  // namespace
