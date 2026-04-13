#include "Pch.hpp"
#include <N503/Diagnostics/Entry.hpp>
#include <N503/Diagnostics/Reporter.hpp>
#include <N503/Diagnostics/Sink.hpp>
#include <chrono>
#include <fstream>
#include <iostream>
#include <mutex>
#include <stop_token>
#include <thread>
#include <utility>
#include <vector>

namespace N503::Diagnostics
{
    Reporter::Reporter(std::chrono::milliseconds /*idleInterval*/)
    {
        // try to open diagnostics.log for append; if it fails, keep running without file logging
        try
        {
            m_LogOfs.open("diagnostics.log", std::ios::out | std::ios::app);
            m_LogEnabled = m_LogOfs.is_open();
        }
        catch (...)
        {
            m_LogEnabled = false;
        }

        // jthread will call Run with its stop_token
        m_Thread = std::jthread(
            [this](std::stop_token stopToken)
            {
                Run(stopToken);
            });
    }

    Reporter::~Reporter()
    {
        // jthread destructor requests stop and joins automatically.
        // flush remaining entries if any
        std::vector<Entry> remaining;
        {
            std::lock_guard lock(m_Mutex);
            if (!m_PendingEntries.empty())
            {
                remaining = std::move(m_PendingEntries);
                m_PendingEntries.clear();
            }
        }

        if (!remaining.empty())
        {
            // Write remaining synchronously
            for (const auto& e : remaining)
            {
                try
                {
                    const char* sev = (e.Severity == Severity::Error) ? "ERROR" : (e.Severity == Severity::Warning) ? "WARNING" : "INFO";
                    std::ostream& out = (e.Severity == Severity::Error) ? std::cerr : std::cout;
                    out << "[" << sev << "] pos=" << e.Position << " : " << e.Expected << std::endl;

                    if (m_LogEnabled)
                    {
                        m_LogOfs << "[" << sev << "] pos=" << e.Position << " : " << e.Expected << "\r\n";
                        m_LogOfs.flush();
                    }
                }
                catch (...)
                {
                    // swallow
                }
            }
        }

        if (m_LogEnabled)
        {
            try
            {
                m_LogOfs.flush();
                m_LogOfs.close();
            }
            catch (...)
            {
            }
            m_LogEnabled = false;
        }
    }

    void Reporter::AddEntry(const Entry& entry)
    {
        std::lock_guard lock(m_Mutex);
        m_PendingEntries.push_back(entry);
        m_Ready = true;
        m_ConditionVariable.notify_one();
    }

    void Reporter::AddEntry(Severity severity, std::string_view expected, std::size_t position)
    {
        Entry e;
        e.Severity = severity;
        e.Position = position;
        e.Expected.assign(expected.begin(), expected.end());
        AddEntry(e);
    }

    void Reporter::Submit(Sink& sink)
    {
        // Drain from the Sink (move) and append to pending entries
        try
        {
            auto newEntries = sink.DrainEntries(); // thread-safe on sink side
            if (!newEntries.empty())
            {
                std::lock_guard lock(m_Mutex);
                // move elements into m_PendingEntries
                m_PendingEntries.insert(m_PendingEntries.end(), std::make_move_iterator(newEntries.begin()), std::make_move_iterator(newEntries.end()));
                m_Ready = true;
                m_ConditionVariable.notify_one();
            }
        }
        catch (...)
        {
            // swallow any errors from Drain
        }
    }

    auto Reporter::GetEntries() const -> std::vector<Entry>
    {
        std::lock_guard lock(m_Mutex);
        return m_PendingEntries;
    }

    void Reporter::Run(std::stop_token stopToken)
    {
        while (!stopToken.stop_requested())
        {
            std::vector<Entry> working;
            {
                std::unique_lock lock(m_Mutex);
                m_ConditionVariable.wait(lock,
                                         [&]
                                         {
                                             return m_Ready || stopToken.stop_requested();
                                         });

                if (stopToken.stop_requested())
                    break;

                // move pending to working and reset flag
                working = std::move(m_PendingEntries);
                m_PendingEntries.clear();
                m_Ready = false;
            }

            // Process working entries outside lock
            for (const auto& e : working)
            {
                try
                {
                    const char* sev = (e.Severity == Severity::Error) ? "ERROR" : (e.Severity == Severity::Warning) ? "WARNING" : "INFO";
                    std::ostream& out = (e.Severity == Severity::Error) ? std::cerr : std::cout;
                    out << "[" << sev << "] pos=" << e.Position << " : " << e.Expected << std::endl;

                    if (m_LogEnabled)
                    {
                        m_LogOfs << "[" << sev << "] pos=" << e.Position << " : " << e.Expected << "\r\n";
                    }
                }
                catch (...)
                {
                    // swallow per-entry exceptions to keep the worker running
                }
            }

            if (m_LogEnabled)
            {
                try
                {
                    m_LogOfs.flush();
                }
                catch (...)
                {
                }
            }
        } // while
    }

} // namespace N503::Diagnostics