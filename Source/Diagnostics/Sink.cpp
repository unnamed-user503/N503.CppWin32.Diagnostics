#include "Pch.hpp"
#include <N503/Diagnostics/Entry.hpp>
#include <N503/Diagnostics/Severity.hpp>
#include <N503/Diagnostics/Sink.hpp>
#include <algorithm>
#include <iterator>
#include <utility>
#include <vector>

namespace N503::Diagnostics
{

    auto Sink::AddEntry(const Entry& entry) -> void
    {
        m_Entries.push_back(entry);
    }

    auto Sink::Report(std::vector<Entry> entries) -> void
    {
        // 既存のエントリの後ろに新しいエントリ群を追加
        m_Entries.insert(
            m_Entries.end(),
            std::make_move_iterator(entries.begin()),
            std::make_move_iterator(entries.end())
        );
    }

    auto Sink::HasError() const -> bool
    {
        // エントリの中に Severity::Error が一つでも含まれているか確認
        return std::any_of(m_Entries.begin(), m_Entries.end(), [](const Entry& entry)
        {
            return entry.Severity == Severity::Error;
        });
    }

    auto Sink::GetEntries() const -> const std::vector<Entry>&
    {
        return m_Entries;
    }

    auto Sink::DrainEntries() -> std::vector<Entry>
    {
        return std::exchange(m_Entries, {});
    }

    auto Sink::Clear() -> void
    {
        m_Entries.clear();
    }

} // namespace N503::Diagnostics
