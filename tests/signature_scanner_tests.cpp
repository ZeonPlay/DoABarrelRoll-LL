#include "native/signature_scanner.hpp"

#include <cassert>

int main() {
    using dabr::native::parsePattern;

    const auto exact = parsePattern("48 8B 01");
    assert(exact.has_value());
    assert(exact->valid());
    assert(exact->bytes.size() == 3);
    assert(exact->bytes[0] == 0x48 && exact->significant[0]);
    assert(exact->bytes[1] == 0x8B && exact->significant[1]);

    const auto wildcard = parsePattern(" 48\t8B ?? 0F ");
    assert(wildcard.has_value());
    assert(wildcard->bytes.size() == 4);
    assert(!wildcard->significant[2]);
    assert(wildcard->significant[3]);

    assert(!parsePattern("").has_value());
    assert(!parsePattern("GG").has_value());
    assert(!parsePattern("A").has_value());
    assert(!parsePattern("??Z").has_value());

    return 0;
}
