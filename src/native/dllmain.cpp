#include <Windows.h>
#include <strsafe.h>
#include <iterator>
#include <cstdint>
#include <cwchar>

namespace {
constexpr wchar_t kLogName[] = L"DoABarrelRoll-diagnostics.log";
void appendLog(const wchar_t* message) noexcept {
    wchar_t tempPath[MAX_PATH]{};
    const DWORD length = GetTempPathW(MAX_PATH, tempPath);
    if (length == 0 || length >= MAX_PATH) { OutputDebugStringW(L"[DoABarrelRoll] Could not resolve TEMP path.\n"); return; }
    wchar_t logPath[MAX_PATH]{};
    if (FAILED(StringCchCopyW(logPath, MAX_PATH, tempPath))) return;
    if (FAILED(StringCchCatW(logPath, MAX_PATH, kLogName))) return;
    HANDLE file = CreateFileW(logPath, FILE_APPEND_DATA, FILE_SHARE_READ | FILE_SHARE_WRITE,
                              nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (file == INVALID_HANDLE_VALUE) { OutputDebugStringW(L"[DoABarrelRoll] Could not open diagnostics log.\n"); return; }
    const DWORD chars = static_cast<DWORD>(wcslen(message));
    DWORD written = 0;
    WriteFile(file, message, chars * sizeof(wchar_t), &written, nullptr);
    const wchar_t newline[] = L"\r\n";
    WriteFile(file, newline, sizeof(newline) - sizeof(wchar_t), &written, nullptr);
    CloseHandle(file);
    OutputDebugStringW(message);
    OutputDebugStringW(L"\n");
}
void logMainModuleInfo() noexcept {
    appendLog(L"=== DoABarrelRoll native diagnostic build ===");
    HMODULE mainModule = GetModuleHandleW(nullptr);
    if (!mainModule) { appendLog(L"ERROR: GetModuleHandleW(nullptr) failed."); return; }
    wchar_t modulePath[32768]{};
    const DWORD pathLength = GetModuleFileNameW(mainModule, modulePath, static_cast<DWORD>(std::size(modulePath)));
    if (pathLength > 0 && pathLength < std::size(modulePath)) appendLog(modulePath);
    else appendLog(L"Main module path unavailable.");
    const auto* base = reinterpret_cast<const std::uint8_t*>(mainModule);
    const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
    if (dos->e_magic != IMAGE_DOS_SIGNATURE || dos->e_lfanew <= 0 || dos->e_lfanew > 0x100000) {
        appendLog(L"ERROR: main module does not have a valid in-memory DOS header."); return;
    }
    const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS64*>(base + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE || nt->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC) {
        appendLog(L"ERROR: main module does not have a valid PE32+ header."); return;
    }
    wchar_t line[512]{};
    swprintf_s(line, L"Main module base=%p SizeOfImage=0x%X TimeDateStamp=0x%08X CheckSum=0x%08X Machine=0x%04X Sections=%u",
               mainModule, nt->OptionalHeader.SizeOfImage, nt->FileHeader.TimeDateStamp,
               nt->OptionalHeader.CheckSum, nt->FileHeader.Machine, nt->FileHeader.NumberOfSections);
    appendLog(line);
    const auto* section = IMAGE_FIRST_SECTION(nt);
    for (WORD i = 0; i < nt->FileHeader.NumberOfSections; ++i, ++section) {
        char name[IMAGE_SIZEOF_SHORT_NAME + 1]{};
        for (size_t j = 0; j < IMAGE_SIZEOF_SHORT_NAME; ++j) name[j] = static_cast<char>(section->Name[j]);
        swprintf_s(line, L"Section %u: %.8hs RVA=0x%X VirtualSize=0x%X RawSize=0x%X Flags=0x%08X",
                   i, name, section->VirtualAddress, section->Misc.VirtualSize, section->SizeOfRawData, section->Characteristics);
        appendLog(line);
    }
    appendLog(L"Diagnostic only: no gameplay hooks are installed by this build.");
}
DWORD WINAPI initializeThread(void*) noexcept { logMainModuleInfo(); return 0; }
} // namespace
BOOL WINAPI DllMain(HINSTANCE instance, DWORD reason, LPVOID) {
    if (reason == DLL_PROCESS_ATTACH) {
        DisableThreadLibraryCalls(instance);
        HANDLE thread = CreateThread(nullptr, 0, initializeThread, nullptr, 0, nullptr);
        if (thread) CloseHandle(thread);
    }
    return TRUE;
}
