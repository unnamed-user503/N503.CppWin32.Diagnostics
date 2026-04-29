#pragma once

// 1. Project Headers

// 2. Project Dependencies

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <string>

namespace N503::Diagnostics
{

    enum class Severity
    {
        Verbose,
        Warning,
        Error
    };

    struct Entry
    {
        Severity Severity;
        std::wstring Expected;
        std::size_t Position;
    };

} // namespace N503::Diagnostics
