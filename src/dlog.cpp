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

    long range = 2 * bound + 1;
    long m = static_cast<long>(std::ceil(std::sqrt(static_cast<double>(range))));
    if (m < 1) m = 1;

    std::unordered_map<std::string, long> babySteps;
    babySteps.reserve(static_cast<size_t>(m));

    G1 baby;
    for (long j = 0; j < m; ++j) {
        babySteps.emplace(toKey(baby), j);
        baby = baby + g;
    }

    ZP zm(static_cast<uint32_t>(m));
    ZP zbound(static_cast<uint32_t>(bound));
    ZP negZm = ZP(0u) - zm;
    G1 negStep = g * negZm;
    G1 giant = h + (g * zbound);

    for (long i = 0; i < m; ++i) {
        auto it = babySteps.find(toKey(giant));
        if (it != babySteps.end()) {
            long x_shifted = i * m + it->second;
            return x_shifted - bound;
        }
        giant = giant + negStep;
    }

    throw std::runtime_error("discreteLogBSGS: no solution found in [-bound, bound]");
}

}
