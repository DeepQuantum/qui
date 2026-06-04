#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

#include <qui/typography.hpp>

namespace qui {

std::filesystem::path executable_directory() {
#if defined(_WIN32)
    wchar_t buffer[MAX_PATH];
    const DWORD length = GetModuleFileNameW(nullptr, buffer, MAX_PATH);
    if (length > 0 && length < MAX_PATH) {
        return std::filesystem::path(std::wstring(buffer, length)).parent_path();
    }
#endif
    return std::filesystem::current_path();
}

std::filesystem::path font_directory() {
    return executable_directory() / "fonts";
}

} // namespace qui
