#pragma once

// 1. Project Headers

// 2. Project Dependencies
#include <N503/Diagnostics/Types.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers
#include <Windows.h>

// 6. C++ Standard Libraries
#include <format>
#include <iostream>
#include <string>
#include <filesystem>
#include <fstream>
#include <vector>

namespace N503::Diagnostics
{

    // -----------------------------------
    // Sinkは、診断エントリを受け取り、適切な形式で出力するための抽象クラスです。具体的な出力方法は、ConsoleSink、DebugStringSink、FileSinkなどの派生クラスで実装されます。
    // -----------------------------------
    class Sink
    {
    public:
        virtual ~Sink() = default;

        virtual auto Report(const std::vector<Entry>& entries) -> void = 0;

        virtual auto FormatEntry(const Entry& entry) -> std::wstring
        {
            switch (entry.Severity)
            {
                case Severity::Verbose:
                    return std::format(L"[Verbose]{}({})", entry.Expected, entry.Position);

                case Severity::Warning:
                    return std::format(L"[Warning]{}({})", entry.Expected, entry.Position);

                case Severity::Error:
                    return std::format(L"[Error]{}({})", entry.Expected, entry.Position);
            }

            return {};
        }
    };

    // -----------------------------------
    // ConsoleSinkは、診断エントリを標準出力（コンソール）に送信するためのシンクです。エントリの重大度に応じて、標準出力または標準エラー出力に書き込みます。
    // -----------------------------------
    class ConsoleSink : public Sink
    {
    public:
        auto Report(const std::vector<Entry>& entries) -> void override
        {
            for (const auto& entry : entries)
            {
                auto& out = (entry.Severity == Severity::Error) ? std::wcerr : std::wcout;
                out << FormatEntry(entry) << std::endl;
            }
        }
    };

    // -----------------------------------
    // DebugStringSinkは、診断エントリをWindowsのデバッグ出力に送信するためのシンクです。Visual Studioの出力ウィンドウなどで確認できます。
    // -----------------------------------
    class DebugStringSink : public Sink
    {
    public:
        auto Report(const std::vector<Entry>& entries) -> void override
        {
            for (const auto& entry : entries)
            {
                auto message = FormatEntry(entry) + L"\r\n";
                ::OutputDebugStringW(message.data());
            }
        }
    };

    // -----------------------------------
    // FileSinkは、指定されたファイルに診断エントリを書き込むためのシンクです。ファイルは追記モードで開かれ、既存の内容を保持しながら新しいエントリが追加されます。
    // -----------------------------------
    class FileSink : public Sink
    {
    public:
        explicit FileSink(const std::filesystem::path& path)
        {
            // std::ios::app で追記モードを指定
            m_FileStream.open(path, std::ios::out | std::ios::app);
        }

        virtual ~FileSink() override = default;

        auto Report(const std::vector<Entry>& entries) -> void override
        {
            if (!m_FileStream.is_open())
            {
                return;
            }

            for (const auto& entry : entries)
            {
                // wofstream を使用してワイド文字列を直接書き込み
                m_FileStream << FormatEntry(entry) << std::endl;
            }
            
            // バッファを確実にフラッシュ
            m_FileStream.flush();
        }

    private:
        std::wofstream m_FileStream;
    };

} // namespace N503::Diagnostics
