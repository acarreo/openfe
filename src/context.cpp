#include "openfe.h"
#include <stdexcept>

namespace openfe {

std::unique_ptr<OpenFEContext> OpenFE_createContext(OpenFESchemeID id, size_t vector_len) {
    switch (id) {
        case OpenFE_SCHEME_KSW08:
            return std::make_unique<OpenFEContextKSW08>(vector_len);
        default:
            throw std::invalid_argument("Unknown OpenFESchemeID");
    }
}

}
