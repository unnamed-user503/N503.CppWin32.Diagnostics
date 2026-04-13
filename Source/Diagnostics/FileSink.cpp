#include "Pch.hpp"
#include <N503/Diagnostics/Entry.hpp>
#include <N503/Diagnostics/FileSink.hpp>
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
        // UTF-8文字列をWide文字列（Windows用）に変換するヘルパー
        auto TranscodeUtf8ToWide(const std::string_view& utf8) -> std::wstring
        {
            if (utf8.empty()) return {};
            int desired = ::MultiByteToWideChar(CP_UTF8, 0, utf8.data(), static_cast<int>(utf8.length()), nullptr, 0);
            if (desired == 0) return {};

            std::wstring result(desired, 0);
            ::MultiByteToWideChar(CP_UTF8, 0, utf8.data(), static_cast<int>(utf8.length()), &result[0], desired);
            return result;
        }

        // Severity 列挙型を文字列に変換するヘルパー
        auto SeverityToString(Severity severity) -> std::string_view
        {
            switch (severity)
            {
            case Severity::Info:    return "INFO";
            case Severity::Warning: return "WARNING";
            case Severity::Error:   return "ERROR";
            default:                return "UNKNOWN";
            }
        }
    }

    FileSink::FileSink(const std::string& path)
    {
        // ファイルを開く（末尾に追記する設定）
        m_Handle.reset(::CreateFileW(
            TranscodeUtf8ToWide(path).c_str(),
            FILE_APPEND_DATA,      // 追記モード
            FILE_SHARE_READ,
            nullptr,
            OPEN_ALWAYS,           // なければ作成、あれば開く
            FILE_ATTRIBUTE_NORMAL,
            nullptr));

        THROW_LAST_ERROR_IF(!m_Handle);
    }

    FileSink::~FileSink() = default;

    auto FileSink::Report(std::vector<Entry> entries) -> void
    {
        // 1. 基底クラスの Sink::m_Entries にも保存する
        Sink::Report(entries);

        // 2. ファイルへ書き出す
        for (const auto& entry : entries)
        {
            // 出力フォーマットの整形: [SEVERITY] Position: 123, Expected: 'something'\n
            std::string line = "[" + std::string(SeverityToString(entry.Severity)) + "] ";
            line += "Position: " + std::to_string(entry.Position) + ", ";
            line += "Expected: '" + entry.Expected + "'\r\n"; // Windowsの改行コード

            DWORD bytesWritten = 0;
            ::WriteFile(m_Handle.get(), line.data(), static_cast<DWORD>(line.size()), &bytesWritten, nullptr);
        }
    }

} // namespace N503::Diagnostics
