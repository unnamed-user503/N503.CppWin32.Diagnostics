#include "Pch.hpp"
#include <N503/Diagnostics/ConsoleSink.hpp>
#include <N503/Diagnostics/Entry.hpp>
#include <iostream>
#include <string_view>
#include <utility>
#include <vector>

namespace N503::Diagnostics
{
    namespace
    {
        // Severity に応じたラベル文字列を返す
        auto SeverityToLabel(Severity severity) -> std::string_view
        {
            switch (severity)
            {
            case Severity::Info:    return "[INFO]";
            case Severity::Warning: return "[WARNING]";
            case Severity::Error:   return "[ERROR]";
            default:                return "[UNKNOWN]";
            }
        }
    }

    auto ConsoleSink::Report(std::vector<Entry> entries) -> void
    {
        // 1. 基底クラスのメモリ保持ロジックを呼び出す
        Sink::Report(entries);

        // 2. 標準出力へ整形して表示
        for (const auto& entry : entries)
        {
            // エラーの場合は std::cerr、それ以外は std::cout に出力するなどの使い分けも可能
            std::ostream& out = (entry.Severity == Severity::Error) ? std::cerr : std::cout;

            out << SeverityToLabel(entry.Severity) << " "
                << "Pos: " << entry.Position << " | "
                << "Expected: " << (entry.Expected.empty() ? "(none)" : entry.Expected)
                << std::endl;
        }
    }

} // namespace N503::Diagnostics
