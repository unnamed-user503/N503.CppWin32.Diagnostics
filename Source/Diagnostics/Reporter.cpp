#include "Pch.hpp"
#include <N503/Diagnostics/Entry.hpp>
#include <N503/Diagnostics/Reporter.hpp>
#include <N503/Diagnostics/Sink.hpp>
#include <iterator>
#include <memory>
#include <mutex>
#include <stop_token>
#include <thread>
#include <utility>
#include <vector>

namespace N503::Diagnostics
{

    Reporter::Reporter()
    {
        m_Thread = std::jthread([this](std::stop_token stopToken) { Run(stopToken); });
    }

    Reporter::~Reporter()
    {
        {
            std::lock_guard lock(m_Mutex);
            m_Ready = true;
        }
        m_ConditionVariable.notify_all();
    }

    auto Reporter::AddSink(std::shared_ptr<Sink> sink) -> void
    {
        std::lock_guard lock(m_Mutex);
        m_Sinks.push_back(std::move(sink));
    }

    auto Reporter::Submit(Sink& sink) -> void
    {
        {
            std::lock_guard lock(m_Mutex);

            auto newEntries = sink.DrainEntries();
            m_PendingEntries.insert(m_PendingEntries.end(), std::make_move_iterator(newEntries.begin()), std::make_move_iterator(newEntries.end()));

            m_Ready = true;
        }
        m_ConditionVariable.notify_one();
    }

    auto Reporter::Stop() -> void
    {
        m_Thread.request_stop();
    }

    auto Reporter::Wait() -> void
    {
        if (m_Thread.joinable())
        {
            m_Thread.join();
        }
    }

    auto Reporter::Run(std::stop_token stopToken) -> void
    {
        while (true)
        {
            std::vector<Entry> workingEntries;
            std::vector<std::shared_ptr<Sink>> targetSinks;

            {
                std::unique_lock lock(m_Mutex);

                m_ConditionVariable.wait(lock, [&] { return m_Ready || stopToken.stop_requested(); });

                if (stopToken.stop_requested() && m_PendingEntries.empty())
                {
                    break;
                }

                workingEntries = std::move(m_PendingEntries);
                targetSinks = m_Sinks;
                m_Ready = false;
            }

            if (workingEntries.empty())
            {
                continue;
            }

            for (const auto& sink : targetSinks)
            {
                try
                {
                    sink->Report(workingEntries);
                }
                catch (...)
                {
                    /// @note 特定のSinkで発生した例外が、他のSinkへの配信やシステム全体を停止させないよう保護します。
                }
            }
        }
    }

} // namespace N503::Diagnostics
