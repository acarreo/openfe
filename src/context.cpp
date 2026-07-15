#include "openfe.h"
#include <stdexcept>

namespace openfe {

std::unique_ptr<OpenFEContext> OpenFE_createContext(OpenFESchemeID id, size_t vector_len) {
    if (id == OpenFE_SCHEME_ABDP15) return std::make_unique<OpenFEContextABDP15>(vector_len);
    throw std::invalid_argument("Unknown OpenFESchemeID");
}

}
