#include <Windows.h>

namespace {

void writeDebugMessage(const wchar_t* message) noexcept {
    OutputDebugStringW(L"[DoABarrelRoll] ");
    OutputDebugStringW(message);
    OutputDebugStringW(L"\n");
}

DWORD WINAPI initializeThread(void*) noexcept {
    // Keep complex initialization out of DllMain. The actual game-version-specific
    // hooks must be installed here only after their signatures are verified.
    writeDebugMessage(L"Native DLL loaded. Game hooks are not implemented yet.");
    return 0;
}

} // namespace

BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(instance);
        HANDLE thread = CreateThread(nullptr, 0, initializeThread, nullptr, 0, nullptr);
        if (thread) {
            CloseHandle(thread);
        }
    }
    return TRUE;
}
