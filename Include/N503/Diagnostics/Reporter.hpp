#pragma once

// 1. Project Headers

// 2. Project Dependencies
#include <N503/Diagnostics/Entry.hpp>
#include <N503/Diagnostics/Sink.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
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
        ~Reporter();

        Reporter(const Reporter&) = delete;
        auto operator=(const Reporter&) -> Reporter& = delete;

    public:
        auto AddSink(std::shared_ptr<Sink> sink) -> void;

        auto Submit(Sink& sink) -> void;

        auto Stop() -> void;

        auto Wait() -> void;

    private:
        auto Run(std::stop_token stopToken) -> void;

    private:
        std::condition_variable_any m_ConditionVariable;

        std::mutex m_Mutex;

        std::vector<Entry> m_PendingEntries;

        std::vector<std::shared_ptr<Sink>> m_Sinks;

        bool m_Ready = false;

        std::jthread m_Thread;
    };

} // namespace N503::Diagnostics
