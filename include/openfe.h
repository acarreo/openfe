#ifndef OPENFE_OPENFE_H
#define OPENFE_OPENFE_H

#include "openfe/context.h"
#include "openfe/contextabdp15.h"

#include <memory>

namespace openfe {

enum OpenFESchemeID {
    OpenFE_SCHEME_ABDP15
};

std::unique_ptr<OpenFEContext> OpenFE_createContext(OpenFESchemeID id, size_t vector_len);

int self_check();
}

#endif
