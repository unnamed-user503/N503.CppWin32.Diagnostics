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

        std::wstring Expected{ L"" };

        std::size_t Position{ 0 };

        auto ToString() const -> std::wstring
        {
            auto SeverityToString = [](Diagnostics::Severity severity) -> std::wstring_view
            {
                switch (severity)
                {
                    case Severity::Verbose:
                        return L"Verbose";

                    case Severity::Warning:
                        return L"Warning";

                    case Severity::Error:
                        return L"Error";

                    default:
                        return L"Unknown";
                }
            };

            std::wstring result  = L"[";
            result              += SeverityToString(Severity);

            if (0 < Position)
            {
                result += L"(Line: ";
                result += std::to_wstring(Position);
                result += L")";
            }

            result += L"] : ";
            result += Expected.empty() ? L"(none)" : Expected;

            return result;
        }
    };

} // namespace N503::Diagnostics
