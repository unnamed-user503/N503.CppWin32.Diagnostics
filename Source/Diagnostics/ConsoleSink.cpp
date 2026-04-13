#include "Pch.hpp"
#include <N503/Diagnostics/ConsoleSink.hpp>
#include <N503/Diagnostics/Entry.hpp>
#include <N503/Diagnostics/Severity.hpp>
#include <N503/Diagnostics/Sink.hpp>
#include <iostream>
#include <string_view>
#include <vector>

namespace N503::Diagnostics
{

    /// @param entries 出力対象の診断エントリのリスト。
    auto ConsoleSink::Report(std::vector<Entry> entries) -> void
    {
        /// @note 基底クラスのメモリ保持ロジックを呼び出し、エントリを蓄積します。
        Sink::Report(entries);

        for (const auto& entry : entries)
        {
            /// @note 重要度が Error の場合は標準エラー出力(std::cerr)を選択し、それ以外は標準出力(std::cout)を選択します。
            auto& outputStream = (entry.Severity == Diagnostics::Severity::Error) ? std::cerr : std::cout;

            /// @note Entry::ToString() を使用して、統一されたフォーマットでコンソールに出力します。
            outputStream << entry.ToString() << std::endl;
        }
    }

} // namespace N503::Diagnostics
