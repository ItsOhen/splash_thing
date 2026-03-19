#include "defines.hpp"
#include <hyprland/src/render/Renderer.hpp>
#include <hyprutils/path/Path.hpp>
#include <random>
#include <ranges>
#include <src/Compositor.hpp>
#include <src/SharedDefs.hpp>
#include <src/helpers/fs/FsUtils.hpp>
#include <src/plugins/HookSystem.hpp>
#include <src/plugins/PluginAPI.hpp>

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
  for (auto &m : g_pCompositor->m_monitors) {
    m->m_splash = g_pHyprRenderer->renderSplash([m](auto width, auto height, const auto DATA) { return g_pHyprRenderer->createTexture(width, height, DATA); }, m->m_transformedSize.y / 76,
                                                m->m_transformedSize.x, m->m_transformedSize.y);
  }
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

  HyprlandAPI::addDispatcherV2(PHANDLE, "setsplash", [&](std::string cmd) -> SDispatchResult {
    if (cmd.empty())
      return {.success = false, .error = "Usage: setsplash <text>"};
    g_pCompositor->m_currentSplash = cmd;
    updateRendererTex();
    Log::logger->log(Log::ERR, "Splash set to {}", g_pCompositor->m_currentSplash);
    return {};
  });

  HyprlandAPI::addDispatcherV2(PHANDLE, "randomsplash", [&](std::string cmd) -> SDispatchResult {
    g_pCompositor->m_currentSplash = getRandomSplash();
    updateRendererTex();
    return {};
  });

  if (!splashes.empty()) {
    g_pCompositor->m_currentSplash = getRandomSplash();
    updateRendererTex();
  }

  return {PLUGIN_NAME, PLUGIN_DESCRIPTION, PLUGIN_AUTHOR, PLUGIN_VERSION};
}

APICALL EXPORT void PLUGIN_EXIT() {
}
