#pragma once

#include <N503/Diagnostics/Entry.hpp>
#include <N503/Diagnostics/Sink.hpp>
#include <N503/Diagnostics/Severity.hpp>

#include <atomic>
#include <chrono>
#include <condition_variable>
#include <deque>
#include <mutex>
#include <string_view>
#include <strsafe.h>
#include <thread>
#include <vector>
#include <stop_token>

namespace N503::Diagnostics
{

    class Reporter final
    {
    public:
        explicit Reporter(std::chrono::milliseconds idleInterval = std::chrono::milliseconds{ 200 });

        ~Reporter();

        Reporter(const Reporter&) = delete;

        auto operator=(const Reporter&) -> Reporter& = delete;

    public:
        auto AddEntry(const Entry& entry) -> void;

        auto AddEntry(Severity severity, std::string_view expected, std::size_t position = 0) -> void;

        auto Submit(Sink& sink) -> void;

        auto GetEntries() const -> std::vector<Entry>;

private:
    void Run(std::stop_token stopToken); // stop_tokenを受け取るように変更

    std::jthread m_Thread;
    std::condition_variable m_ConditionVariable;
    std::mutex m_Mutex;                 // 追加：データの保護用
    std::vector<Entry> m_PendingEntries; // 追加：処理待ちのキュー
    bool m_Ready = false;               // 追加：通知用フラグ
    };

} // namespace N503::Diagnostics
