#include "settings.hpp"

#include "srpch.hpp"

namespace
{

glm::ivec2 s_ViewResolution{1440, 900}, s_RenderResolution{720, 450};
glm::vec2 s_Sensitivity{1000.f, 1000.f};

int s_RefreshRate = 70;
bool s_UseVSync = false;
std::string s_AppName;

float s_AmbientIntensity = 0.1f;

bool s_Wireframe = false, s_ShowMipmaps = false, s_VisualizeTiles = false;

} // namespace

namespace core
{

// settings

const glm::ivec2& GetSettingViewResolution() { return s_ViewResolution; }
const glm::ivec2& GetSettingRenderResolution() { return s_RenderResolution; }
const glm::vec2& GetSettingSensitivity() { return s_Sensitivity; }
glm::vec2 GetResolutionRatio() { return s_ViewResolution / s_RenderResolution; }

const int& GetSettingRefreshRate() { return s_RefreshRate; }
const bool& GetSettingUseVSync() { return s_UseVSync; }
const std::string& GetAppName() { return s_AppName; }


const float& GetSettingAmbientIntensity() { return s_AmbientIntensity; }

void LoadSettings(const std::string& t_Path)
{
    using json = nlohmann::json;
    std::ifstream File(t_Path);
    const json SettingsData{json::parse(File)};

    for (const auto& [Key, Element] : SettingsData[0].items())
    {
        if (Key == "app")
        {
            s_AppName = " - " + Element.get<std::string>();
        }

        else if (Key == "resolution")
        {
            s_ViewResolution = JsonArrayToIVec2(Element["view"]);
            s_RenderResolution = JsonArrayToIVec2(Element["render"]);
        }

        else if (Key == "refresh")
        {
            s_RefreshRate = Element.get<int>();
        }

        else if (Key == "vsync")
        {
            s_RefreshRate = Element.get<bool>();
        }

        else if (Key == "sensitivity")
        {
            s_Sensitivity = JsonArrayToVec2(Element);
        }

        else if (Key == "ambient")
        {
            s_AmbientIntensity = Element.get<float>();
        }

        else
        {
            continue;
        }
    }

    File.close();
}

// states

const bool& GetWireframe() { return s_Wireframe; }
void SetWireframe(const bool& t_Value) { s_Wireframe = t_Value; }
const bool& GetShowMipmaps() { return s_ShowMipmaps; }
void SetShowMipmaps(const bool& t_Value) { s_ShowMipmaps = t_Value; }
const bool& GetVisualizeTiles() { return s_VisualizeTiles; }
void SetVisualizeTiles(const bool& t_Value) { s_VisualizeTiles = t_Value; }

} // namespace core
