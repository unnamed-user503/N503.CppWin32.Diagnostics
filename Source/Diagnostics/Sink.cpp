#include "Pch.hpp"

// 1. Project Headers

// 2. Project Dependencies
#include <N503/Diagnostics/Entry.hpp>
#include <N503/Diagnostics/Severity.hpp>
#include <N503/Diagnostics/Sink.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <algorithm>
#include <iterator>
#include <string>
#include <string_view>
#include <utility>
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

    Sink::Sink()
    {
        m_Entries.reserve(MaxCapacity + 128);
    }

    auto Sink::AddEntry(const Entry& entry) -> void
    {
        const std::size_t limit       = MaxCapacity;
        const std::size_t currentSize = m_Entries.size();

        if (currentSize >= limit)
        {
            if (entry.Severity >= Severity::Error && currentSize < limit + 128)
            {
                m_Entries.push_back(entry);
                return;
            }

            if (currentSize == limit && !m_Overheated)
            {
                m_Entries.push_back(Entry{
                    .Severity = Severity::Warning,
                    .Expected = L"!!! Diagnostics log buffer is full. Subsequent logs (except Errors) are being dropped. !!!",
                    .Position = 0,
                });

                m_Overheated = true;
            }

            return;
        }

        m_Entries.push_back(entry);
    }

    auto Sink::AddEntry(Diagnostics::Severity severity, std::string_view expected, std::size_t position) -> void
    {
        m_Entries.emplace_back(Entry{ severity, TranscodeUtf8ToWide(expected), position });
    }

    auto Sink::Verbose(std::string_view expected) -> void
    {
        AddEntry(Severity::Verbose, expected, 0);
    }

    auto Sink::Warning(std::string_view expected) -> void
    {
        AddEntry(Severity::Warning, expected, 0);
    }

    auto Sink::Error(std::string_view expected) -> void
    {
        AddEntry(Severity::Error, expected, 0);
    }

    auto Sink::AddEntry(Diagnostics::Severity severity, std::wstring_view expected, std::size_t position) -> void
    {
        m_Entries.emplace_back(Entry{ severity, std::wstring(expected), position });
    }

    auto Sink::Verbose(std::wstring_view expected) -> void
    {
        AddEntry(Severity::Verbose, expected, 0);
    }

    auto Sink::Warning(std::wstring_view expected) -> void
    {
        AddEntry(Severity::Warning, expected, 0);
    }

    auto Sink::Error(std::wstring_view expected) -> void
    {
        AddEntry(Severity::Error, expected, 0);
    }

    auto Sink::Report(std::vector<Entry> entries) -> void
    {
        m_Entries.clear();
        m_Entries.reserve(MaxCapacity + 128);
    }

    auto Sink::HasError() const -> bool
    {
        return std::any_of(m_Entries.begin(), m_Entries.end(), [](const Entry& entry) { return entry.Severity == Severity::Error; });
    }

    auto Sink::GetEntries() const -> const std::vector<Entry>&
    {
        return m_Entries;
    }

    auto Sink::Clear() -> void
    {
        m_Entries.clear();
    }

    auto Sink::DrainEntries() -> std::vector<Entry>
    {
        return std::exchange(m_Entries, {});
    }

} // namespace N503::Diagnostics
