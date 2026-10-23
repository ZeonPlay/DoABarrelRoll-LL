#include "native/signature_scanner.hpp"

#include <charconv>
#include <limits>
#include <string>
#include <system_error>

namespace dabr::native {
namespace {

bool parseHexByte(std::string_view token, std::uint8_t& value) noexcept {
    if (token.size() != 2) return false;
    unsigned int parsed = 0;
    const auto result = std::from_chars(token.data(), token.data() + token.size(), parsed, 16);
    if (result.ec != std::errc{} || result.ptr != token.data() + token.size() || parsed > 0xFF) {
        return false;
    }
    value = static_cast<std::uint8_t>(parsed);
    return true;
}

bool matches(const std::uint8_t* candidate, const BytePattern& pattern) noexcept {
    for (std::size_t i = 0; i < pattern.bytes.size(); ++i) {
        if (pattern.significant[i] && candidate[i] != pattern.bytes[i]) return false;
    }
    return true;
}

} // namespace

bool BytePattern::valid() const noexcept {
    return !bytes.empty() && bytes.size() == significant.size();
}

std::optional<BytePattern> parsePattern(std::string_view text) {
    BytePattern result;
    std::size_t cursor = 0;

    while (cursor < text.size()) {
        while (cursor < text.size() && (text[cursor] == ' ' || text[cursor] == '\t' ||
               text[cursor] == '\n' || text[cursor] == '\r')) {
            ++cursor;
        }
        if (cursor == text.size()) break;

        const std::size_t start = cursor;
        while (cursor < text.size() && text[cursor] != ' ' && text[cursor] != '\t' &&
               text[cursor] != '\n' && text[cursor] != '\r') {
            ++cursor;
        }

        const std::string_view token = text.substr(start, cursor - start);
        if (token == "??") {
            result.bytes.push_back(0);
            result.significant.push_back(false);
            continue;
        }

        std::uint8_t byte = 0;
        if (!parseHexByte(token, byte)) return std::nullopt;
        result.bytes.push_back(byte);
        result.significant.push_back(true);
    }

    if (!result.valid()) return std::nullopt;
    return result;
}

std::vector<std::uintptr_t> scanExecutableSections(HMODULE module, const BytePattern& pattern) {
    std::vector<std::uintptr_t> matchesFound;
    if (module == nullptr || !pattern.valid()) return matchesFound;

    const auto* base = reinterpret_cast<const std::uint8_t*>(module);
    const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(base);
    if (dos->e_magic != IMAGE_DOS_SIGNATURE || dos->e_lfanew <= 0) return matchesFound;

    const auto* nt = reinterpret_cast<const IMAGE_NT_HEADERS64*>(base + dos->e_lfanew);
    if (nt->Signature != IMAGE_NT_SIGNATURE ||
        nt->OptionalHeader.Magic != IMAGE_NT_OPTIONAL_HDR64_MAGIC) {
        return matchesFound;
    }

    const auto* section = IMAGE_FIRST_SECTION(nt);
    for (WORD i = 0; i < nt->FileHeader.NumberOfSections; ++i, ++section) {
        if ((section->Characteristics & IMAGE_SCN_MEM_EXECUTE) == 0) continue;

        const std::size_t sectionSize = section->Misc.VirtualSize;
        const std::size_t patternSize = pattern.bytes.size();
        if (sectionSize < patternSize) continue;

        const auto* start = base + section->VirtualAddress;
        const std::size_t lastOffset = sectionSize - patternSize;
        for (std::size_t offset = 0; offset <= lastOffset; ++offset) {
            if (matches(start + offset, pattern)) {
                matchesFound.push_back(reinterpret_cast<std::uintptr_t>(start + offset));
            }
        }
    }

    return matchesFound;
}

} // namespace dabr::native
