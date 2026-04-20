#pragma once

#include <N503/Diagnostics/Entry.hpp>
#include <N503/Diagnostics/Sink.hpp>
#include <vector>

namespace N503::Diagnostics
{

    /// @brief 診断情報を標準出力および標準エラー出力へ表示するためのSinkクラスです。
    class ConsoleSink final : public Sink
    {
    public:
        /// @brief コンストラクタ。
        ConsoleSink() = default;

        /// @brief 仮想デストラクタ。
        virtual ~ConsoleSink() override = default;

        /// @brief 受信した診断エントリのリストを、共通フォーマットを使用してコンソールに出力します。
        /// @param entries 表示対象となる診断エントリのベクター。
        /// @return なし。
        auto Report(std::vector<Entry> entries) -> void override;
    };

} // namespace N503::Diagnostics
