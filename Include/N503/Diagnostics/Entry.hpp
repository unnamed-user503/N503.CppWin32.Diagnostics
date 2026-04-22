#pragma once

// 1. Project Headers

// 2. Project Dependencies
#include <N503/Diagnostics/Severity.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <string>
#include <string_view>

namespace N503::Diagnostics
{

    struct Entry final
    {
        Diagnostics::Severity Severity{ Severity::Verbose };

        std::string Expected{ "" };

        std::size_t Position{ 0 };

        auto ToString() const -> std::string
        {
            auto SeverityToString = [](Diagnostics::Severity severity) -> std::string_view
            {
                switch (severity)
                {
                    case Severity::Verbose:
                        return "Verbose";

                    case Severity::Warning:
                        return "Warning";

                    case Severity::Error:
                        return "Error";

                    default:
                        return "Unknown";
                }
            };

            std::string result = "[";
            result += SeverityToString(Severity);

            if (0 < Position)
            {
                result += "(Line: ";
                result += std::to_string(Position);
                result += ")";
            }

            result += "] : ";
            result += Expected.empty() ? "(none)" : Expected;

            return result;
        }
    };

} // namespace N503::Diagnostics
