#include "Pch.hpp"
#include <N503/Diagnostics/Entry.hpp>
#include <N503/Diagnostics/Sink.hpp>
#include <algorithm>
#include <iterator>
#include <utility>
#include <vector>

namespace N503::Diagnostics
{

    auto Sink::AddEntry(const Entry& entry) -> void
    {
        std::lock_guard lock(m_Mutex);
        m_Entries.push_back(entry);
    }

    auto Sink::Report(std::vector<Entry> entries) -> void
    {
        std::lock_guard lock(m_Mutex);
        m_Entries.insert(m_Entries.end(), std::make_move_iterator(entries.begin()), std::make_move_iterator(entries.end()));
    }

    auto Sink::HasError() const -> bool
    {
        std::lock_guard lock(m_Mutex);
        return std::any_of(m_Entries.begin(),
                           m_Entries.end(),
                           [](const Entry& entry)
                           {
                               return entry.Severity == Severity::Error;
                           });
    }

    auto Sink::GetEntries() const -> const std::vector<Entry>&
    {
        // Note: returning reference to internal vector. Caller must not modify it.
        return m_Entries;
    }

    auto Sink::DrainEntries() -> std::vector<Entry>
    {
        std::lock_guard lock(m_Mutex);
        return std::exchange(m_Entries, {});
    }

    auto Sink::Clear() -> void
    {
        std::lock_guard lock(m_Mutex);
        m_Entries.clear();
    }

} // namespace N503::Diagnostics