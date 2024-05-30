#pragma once

#include "mtpch.hpp"

namespace
{

inline glm::vec2 JsonArrayToVec2(const nlohmann::json& Array)
{
    return glm::vec2(Array[0].get<core::f32>(), Array[1].get<core::f32>());
}

inline glm::ivec2 JsonArrayToIVec2(const nlohmann::json& Array)
{
    return glm::ivec2(Array[0].get<core::i32>(), Array[1].get<core::i32>());
}

} // namespace

#define BLOOM_ENABLED false

namespace core
{

// settings

const glm::ivec2& GetSettingViewResolution();
const glm::ivec2& GetSettingRenderResolution();
const glm::vec2& GetSettingSensitivity();
glm::vec2 GetResolutionRatio();
f32 GetRenderAspectRatio();

const i32& GetSettingRefreshRate();
const b8& GetSettingUseVSync();
const std::string& GetAppName();

const f32& GetSettingAmbientIntensity();

void LoadSettings(const std::string& Path = BUILTINS_DIR + "settings.conf");

// states

const b8& GetWireframe();
void SetWireframe(const b8& Value);
const b8& GetShowMipmaps();
void SetShowMipmaps(const b8& Value);
const b8& GetVisualizeTiles();
void SetVisualizeTiles(const b8& Value);

} // namespace core
