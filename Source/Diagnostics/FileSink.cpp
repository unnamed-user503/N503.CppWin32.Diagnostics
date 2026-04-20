#include "Pch.hpp"
#include <N503/Diagnostics/Entry.hpp>
#include <N503/Diagnostics/FileSink.hpp>
#include <N503/Diagnostics/Severity.hpp>
#include <N503/Diagnostics/Sink.hpp>
#include <Windows.h>
#include <string>
#include <string_view>
#include <vector>
#include <wil/result_macros.h>

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
    } // namespace

    /// @param path 出力先ファイルパス。
    FileSink::FileSink(const std::string& path)
    {
        /// @note 書き込み(FILE_APPEND_DATA)と同期(SYNCHRONIZE)権限を指定し、ファイルを開くか新規作成します。
        m_Handle.reset(::CreateFileW(TranscodeUtf8ToWide(path).c_str(),
                                     FILE_APPEND_DATA | SYNCHRONIZE, // 追記権限と同期フラグを指定
                                     FILE_SHARE_READ, nullptr, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr));

        /// @note ハンドルの取得に失敗した場合は例外をスローします。
        THROW_LAST_ERROR_IF(!m_Handle);
    }

    /// @param entries 書き込み対象の診断エントリのリスト。
    auto FileSink::Report(std::vector<Entry> entries) -> void
    {
        /// @note 基底クラスのメモリ保持ロジックを呼び出し、エントリを蓄積します。
        Sink::Report(entries);

        for (const auto& entry : entries)
        {
            /// @note Entry::ToString() を使用して、ConsoleSinkと共通のフォーマットで文字列を生成します。
            /// @note Windows標準の改行コード(CRLF)を付加します。
            auto line = entry.ToString() + "\r\n";

            DWORD bytesWritten = 0;
            /// @note ファイルハンドルの現在位置（末尾）にデータを書き込みます。
            ::WriteFile(m_Handle.get(), line.data(), static_cast<DWORD>(line.size()), &bytesWritten, nullptr);
        }
    }
} // namespace N503::Diagnostics
