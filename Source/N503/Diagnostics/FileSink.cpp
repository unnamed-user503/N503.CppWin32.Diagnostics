#include "Pch.hpp"
#include <N503/Diagnostics/Entry.hpp>
#include <N503/Diagnostics/FileSink.hpp>
#include <N503/Diagnostics/Sink.hpp>
#include <string>
#include <vector>

namespace N503::Diagnostics
{
    namespace
    {
        auto SeverityToString(Severity severity) -> std::string_view
        {
            switch (severity)
            {
            case Severity::Info:
                return "INFO";
            case Severity::Warning:
                return "WARNING";
            case Severity::Error:
                return "ERROR";
            default:
                return "UNKNOWN";
            }
        }
    } // namespace

    FileSink::FileSink(const std::string& path)
    {
        try
        {
            m_Ofs.open(path, std::ios::out | std::ios::app);
        }
        catch (...)
        {
            // opening may fail, but keep object usable
        }
    }

    FileSink::~FileSink()
    {
        try
        {
            if (m_Ofs.is_open())
            {
                m_Ofs.flush();
                m_Ofs.close();
            }
        }
        catch (...)
        {
        }
    }

    auto FileSink::Report(std::vector<Entry> entries) -> void
    {
        // 1. 基底クラスの Sink::m_Entries にも保存する
        Sink::Report(entries);

        // 2. ファイルへ書き出す
        if (!m_Ofs.is_open())
            return;

        try
        {
            std::lock_guard lock(m_FileMutex);
            for (const auto& entry : entries)
            {
                std::string line = "[" + std::string(SeverityToString(entry.Severity)) + "] ";
                line += "Position: " + std::to_string(entry.Position) + ", ";
                line += "Expected: '" + entry.Expected + "'\n";
                m_Ofs << line;
            }
            m_Ofs.flush();
        }
        catch (...)
        {
            // swallow
        }
    }

} // namespace N503::Diagnostics