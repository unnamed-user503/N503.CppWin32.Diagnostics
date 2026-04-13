#include "Pch.hpp"
#include <N503/Diagnostics/Entry.hpp>
#include <N503/Diagnostics/Reporter.hpp>
#include <N503/Diagnostics/Sink.hpp>
#include <chrono>
#include <mutex>
#include <stop_token>
#include <thread>
#include <utility>
#include <vector>

namespace N503::Diagnostics
{
    Reporter::Reporter(std::chrono::milliseconds idleInterval)
    {
        // jthreadは開始時にstop_tokenを渡せます
        m_Thread = std::jthread([this](std::stop_token stopToken) {
            Run(stopToken);
        });
    }

    Reporter::~Reporter() = default; // jthreadが自動でjoin/stopをハンドルします

    void Reporter::Submit(Sink& sink)
    {
        {
            std::lock_guard lock(m_Mutex);
            // Sinkからエントリをごっそり奪う（Drain）
            auto newEntries = sink.DrainEntries();
            m_PendingEntries.insert(m_PendingEntries.end(), newEntries.begin(), newEntries.end());
            m_Ready = true;
        }
        m_ConditionVariable.notify_one();
    }

    void Reporter::Run(std::stop_token stopToken)
    {
        while (!stopToken.stop_requested())
        {
            std::vector<Entry> workingEntries;
            {
                std::unique_lock lock(m_Mutex);
                // データが来るか、停止要求があるまで待機
                m_ConditionVariable.wait(lock, [&] { 
                    return m_Ready || stopToken.stop_requested(); 
                });

                if (stopToken.stop_requested()) break;

                // 処理対象をローカルに移してロックを即解除
                workingEntries = std::move(m_PendingEntries);
                m_Ready = false;
            }

            // ここで時間のかかる出力処理などを行う（例: ロギング）
            for (const auto& entry : workingEntries)
            {
                // 必要に応じて、登録された各種 Sink に書き出す処理などをここに記述
            }
        }
    }
}
