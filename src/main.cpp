#include "defines.hpp"
#include <config/shared/Types.hpp>
#include <hyprland/src/render/Renderer.hpp>
#include <hyprutils/path/Path.hpp>
#include <random>
#include <ranges>
#include <src/Compositor.hpp>
#include <src/SharedDefs.hpp>
#include <src/helpers/fs/FsUtils.hpp>
#include <src/plugins/HookSystem.hpp>
#include <src/plugins/PluginAPI.hpp>
#include <hyprland/src/config/ConfigManager.hpp>

extern "C" {
#include <lua.h>
#include <lualib.h>
#include <lauxlib.h>
}

APICALL EXPORT std::string PLUGIN_API_VERSION() {
  return HYPRLAND_API_VERSION;
}

static std::vector<std::string> splashes;

std::string getRandomSplash() {
  if (splashes.empty())
    return "No splashes found!";

  static std::mt19937 gen(std::random_device{}());
  std::uniform_int_distribution<size_t> dist(0, splashes.size() - 1);

  return splashes[dist(gen)];
}

void updateRendererTex() {
  static auto PSPLASHFONTSIZE = CConfigValue<Config::INTEGER>("misc:splash_font_size");

  for (auto &m : g_pCompositor->m_monitors) {
    int fontSize = *PSPLASHFONTSIZE > 0 ? *PSPLASHFONTSIZE : m->m_transformedSize.y / 76;

    m->m_splash = g_pHyprRenderer->renderSplash([m](auto width, auto height, const auto DATA) { return g_pHyprRenderer->createTexture(width, height, DATA); }, 
                                                fontSize,
                                                m->m_transformedSize.x, m->m_transformedSize.y);
  }
}

#define REG_SPLASH(name, luaName, ...) \
    HyprlandAPI::addDispatcherV2(PHANDLE, name, [](std::string cmd) -> SDispatchResult { __VA_ARGS__; return {}; }); \
    if constexpr (requires { HyprlandAPI::addLuaFunction(nullptr, "", "", nullptr); }) { \
        HyprlandAPI::addLuaFunction(PHANDLE, "splash_thing", luaName, [](lua_State* L) -> int { \
            std::string cmd = ""; \
            if (lua_isstring(L, 1)) { \
                cmd = lua_tostring(L, 1); \
            } \
            __VA_ARGS__; \
            return 0; \
        }); \
    }

APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle) {
  PHANDLE = handle;
  if (const std::string hash = __hyprland_api_get_hash(); hash != __hyprland_api_get_client_hash())
    throw std::runtime_error("Version mismatch");

  const auto config = Hyprutils::Path::findConfig("splashes");
  if (!config.first.has_value())
    throw std::runtime_error("Could not get config dir");
  auto file = NFsUtils::readFileAsString(config.first.value());
  if (!file.has_value())
    Log::logger->log(Log::ERR, "Could not read file {}", config.first.value());
  else {
    splashes = *file | std::views::split('\n') |
               std::views::filter([](auto &&s) { return !s.empty(); }) |
               std::views::transform([](auto &&s) { return std::string(s.begin(), s.end()); }) |
               std::ranges::to<std::vector<std::string>>();
  }

  REG_SPLASH("setsplash", "set", if(!cmd.empty()) { g_pCompositor->m_currentSplash = cmd; updateRendererTex(); })
  REG_SPLASH("addsplash", "add", if(!cmd.empty()) splashes.push_back(cmd);)
  REG_SPLASH("removesplash", "remove", if(!cmd.empty()) std::erase_if(splashes, [&](auto& s){ return s.starts_with(cmd); });)
  REG_SPLASH("randomsplash", "random", g_pCompositor->m_currentSplash = getRandomSplash(); updateRendererTex();)

  if (!splashes.empty()) {
    g_pCompositor->m_currentSplash = getRandomSplash();
    updateRendererTex();
  }

  return {PLUGIN_NAME, PLUGIN_DESCRIPTION, PLUGIN_AUTHOR, PLUGIN_VERSION};
}

#undef REG_SPLASH

APICALL EXPORT void PLUGIN_EXIT() {
}
