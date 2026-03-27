#pragma once

#include <N503/Diagnostics/Entry.hpp>
#include <N503/Diagnostics/Severity.hpp>
#include <algorithm>
#include <vector>

namespace N503::Diagnostics
{

    /// @class Sink
    /// @brief 診断情報（Entry）を収集し、最遠地点の情報を管理するクラスです。
    class Sink
    {
    public:
        /// @brief 新しい診断情報を報告します。
        /// @details 現在の最遠地点より遠い場合は既存の情報を破棄して更新し、同地点の場合は蓄積します。
        /// @param entry 報告する診断情報
        auto Report(const Entry& entry) -> void
        {
            if (entry.Position > m_FurthestPosition)
            {
                // より遠い位置でエラーが発生したため、古い（手前の）情報は破棄して更新
                m_Entries.clear();
                m_FurthestPosition = entry.Position;
                m_Entries.push_back(entry);
            }
            else if (entry.Position == m_FurthestPosition)
            {
                // 同じ最遠地点での別の可能性として蓄積
                m_Entries.push_back(entry);
            }
        }

        /// @brief エラーレベルの診断情報が含まれているか確認します。
        /// @return エラーが存在すればtrue、そうでなければfalse
        [[nodiscard]]
        auto HasError() const -> bool
        {
            auto condition = [](const Entry& entry)
            {
                return entry.Severity == Severity::Error;
            };

            return std::any_of(m_Entries.begin(), m_Entries.end(), condition);
        }

        /// @brief 収集された診断情報の一覧を取得します。
        /// @return 診断情報のリストへの参照
        [[nodiscard]]
        auto GetEntries() const -> const std::vector<Entry>&
        {
            return m_Entries;
        }

        /// @brief 内部状態をリセットし、蓄積された情報をすべて消去します。
        auto Clear() -> void
        {
            m_Entries.clear();
            m_FurthestPosition = 0;
        }

    private:
        /// @brief 蓄積された診断情報のリスト
        std::vector<Entry> m_Entries;

        /// @brief これまでに報告された中で最も遠い解析位置
        std::size_t m_FurthestPosition{ 0 };
    };

} // namespace N503::Diagnostics
