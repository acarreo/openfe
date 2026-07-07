#include <iostream>

#include "openfe.h"

int main() {
    const int rc = openfe::self_check();
    std::cout << "openfe + bpdpvs compile check: " << rc << std::endl;
    return rc;
}
