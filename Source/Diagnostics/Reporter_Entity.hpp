#pragma once

// 1. Project Headers

// 2. Project Dependencies
#include <N503/Diagnostics/Reporter.hpp>
#include <N503/Diagnostics/Sink.hpp>
#include <N503/Diagnostics/Types.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <mutex>
#include <thread>
#include <vector>
#include <condition_variable>

namespace N503::Diagnostics
{

    struct Reporter::Entity
    {
        std::vector<std::unique_ptr<Sink>> Sinks;

        std::vector<Diagnostics::Entry> Entries;

        std::condition_variable_any ConditionVariable;

        std::mutex Mutex;

        std::jthread WorkerThread;
    };

} // namespace N503::Diagnostics
