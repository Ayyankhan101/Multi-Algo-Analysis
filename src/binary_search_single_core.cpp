// This file exists to satisfy CMakeLists.txt build requirements.
// The binary_search implementation is header-only in binary_search_single_core.hpp
// This file ensures the library can be linked properly in the CMake build system.

#include "binary_search_single_core.hpp"

// Explicit template instantiation (if needed in the future)
// Currently all functions are defined in the header and will be inlined
