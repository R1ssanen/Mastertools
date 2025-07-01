#ifndef MT_TEXTURE_HPP_
#define MT_TEXTURE_HPP_

#include <array>
#include <glm/vec2.hpp>
#include <glm/vec4.hpp>
#include <iostream>
#include <string>

#include "aabb.hpp"
#include "buffers/buffer.hpp"
#include "buffers/framebuffer.hpp"

#define STB_IMAGE_IMPLEMENTATION
#include "external/stb_image.h"

namespace mt {

    inline glm::u8vec4 lerp(const glm::u16vec4& a, const glm::u16vec4& b, u8 t) noexcept {
        return a + (((b - a) * u16(t)) >> u16(8));
    }

    inline glm::u8vec4 average(const glm::u16vec4& a, const glm::u16vec4& b) noexcept {
        return (a + b) >> u16(1);
    }

    inline glm::u8vec4 unpack(u32 c) noexcept {
        u8* comps = reinterpret_cast<u8*>(&c);
        return glm::u8vec4(comps[0], comps[1], comps[2], comps[3]);
    }

    inline u32 pack(const glm::u8vec4& c) noexcept {
        return (c.a << 24) | (c.b << 16) | (c.g << 8) | c.r;
    }

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

                for (u64 i = 0, j = 0; i < u64(width * height * 3); i += 3, j += 4) {
                    tmp[j + 0] = data[i + 0];
                    tmp[j + 1] = data[i + 1];
                    tmp[j + 2] = data[i + 2];
                    tmp[j + 3] = 0xff;
                }

                std::swap(data, tmp);
                free(tmp);
            }

            std::clog << "Channels loaded for texture '" << path << "': " << channels << '\n';
            return Texture(u64(width), u64(height), reinterpret_cast<u32*>(data));
        }

        u32 operator[](const glm::vec2& uv) const {
            u32 x = std::clamp(uv.x, 0.f, 1.f) * (m_width - 1) + 0.5f;
            u32 y = std::clamp(uv.y, 0.f, 1.f) * (m_height - 1) + 0.5f;
            return m_mem[y * m_width + x];
        }

        u64 pitch() const noexcept { return m_pitch; }

      private:

        Texture(u64 width, u64 height, u32* data)
            : Buffer<u32>(width, height), m_pitch(width * sizeof(u32)) {
            this->m_mem = std::vector(data, data + width * height);
        }

        Texture() : Buffer<u32>(2, 2), m_channels(STBI_rgb_alpha) {
            m_mem[0] = 0xffff44d5;
            m_mem[1] = 0xffffffff;
            m_mem[2] = 0xffffffff;
            m_mem[3] = 0xffff44d5;
        }

        u64 m_pitch;
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

    // right-handed system, -z, +y
    inline std::pair<CubemapFace, glm::vec2> sample_cubemap(const glm::vec3& d) {
        assert(glm::length(d) != 0.f);
        glm::vec3 abs_d = glm::abs(d);

        if ((abs_d.x > abs_d.y) && (abs_d.x > abs_d.z)) {
            f32 inv_x_half = 0.5f / d.x;
            f32 u          = 0.5f + d.z * inv_x_half;
            f32 v          = 0.5f - d.y * inv_x_half;

            return d.x > 0.f ? std::make_pair(CubemapFace::POSITIVE_X, glm::vec2(u, v))
                             : std::make_pair(CubemapFace::NEGATIVE_X, glm::vec2(u, 1.f - v));
        }

        else if ((abs_d.y > abs_d.x) && (abs_d.y > abs_d.z)) {
            f32 inv_y_half = 0.5f / d.y;
            f32 u          = 0.5f + d.x * inv_y_half;
            f32 v          = 0.5f + d.z * inv_y_half;

            return d.y > 0.f ? std::make_pair(CubemapFace::POSITIVE_Y, glm::vec2(1.f - u, v))
                             : std::make_pair(CubemapFace::NEGATIVE_Y, glm::vec2(u, v));
        }

        else {
            f32 inv_z_half = 0.5f / d.z;
            f32 u          = 0.5f - d.x * inv_z_half;
            f32 v          = 0.5f - d.y * inv_z_half;

            return d.z > 0.f ? std::make_pair(CubemapFace::POSITIVE_Z, glm::vec2(u, v))
                             : std::make_pair(CubemapFace::NEGATIVE_Z, glm::vec2(u, 1.f - v));
        }
    }

    inline glm::vec3 clip_to_world(const glm::vec4& p, const glm::mat4& inv_view_proj) {
        glm::vec4 world = inv_view_proj * p;
        return glm::vec3(world) / world.w;
    }

    // right-handed system, -z, +y
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
        const glm::mat4& proj, const glm::mat4& view, const cubemap_texture_t& cubemap,
        const glm::mat4& rotation
    ) {

        f32       inv_w_2             = 2.f / m_width;
        f32       inv_h_2             = 2.f / m_height;

        glm::mat4 view_no_translation = view;
        view_no_translation[3]        = glm::vec4(0.f, 0.f, 0.f, 1.f);
        glm::mat4 inv_view_proj       = glm::inverse(proj * view_no_translation);

        glm::vec3 d0       = screen_to_world(0.f, 0.f, 1.f, inv_w_2, inv_h_2, inv_view_proj);
        glm::vec3 slope_d0 = screen_to_world(1.f, 0.f, 1.f, inv_w_2, inv_h_2, inv_view_proj) - d0;
        glm::vec3 slope_d1 = screen_to_world(0.f, 1.f, 1.f, inv_w_2, inv_h_2, inv_view_proj) - d0;

        d0                 = rotation * glm::vec4(d0, 0.f);
        slope_d0           = rotation * glm::vec4(slope_d0, 0.f);
        slope_d1           = rotation * glm::vec4(slope_d1, 0.f);

        u64 row            = 0;
        for (u32 y = 0; y < m_height; ++y, row += m_width) {
            glm::vec3 d = d0;

            for (u32 x = 0; x < m_width; ++x) {

                u32& under = m_color[row + x];
                if (under == 0xdeadbeef) {
                    auto [face, uv] = sample_cubemap(d);
                    under           = cubemap[face][uv];
                    // under           = glm::packUnorm4x8(glm::vec4(uv.x, uv.y, 0.f, 1.f));
                }

                d += slope_d0;
            }

            d0 += slope_d1;
        }
    }

    inline void Framebuffer::render_equirectangular(
        const DefaultCamera& camera, const Texture& texture, const glm::mat4& inv_view_proj
    ) {

        f32       fov      = camera.fov();
        f32       half_fov = fov * 0.5f;
        glm::vec3 angle    = camera.orientation();

        f32       pi       = glm::pi<f32>();
        f32       inv_2pi  = 1.f / glm::two_pi<f32>();

        f32       yaw0     = angle.x + half_fov;
        f32       pitch    = angle.y - half_fov;

        f32       aspect   = m_width / m_height;
        f32       step_x   = fov * aspect / m_width;
        f32       step_y   = fov / m_height;

        for (u32 y = 0; y < m_height; ++y) {
            f32 yaw = yaw0;

            for (u32 x = 0; x < m_width; ++x) {

                glm::vec3 world          = glm::normalize(screen_to_world(
                    x, y, 1.f, 1.f / m_width * 2.f, 1.f / m_height * 2.f, inv_view_proj
                ));

                yaw                      = atan2f(world.z, world.x);
                pitch                    = asinf(world.y);

                f32 u                    = (yaw + pi) * inv_2pi;
                f32 v                    = 1.f - (pitch + glm::half_pi<f32>()) * inv_2pi;

                m_color[y * m_width + x] = texture[glm::mod(glm::vec2(u, v), glm::vec2(1.f))];

                yaw += step_x;
            }

            pitch += step_y;
        }
    }

} // namespace mt

namespace mt {

    inline std::pair<CubemapFace, glm::vec2>
    sample_cubemap_parallax_corrected(const glm::vec3& o, const glm::vec3& d, const AABB& aabb) {

        glm::vec3 intersection;
        if (!aabb.from_inside_intersect_ray(o, d, intersection))
            return std::make_pair(CubemapFace::NEGATIVE_Z, glm::vec2(0.f));

        return sample_cubemap(intersection - aabb.center());
    }

} // namespace mt

#endif
