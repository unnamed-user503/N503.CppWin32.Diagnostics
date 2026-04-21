#pragma once

#include <N503/Diagnostics/Severity.hpp>
#include <cstddef>
#include <string>
#include <string_view>

namespace N503::Diagnostics
{

    /// @brief 診断情報（エラーや警告）の個別のエントリを表す構造体です。
    struct Entry final
    {
        /// @brief 診断情報の重要度（Info, Warning, Error）。
        Diagnostics::Severity Severity{Severity::Verbose};

        /// @brief 期待されていた値や、発生した事象に関する説明文字列。
        std::string Expected{""};

        /// @brief 診断情報が発生したソースコード上の位置（オフセット）。
        std::size_t Position{0};

        /// @brief エントリの内容を、各Sink共通のフォーマット済み文字列に変換します。
        /// @return 統一された形式の診断メッセージ文字列。
        auto ToString() const -> std::string
        {
            /// @brief Severity列挙型を文字列に変換する内部ラムダ関数。
            auto SeverityToString = [](Diagnostics::Severity severity) -> std::string_view
            {
                switch (severity)
                {
                    case Severity::Verbose:
                        return "Verbose";
                    case Severity::Warning:
                        return "Warning";
                    case Severity::Error:
                        return "Error";
                    default:
                        return "Unknown";
                }
            };

            std::string result = "[";
            result += SeverityToString(Severity);

            if (0 < Position)
            {
                result += "(Line: ";
                result += std::to_string(Position);
                result += ")";
            }

            result += "] : ";
            result += Expected.empty() ? "(none)" : Expected;

            return result;
        }
    };

} // namespace N503::Diagnostics
