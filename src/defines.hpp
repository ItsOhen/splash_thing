#pragma once
#include <src/desktop/DesktopTypes.hpp>
#include <src/plugins/PluginAPI.hpp>

#define HOOK_EVENT(PATH, LAMBDA) Event::bus()->m_events.PATH.listen(LAMBDA);
inline HANDLE PHANDLE = nullptr;
