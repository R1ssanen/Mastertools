#include "settings.hpp"

#include "mtpch.hpp"

namespace
{

glm::ivec2 s_ViewResolution{1440, 900}, s_RenderResolution{720, 450};
glm::vec2 s_Sensitivity{1000.f, 1000.f};

core::i32   s_RefreshRate = 70;
core::b8    s_UseVSync    = false;
std::string s_AppName;

core::f32   s_AmbientIntensity = 0.1f;

core::b8    s_Wireframe = false, s_ShowMipmaps = false, s_VisualizeTiles = false;

} // namespace

namespace core
{

// settings

const glm::ivec2& GetSettingViewResolution() { return s_ViewResolution; }
const glm::ivec2& GetSettingRenderResolution() { return s_RenderResolution; }
const glm::vec2& GetSettingSensitivity() { return s_Sensitivity; }
glm::vec2 GetResolutionRatio() { return s_ViewResolution / s_RenderResolution; }
f32 GetRenderAspectRatio() { return static_cast<f32>(s_RenderResolution.x) / s_RenderResolution.y; }

const i32& GetSettingRefreshRate() { return s_RefreshRate; }
const b8& GetSettingUseVSync() { return s_UseVSync; }
const std::string& GetAppName() { return s_AppName; }


const f32& GetSettingAmbientIntensity() { return s_AmbientIntensity; }

void LoadSettings(const std::string& Path)
{
    using json = nlohmann::json;
    std::ifstream File(Path);
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
            s_RefreshRate = Element.get<i32>();
        }

        else if (Key == "vsync")
        {
            s_RefreshRate = Element.get<b8>();
        }

        else if (Key == "sensitivity")
        {
            s_Sensitivity = JsonArrayToVec2(Element);
        }

        else if (Key == "ambient")
        {
            s_AmbientIntensity = Element.get<f32>();
        }

        else
        {
            continue;
        }
    }

    File.close();
}

// states

const b8& GetWireframe() { return s_Wireframe; }
void SetWireframe(const b8& Value) { s_Wireframe = Value; }
const b8& GetShowMipmaps() { return s_ShowMipmaps; }
void SetShowMipmaps(const b8& Value) { s_ShowMipmaps = Value; }
const b8& GetVisualizeTiles() { return s_VisualizeTiles; }
void SetVisualizeTiles(const b8& Value) { s_VisualizeTiles = Value; }

} // namespace core
