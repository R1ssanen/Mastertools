#include "settings.hpp"

#include "../mtpch.hpp"

namespace {

    glm::ivec2  s_WindowResolution{ 1440, 900 }, s_RenderResolution{ 720, 450 };
    glm::vec2   s_Sensitivity{ 1000.f, 1000.f };

    mt::i32     s_RefreshRate      = 70;
    mt::b8      s_UseVSync         = false;
    std::string s_AppName          = "";

    mt::f32     s_AmbientIntensity = 0.1f;
    mt::u32     s_MaxFPS           = 144;

    mt::b8      s_Fullscreen       = false;
    mt::b8      s_BloomEnabled     = false;
    mt::b8      s_Wireframe = false, s_ShowMipmaps = false, s_VisualizeTiles = false;

} // namespace

namespace mt {

    // settings

    const glm::ivec2& GetSettingWindowResolution() {
        static auto DefaultResolution = glm::ivec2{ 1920, 1080 };
        return s_Fullscreen ? DefaultResolution : s_WindowResolution;
    }

    const glm::ivec2& GetSettingRenderResolution() { return s_RenderResolution; }

    const glm::vec2&  GetSettingSensitivity() { return s_Sensitivity; }

    glm::vec2         GetResolutionRatio() { return s_WindowResolution / s_RenderResolution; }

    f32               GetRenderAspectRatio() {
        return static_cast<f32>(s_RenderResolution.x) / s_RenderResolution.y;
    }

    const i32&         GetSettingRefreshRate() { return s_RefreshRate; }

    const b8&          GetSettingUseVSync() { return s_UseVSync; }

    const std::string& GetAppName() { return s_AppName; }

    const f32&         GetSettingAmbientIntensity() { return s_AmbientIntensity; }

    const u32&         GetSettingMaxFPS() { return s_MaxFPS; }

    const b8&          GetFullscreen() { return s_Fullscreen; }

    void               LoadSettings(const std::string& Path) {
        using json = nlohmann::json;
        std::ifstream File(Path);
        const json    SettingsData{ json::parse(File) };

        for (const auto& [Key, Element] : SettingsData[0].items()) {
            if (Key == "app") {
                s_AppName = " - " + Element.get<std::string>();
            }

            else if (Key == "resolution") {
                s_RenderResolution = JsonArrayToIVec2(Element);
            }

            else if (Key == "windowed_res") {
                s_WindowResolution = JsonArrayToIVec2(Element);
            }

            else if (Key == "fullscreen") {
                s_Fullscreen = Element.get<b8>();
            }

            else if (Key == "refresh") {
                s_RefreshRate = Element.get<i32>();
            }

            else if (Key == "vsync") {
                s_RefreshRate = Element.get<b8>();
            }

            else if (Key == "sensitivity") {
                s_Sensitivity = JsonArrayToVec2(Element);
            }

            else if (Key == "ambient") {
                s_AmbientIntensity = Element.get<f32>();
            }

            else if (Key == "bloom") {
                s_BloomEnabled = Element.get<b8>();
            }

            else if (Key == "max_fps") {
                s_MaxFPS = Element.get<u32>();
            }

            else {
                continue;
            }
        }

        File.close();
    }

    // states

    const b8& GetWireframe() { return s_Wireframe; }

    void      SetWireframe(const b8& Value) { s_Wireframe = Value; }

    const b8& GetShowMipmaps() { return s_ShowMipmaps; }

    void      SetShowMipmaps(const b8& Value) { s_ShowMipmaps = Value; }

    const b8& GetVisualizeTiles() { return s_VisualizeTiles; }

    void      SetVisualizeTiles(const b8& Value) { s_VisualizeTiles = Value; }

    const b8& GetBloomEnabled() { return s_BloomEnabled; }

    void      SetBloomEnabled(const b8& Value) { s_BloomEnabled = Value; }

} // namespace mt
