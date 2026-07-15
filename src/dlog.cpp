#include "openfe/dlog.h"

#include <cmath>
#include <cstdint>
#include <cstdlib>
#include <stdexcept>
#include <string>
#include <unordered_map>

namespace openfe {

namespace {

std::string toKey(const G1 &p) {
    int size = 0;
    uint8_t *bytes = p.getBytes(&size);
    std::string key(reinterpret_cast<char *>(bytes), static_cast<size_t>(size));
    free(bytes);
    return key;
}

}

long discreteLogBSGS(const G1 &g, const G1 &h, long bound) {
    if (bound < 0) throw std::invalid_argument("discreteLogBSGS: bound must be non-negative");

        long m = static_cast<long>(std::ceil(std::sqrt(static_cast<double>(2 * bound + 1))));
    if (m < 1) m = 1;

        std::unordered_map<std::string, long> babies;
        babies.reserve(static_cast<size_t>(m));

    G1 baby;
    for (long j = 0; j < m; ++j) {
            babies.emplace(toKey(baby), j);
        baby = baby + g;
    }

        ZP zm(static_cast<uint32_t>(m));
        G1 step = g * (ZP(0u) - zm);
        G1 giant = h + g * ZP(static_cast<uint32_t>(bound));

    for (long i = 0; i < m; ++i) {
            auto it = babies.find(toKey(giant));
            if (it != babies.end()) return i * m + it->second - bound;
            giant = giant + step;
    }

    throw std::runtime_error("discreteLogBSGS: no solution found in [-bound, bound]");
}

}
