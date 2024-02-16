#pragma once

#include "srpch.hpp"

namespace core {

    class Buffer {
        public:
            uint32_t* GetColorBuffer() const { return m.ColorBuffer; }
            float* GetDepthBuffer() const { return m.DepthBuffer; }
            const int& GetWidth() const { return m.Width; }
            const int& GetHeight() const { return m.Height; }

            void SetPixel(size_t t_Loc, uint32_t t_Color) { m.ColorBuffer[t_Loc] = t_Color; }
            const uint32_t& GetPixel(size_t t_Loc) { return m.ColorBuffer[t_Loc]; }
            void SetDepth(size_t t_Loc, float t_Depth) { m.DepthBuffer[t_Loc] = t_Depth; }
            const float& GetDepth(size_t t_Loc) { return m.DepthBuffer[t_Loc]; }

            Buffer(const Buffer& t_Other) { m = t_Other.m; }
            static std::shared_ptr<Buffer> New(int t_Width, int t_Height) {
                return std::make_shared<Buffer>(
                    Buffer(
                        _M{
                            .ColorBuffer = new uint32_t[t_Width * t_Height],
                            .DepthBuffer = new float[t_Width * t_Height],
                            .Width = t_Width,
                            .Height = t_Height
                        }
                    )
                );
            }

        private:
            struct _M {
                uint32_t* ColorBuffer;
                float* DepthBuffer;
                int Width, Height;
            } m;

            explicit Buffer(_M&& t_Data) : m{std::move(t_Data)} {}
    };

    using buffer_t = std::shared_ptr<Buffer>;

}
