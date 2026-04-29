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
        explicit FileSink(const std::wstring& path)
        {
            m_Handle = ::CreateFileW(
                path.data(),
                FILE_APPEND_DATA | SYNCHRONIZE, // 追記権限と同期フラグを指定
                FILE_SHARE_READ,
                nullptr,
                OPEN_ALWAYS,
                FILE_ATTRIBUTE_NORMAL,
                nullptr
            );
        }

        ~FileSink()
        {
            if (m_Handle != INVALID_HANDLE_VALUE)
            {
                ::CloseHandle(m_Handle);
            }
        }

        auto Report(const std::vector<Entry>& entries) -> void override
        {
            if (m_Handle == INVALID_HANDLE_VALUE)
            {
                return;
            }

            for (const auto& entry : entries)
            {
                auto message = FormatEntry(entry);

                DWORD bytesWritten = 0;
                ::WriteFile(m_Handle, message.data(), static_cast<DWORD>(message.size() * sizeof(wchar_t)), &bytesWritten, nullptr);
            }
        }

    private:
        HANDLE m_Handle{ INVALID_HANDLE_VALUE };
    };

} // namespace N503::Diagnostics
