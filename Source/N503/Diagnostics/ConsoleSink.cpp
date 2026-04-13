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
        auto SeverityToLabel(Severity severity) -> std::string_view
        {
            switch (severity)
            {
            case Severity::Info:
                return "[INFO]";
            case Severity::Warning:
                return "[WARNING]";
            case Severity::Error:
                return "[ERROR]";
            default:
                return "[UNKNOWN]";
            }
        }
    } // namespace

    auto ConsoleSink::Report(std::vector<Entry> entries) -> void
    {
        // 1. 基底クラスのメモリ保持ロジックを呼び出す
        Sink::Report(entries);

        // 2. 標準出力へ整形して表示
        for (const auto& entry : entries)
        {
            std::ostream& out = (entry.Severity == Severity::Error) ? std::cerr : std::cout;

            out << SeverityToLabel(entry.Severity) << " "
                << "Pos: " << entry.Position << " | "
                << "Expected: " << (entry.Expected.empty() ? "(none)" : entry.Expected) << std::endl;
        }
    }

} // namespace N503::Diagnostics