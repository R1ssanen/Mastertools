#pragma once

#include <cstdint>
#include <memory>

namespace core {

	using b8 = bool;

	using i8 = std::int8_t;
	using i16 = std::int16_t;
	using i32 = std::int32_t;
	using i64 = std::int64_t;

	using u8 = std::uint8_t;
	using u16 = std::uint16_t;
	using u32 = std::uint32_t;
	using u64 = std::uint64_t;

	using f32 = float_t;
	using f64 = double_t;

	static_assert(sizeof(b8) == 1, "Size of bool must be 1 byte.");
    static_assert(sizeof(f32) == 4, "Size of float 32-bits must be 4 bytes.");
    static_assert(sizeof(f64) == 8, "Size of float 64-bits must be 8 bytes.");

	template<typename T> using Shared = std::shared_ptr<T>;
	template<typename T> using Unique = std::unique_ptr<T>;
}
