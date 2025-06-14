#ifndef MT_TEXTURE_HPP_
#define MT_TEXTURE_HPP_

#include <array>
#include <glm/vec2.hpp>
#include <iostream>
#include <string>

#include "buffers/buffer.hpp"
#include "buffers/framebuffer.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "external/stb_image.h"

namespace mt {

    class Texture : public Buffer<u32> {
      public:

        static Texture Load(const std::string& path) {
            int width, height, channels;
            u8* data = stbi_load(path.c_str(), &width, &height, &channels, 0);

            if (!data) {
                std::cerr << "Could not load texture '" << path << "'.\n";
                return Texture();
            }

            if (channels == 3) {
                u8* tmp = new u8[width * height * sizeof(u32)];

                // endianness reversed, for some reason :/
                for (u64 i = 0, j = 0; i < width * height * 3; i += 3, j += 4) {
                    tmp[j]     = 0xff;
                    tmp[j + 1] = data[i + 2];
                    tmp[j + 2] = data[i + 1];
                    tmp[j + 3] = data[i + 0];
                }

                std::swap(data, tmp);
                free(tmp);

            } else {
                for (u64 i = 0; i < width * height * sizeof(u32); i += 4) {
                    std::swap(data[i], data[i + 3]);
                    std::swap(data[i + 1], data[i + 2]);
                }
            }

            std::clog << "Channels loaded for texture '" << path << "': " << channels << '\n';
            return Texture(u64(width), u64(height), reinterpret_cast<u32*>(data));
        }

        u32 operator[](const glm::vec2& uv) const {
            u32 x = std::clamp(uv.x, 0.f, 1.f) * m_width - 0.5f;
            u32 y = std::clamp(uv.y, 0.f, 1.f) * m_height - 0.5f;
            return m_mem[y * m_width + x];
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

    enum CubemapFace {
        POSITIVE_X,
        NEGATIVE_X,
        POSITIVE_Y,
        NEGATIVE_Y,
        POSITIVE_Z,
        NEGATIVE_Z,
    };

    using cubemap_texture_t = std::array<Texture, 6>;

    inline std::pair<CubemapFace, glm::vec2> sample_cubemap(const glm::vec3& d) {
        assert(glm::length(d) != 0.f);
        glm::vec3 abs_d = glm::abs(d);

        if (abs_d.x > abs_d.y && abs_d.x > abs_d.z) {
            f32 inv_x_half = 0.5f / (d.x + FLT_EPSILON);
            f32 u          = 0.5f + d.z * inv_x_half;
            f32 v          = 0.5f + d.y * inv_x_half;

            return d.x > 0.f ? std::make_pair(CubemapFace::POSITIVE_X, glm::vec2(u, v))
                             : std::make_pair(CubemapFace::NEGATIVE_X, glm::vec2(u, 1.f - v));
        }

        else if (abs_d.y > abs_d.x && abs_d.y > abs_d.z) {
            f32 inv_y_half = 0.5f / (d.y + FLT_EPSILON);
            f32 u          = 0.5f + d.x * inv_y_half;
            f32 v          = 0.5f - d.z * inv_y_half;

            // inverted, y-up
            return d.y < 0.f ? std::make_pair(CubemapFace::POSITIVE_Y, glm::vec2(u, v))
                             : std::make_pair(CubemapFace::NEGATIVE_Y, glm::vec2(1.f - u, v));
        }

        else {
            f32 inv_z_half = 0.5f / (d.z + FLT_EPSILON);
            f32 u          = 0.5f - d.x * inv_z_half;
            f32 v          = 0.5f + d.y * inv_z_half;

            return d.z > 0.f ? std::make_pair(CubemapFace::POSITIVE_Z, glm::vec2(u, v))
                             : std::make_pair(CubemapFace::NEGATIVE_Z, glm::vec2(u, 1.f - v));
        }
    }

    inline glm::vec3 clip_to_world(const glm::vec4& p, const glm::mat4& inv_view_proj) {
        glm::vec4 world = inv_view_proj * p;
        return glm::vec3(world) / world.w;
    }

    inline glm::vec3 screen_to_world(
        f32 screen_x, f32 screen_y, f32 ndc_z, f32 inv_w_2, f32 inv_h_2,
        const glm::mat4& inv_view_proj
    ) {

        f32 ndc_x = screen_x * inv_w_2 - 1.f;
        f32 ndc_y = 1.f - screen_y * inv_h_2;

        return clip_to_world(glm::vec4(ndc_x, ndc_y, ndc_z, 1.f), inv_view_proj);
    }

    inline glm::vec3
    screen_to_world(const glm::vec3& p, f32 inv_w_2, f32 inv_h_2, const glm::mat4& inv_view_proj) {
        return screen_to_world(p.x, p.y, p.z, inv_w_2, inv_h_2, inv_view_proj);
    }

    void Framebuffer::render_cubemap_fullscreen(
        const glm::mat4& proj, const glm::mat4& view, const cubemap_texture_t& cubemap
    ) {

        f32       inv_w_2             = 2.f / m_width;
        f32       inv_h_2             = 2.f / m_height;

        glm::mat4 view_no_translation = view;
        view_no_translation[3]        = glm::vec4(0.f, 0.f, 0.f, 1.f);
        glm::mat4 inv_view_proj       = glm::inverse(proj * view_no_translation);

        glm::vec3 d0       = screen_to_world(0.f, 0.f, 1.f, inv_w_2, inv_h_2, inv_view_proj);
        glm::vec3 slope_d0 = screen_to_world(1.f, 0.f, 1.f, inv_w_2, inv_h_2, inv_view_proj) - d0;
        glm::vec3 slope_d1 = screen_to_world(0.f, 1.f, 1.f, inv_w_2, inv_h_2, inv_view_proj) - d0;

        for (u32 y = 0, row = 0; y < m_height; ++y) {
            glm::vec3 d = d0;

            for (u32 x = 0; x < m_width; ++x) {

                u32& under = m_color[row + x];

                if (under == 0xdeadbeef) {
                    auto [face, uv] = sample_cubemap(d);
                    under           = cubemap[face][uv];
                }

                d += slope_d0;
            }

            row += m_width;
            d0 += slope_d1;
        }
    }

} // namespace mt

#endif
