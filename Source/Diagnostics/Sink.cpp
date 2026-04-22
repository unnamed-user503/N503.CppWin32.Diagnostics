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

    auto Sink::AddEntry(const Entry& entry) -> void
    {
        m_Entries.push_back(entry);
    }

    auto Sink::AddEntry(Diagnostics::Severity severity, std::string_view expected, std::size_t position) -> void
    {
        m_Entries.emplace_back(Entry{ severity, std::string(expected), position });
    }

    auto Sink::Report(std::vector<Entry> entries) -> void
    {
        m_Entries.insert(m_Entries.end(), std::make_move_iterator(entries.begin()), std::make_move_iterator(entries.end()));
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
