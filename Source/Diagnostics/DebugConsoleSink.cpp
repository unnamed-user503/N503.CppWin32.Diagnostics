#include "Pch.hpp"

// 1. Project Headers

// 2. Project Dependencies
#include <N503/Diagnostics/DebugConsoleSink.hpp>
#include <N503/Diagnostics/Entry.hpp>
#include <N503/Diagnostics/Sink.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers
#include <Windows.h>

// 6. C++ Standard Libraries
#include <string>
#include <string_view>
#include <vector>

namespace N503::Diagnostics
{

    namespace
    {
        auto TranscodeUtf8ToWide(const std::string_view utf8) -> std::wstring
        {
            if (utf8.empty())
            {
                return {};
            }

            int desired = ::MultiByteToWideChar(CP_UTF8, 0, utf8.data(), -1, nullptr, 0);
            if (desired == 0)
            {
                return {};
            }

            std::wstring result(desired, 0);
            ::MultiByteToWideChar(CP_UTF8, 0, utf8.data(), -1, &result[0], desired);

            result.resize(desired - 1);
            return result;
        }
    } // namespace

    auto DebugConsoleSink::Report(std::vector<Entry> entries) -> void
    {
        Sink::Report(entries);

        for (const auto& entry : entries)
        {
            ::OutputDebugStringW((entry.ToString() + L"\r\n").data());
        }
    }

} // namespace N503::Diagnostics
