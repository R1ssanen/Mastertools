#pragma once

#include "srpch.hpp"

namespace
{

inline glm::vec2 JsonArrayToVec2(const nlohmann::json& t_Array)
{
    return glm::vec2(t_Array[0].get<float>(), t_Array[1].get<float>());
}

inline glm::ivec2 JsonArrayToIVec2(const nlohmann::json& t_Array)
{
    return glm::ivec2(t_Array[0].get<int>(), t_Array[1].get<int>());
}

} // namespace

namespace core
{

// settings

const glm::ivec2& GetSettingViewResolution();
const glm::ivec2& GetSettingRenderResolution();
const glm::vec2& GetSettingSensitivity();

const int& GetSettingRefreshRate();
const bool& GetSettingUseVSync();
const std::string& GetAppName();

const float& GetSettingAmbientIntensity();

void LoadSettings(const std::string& t_Path = BUILTINS_DIR + "settings.conf");

// states

const bool& GetWireframe();
void SetWireframe(const bool& t_Value);
const bool& GetShowMipmaps();
void SetShowMipmaps(const bool& t_Value);
const bool& GetVisualizeTiles();
void SetVisualizeTiles(const bool& t_Value);

} // namespace core
