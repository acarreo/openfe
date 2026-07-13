#ifndef OPENFE_OPENFE_H
#define OPENFE_OPENFE_H

#include "openfe/context.h"
#include "openfe/contextksw08.h"

#include <memory>

namespace openfe {

enum OpenFESchemeID {
    OpenFE_SCHEME_KSW08
};

std::unique_ptr<OpenFEContext> OpenFE_createContext(OpenFESchemeID id, size_t vector_len);

int self_check(); // vérif rapide des types bpdpvs
}

#endif
