#ifndef MT_TEXTURE_HPP_
#define MT_TEXTURE_HPP_

#include <glm/ext/vector_float2.hpp>
#include <iostream>
#include <string>

#include "buffers/buffer.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "external/stb_image.h"

namespace mt {

    class Texture : public Buffer<u32> {
      public:

        static Texture Load(const std::string& path) {
            int width, height, channels;
            u8* data = stbi_load(path.c_str(), &width, &height, &channels, STBI_rgb_alpha);

            if (!data) {
                std::cerr << "Could not load texture.\n";
                return Texture();
            }

            std::clog << "Channels loaded for texture '" << path << "': " << channels << '\n';
            return Texture(u64(width), u64(height), reinterpret_cast<u32*>(data));
        }

        u32& operator[](const glm::vec2& uv) {
            return m_mem[u64(uv.y * m_height) * m_width + u64(uv.x * m_width)];
        }

      private:

        Texture(u64 width, u64 height, u32* data) : Buffer<u32>(width, height) {
            this->m_mem = std::vector(data, data + width * height);
        }

        Texture() : Buffer<u32>(2, 2), m_channels(STBI_rgb_alpha) {
            m_mem[0] = 0xd544ffff;
            m_mem[1] = 0xffffffff;
            m_mem[2] = 0xd544ffff;
            m_mem[3] = 0xffffffff;
        }

        int m_channels;
    };

} // namespace mt

#endif
