#pragma once

#include <N503/Diagnostics/Entry.hpp>
#include <N503/Diagnostics/Sink.hpp>
#include <condition_variable>
#include <memory>
#include <mutex>
#include <stop_token>
#include <thread>
#include <vector>

namespace N503::Diagnostics
{
    class Reporter final
    {
    public:
        Reporter();
        ~Reporter() = default;

        Reporter(const Reporter&) = delete;
        auto operator=(const Reporter&) -> Reporter& = delete;

        void AddSink(std::shared_ptr<Sink> sink);
        void Submit(Sink& sink);

    private:
        void Run(std::stop_token stopToken);

    private:
        std::jthread m_Thread;
        std::condition_variable_any m_ConditionVariable;
        std::mutex m_Mutex;
        std::vector<Entry> m_PendingEntries;
        std::vector<std::shared_ptr<Sink>> m_Sinks;
        bool m_Ready = false;
    };
} // namespace N503::Diagnostics
