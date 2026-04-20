#pragma once

#include <N503/Diagnostics/Entry.hpp>
#include <string_view>
#include <vector>

namespace N503::Diagnostics
{

    /// @brief 診断エントリを蓄積・管理するための基底クラスです。
    class Sink
    {
    public:
        /// @brief 仮想デストラクタ。
        virtual ~Sink() = default;

        /// @brief 単一の診断エントリを追加します。
        /// @param entry 追加する診断エントリ。
        virtual auto AddEntry(const Entry& entry) -> void;

        /// @brief 発生した事象に関する説明文字列のみ引数として受け取る(重大度はInfo固定)
        /// @param expected 診断エントリ。
        virtual auto AddEntry(std::string_view expected) -> void;

        /// @brief 複数の診断エントリを一括で報告（追加）します。
        /// @param entries 追加する診断エントリのリスト。
        virtual auto Report(std::vector<Entry> entries) -> void;

        /// @brief 蓄積されたエントリの中にエラーが含まれているか確認します。
        /// @return エラーがある場合は true、それ以外は false。
        virtual auto HasError() const -> bool;

        /// @brief 現在蓄積されているすべてのエントリを取得します。
        /// @return 診断エントリのリストへの定数参照。
        virtual auto GetEntries() const -> const std::vector<Entry>&;

        /// @brief 蓄積されたエントリをすべて取り出し（ムーブし）、内部の状態を空にします。
        /// @return 取り出された診断エントリのリスト。
        auto DrainEntries() -> std::vector<Entry>;

        /// @brief 蓄積されたエントリをすべて消去します。
        virtual auto Clear() -> void;

    private:
        /// @brief 蓄積された診断エントリを保持するコンテナ。
        std::vector<Entry> m_Entries;
    };

} // namespace N503::Diagnostics
