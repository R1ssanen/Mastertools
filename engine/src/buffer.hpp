#pragma once

#include "mtpch.hpp"

namespace core {

    class Buffer {
        public:
            u32* GetColorBuffer() const { return m.ColorBuffer; }
            f32* GetDepthBuffer() const { return m.DepthBuffer; }
            const i32& GetWidth() const { return m.Width; }
            const i32& GetHeight() const { return m.Height; }
            i32 GetResolution() const { return m.Width * m.Height; }

            void SetPixel(u64 Loc, u32 Color) { m.ColorBuffer[Loc] = Color; }
            const u32& GetPixel(u64 Loc) { return m.ColorBuffer[Loc]; }
            void SetDepth(u64 Loc, f32 Depth) { m.DepthBuffer[Loc] = Depth; }
            const f32& GetDepth(u64 Loc) { return m.DepthBuffer[Loc]; }

            Buffer(const Buffer& Other) { m = Other.m; }
            static std::shared_ptr<Buffer> New(i32 Width, i32 Height) {
                return std::make_shared<Buffer>(
                    Buffer(
                        _M{
                            .ColorBuffer = new u32[Width * Height],
                            .DepthBuffer = new f32[Width * Height],
                            .Width = Width,
                            .Height = Height
                        }
                    )
                );
            }

        private:
            struct _M {
                u32* ColorBuffer;
                f32* DepthBuffer;
                i32 Width, Height;
            } m;

            explicit Buffer(_M&& Data) : m{std::move(Data)} {}
    };

    using buffer_t = std::shared_ptr<Buffer>;

}
