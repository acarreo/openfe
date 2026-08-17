#ifndef OPENFE_TESTS_TESTUTIL_H
#define OPENFE_TESTS_TESTUTIL_H

#include "bpdpvs.h"

namespace openfe_test {

inline ZP groupOrder() {
    BPGroup group;
    return group.getGroupOrder();
}

template <typename T>
T roundTrip(const T &in) {
    OpenABEByteString bytes;
    in.serialize(bytes);
    T out;
    out.deserialize(bytes);
    return out;
}

inline ZP roundTripZP(const ZP &in, const ZP &ord) {
    OpenABEByteString bytes;
    in.serialize(bytes);
    ZP out;
    out.setOrder(ord.m_ZP);
    out.deserialize(bytes);
    return out;
}

}

#endif
