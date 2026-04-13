#include "Pch.hpp"
#include <N503/Diagnostics/DebugConsoleSink.hpp>
#include <N503/Diagnostics/Entry.hpp>
#include <N503/Diagnostics/Severity.hpp>
#include <N503/Diagnostics/Sink.hpp>
#include <Windows.h>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

namespace N503::Diagnostics
{
    
    namespace
    {
        /// @brief UTF-8エンコードされた文字列をWindows API（WideChar）形式の文字列に変換します。
        /// @param utf8 変換元のUTF-8文字列。
        /// @return 変換後のWide文字列。失敗した場合は空の文字列を返します。
        auto TranscodeUtf8ToWide(const std::string_view& utf8) -> std::wstring
        {
            if (utf8.empty())
            {
                return {};
            }

            int desired = ::MultiByteToWideChar(CP_UTF8, 0, utf8.data(), -1, nullptr, 0);
            if (desired == 0)
            {
                return {};
            }

            std::wstring result(desired, 0);
            ::MultiByteToWideChar(CP_UTF8, 0, utf8.data(), -1, &result[0], desired);

            result.resize(desired - 1);
            return result;
        }
    }

    /// @param entries 出力対象の診断エントリのリスト。
    auto DebugConsoleSink::Report(std::vector<Entry> entries) -> void
    {
        /// @note 基底クラスのメモリ保持ロジックを呼び出し、エントリを蓄積します。
        Sink::Report(entries);

        for (const auto& entry : entries)
        {
            /// @note Entry::ToString() を使用して、統一されたフォーマットでデバッグコンソールに出力します。
            ::OutputDebugStringW(TranscodeUtf8ToWide(entry.ToString() + "\r\n").data());
        }
    }

} // namespace N503::Diagnostics
