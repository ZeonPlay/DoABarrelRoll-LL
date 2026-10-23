#pragma once

#include <Windows.h>

#include <cstddef>
#include <cstdint>
#include <optional>
#include <string_view>
#include <vector>

namespace dabr::native {

struct BytePattern {
    std::vector<std::uint8_t> bytes;
    std::vector<bool> significant;

    [[nodiscard]] bool valid() const noexcept;
};

// Pattern format: "48 8B ?? ?? 89". Whitespace separates bytes;
// ?? matches any byte. Invalid tokens return std::nullopt.
[[nodiscard]] std::optional<BytePattern> parsePattern(std::string_view text);

// Scan only executable PE sections in an already-loaded module in this process.
// Returns every match so callers can reject ambiguous signatures.
[[nodiscard]] std::vector<std::uintptr_t> scanExecutableSections(
    HMODULE module,
    const BytePattern& pattern
);

} // namespace dabr::native
