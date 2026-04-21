#include "Pch.hpp"
#include <N503/Diagnostics/Entry.hpp>
#include <N503/Diagnostics/Severity.hpp>
#include <N503/Diagnostics/Sink.hpp>
#include <algorithm>
#include <iterator>
#include <string_view>
#include <utility>
#include <vector>

namespace N503::Diagnostics
{

    /// @param entry 追加する診断エントリ。
    auto Sink::AddEntry(const Entry &entry) -> void
    {
        m_Entries.push_back(entry);
    }

    /// @brief 発生した事象に関する説明文字列のみ引数として受け取る
    auto Sink::AddEntry(std::string_view expected) -> void
    {
        AddEntry({Severity::Verbose, expected.data(), 0});
    }

    /// @param entries 追加（報告）する診断エントリのリスト。
    auto Sink::Report(std::vector<Entry> entries) -> void
    {
        /// @note 引数で渡されたエントリをムーブイテレータを使用して内部ストレージの末尾に追加します。
        m_Entries
            .insert(m_Entries.end(), std::make_move_iterator(entries.begin()), std::make_move_iterator(entries.end()));
    }

    /// @return エラーが含まれている場合は true、それ以外は false。
    auto Sink::HasError() const -> bool
    {
        /// @note 蓄積されたエントリの中に一つでも Severity::Error があるか走査します。
        return std::any_of(
            m_Entries.begin(), m_Entries.end(), [](const Entry &entry) { return entry.Severity == Severity::Error; }
        );
    }

    /// @return 内部で保持しているエントリリストへの定数参照。
    auto Sink::GetEntries() const -> const std::vector<Entry> &
    {
        return m_Entries;
    }

    /// @return 取り出された診断エントリのリスト。
    auto Sink::DrainEntries() -> std::vector<Entry>
    {
        /// @note std::exchange を使用して内部コンテナを空にしつつ、以前の中身を返します。
        return std::exchange(m_Entries, {});
    }

    /// @brief 内部に保持されているエントリをすべて削除します。
    auto Sink::Clear() -> void
    {
        m_Entries.clear();
    }

} // namespace N503::Diagnostics
