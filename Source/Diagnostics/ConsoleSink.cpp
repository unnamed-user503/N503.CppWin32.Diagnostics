#include "Pch.hpp"

// 1. Project Headers

// 2. Project Dependencies
#include <N503/Diagnostics/ConsoleSink.hpp>
#include <N503/Diagnostics/Entry.hpp>
#include <N503/Diagnostics/Severity.hpp>
#include <N503/Diagnostics/Sink.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <iostream>
#include <vector>

namespace N503::Diagnostics
{

    auto ConsoleSink::Report(std::vector<Entry> entries) -> void
    {
        Sink::Report(entries);

        for (const auto& entry : entries)
        {
            auto& outputStream = (entry.Severity == Diagnostics::Severity::Error) ? std::cerr : std::cout;

            outputStream << entry.ToString() << std::endl;
        }
    }

} // namespace N503::Diagnostics
