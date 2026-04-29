#include "Pch.hpp"
#include "Reporter_Entity.hpp"

// 1. Project Headers

// 2. Project Dependencies
#include <N503/Diagnostics/Reporter.hpp>
#include <N503/Diagnostics/Sink.hpp>
#include <N503/Diagnostics/Types.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <memory>
#include <mutex>

namespace N503::Diagnostics
{
    namespace
    {
        auto TranscodeUtf8ToWide(const std::string_view utf8) -> std::wstring
        {
            if (utf8.empty())
            {
                return {};
            }

            int desired = ::MultiByteToWideChar(CP_UTF8, 0, utf8.data(), -1, nullptr, 0);
            if (desired == 0)
            {
                return {};
            }

            std::wstring result(desired, 0);
            ::MultiByteToWideChar(CP_UTF8, 0, utf8.data(), -1, &result[0], desired);

            result.resize(desired - 1);
            return result;
        }
    } // namespace

    Reporter::Reporter() : m_Entity(std::make_unique<Entity>())
    {
        m_Entity->WorkerThread = std::jthread(
            [this](const std::stop_token stopToken)
            {
                while (!stopToken.stop_requested())
                {
                    std::vector<Diagnostics::Entry> entriesToReport;
                    {
                        // ログが届くか停止を要求されるまで待機 (CPU負荷 0%)
                        std::unique_lock lock(m_Entity->Mutex);
                        m_Entity->ConditionVariable.wait(lock, stopToken, [this] { return !m_Entity->Entries.empty(); });

                        if (stopToken.stop_requested() && m_Entity->Entries.empty())
                        {
                            break;
                        }

                        // バッファをスワップして、ロックを即座に抜ける
                        // entriesToReport へ move することで、ロック時間を最小化
                        entriesToReport = std::move(m_Entity->Entries);
                        m_Entity->Entries.clear();
                    }

                    // ロックの外側で、登録されたすべてのSinkに対して一括レポートを実行
                    // ここで重い I/O が発生しても Submit を妨げない
                    if (!entriesToReport.empty())
                    {
                        for (const auto& sink : m_Entity->Sinks)
                        {
                            sink->Report(entriesToReport);
                        }
                    }
                }
            }
        );
    }

    Reporter::~Reporter()
    {
        m_Entity->WorkerThread.request_stop();

        if (m_Entity->WorkerThread.joinable())
        {
            m_Entity->WorkerThread.join();
        }
    }

    auto Reporter::AddSink(std::unique_ptr<Sink> sink) -> void
    {
        m_Entity->Sinks.push_back(std::move(sink));
    }

    auto Reporter::Report() -> void
    {
        m_Entity->ConditionVariable.notify_one();
    }

    auto Reporter::Submit(Diagnostics::Entry&& entry) -> void
    {
        std::scoped_lock lock(m_Entity->Mutex);
        m_Entity->Entries.push_back(std::move(entry));
    }

    auto Reporter::Verbose(std::string_view expected, std::size_t position) -> void
    {
        Submit({ Diagnostics::Severity::Verbose, TranscodeUtf8ToWide(expected), position });
    }

    auto Reporter::Warning(std::string_view expected, std::size_t position) -> void
    {
        Submit({ Diagnostics::Severity::Warning, TranscodeUtf8ToWide(expected), position });
    }

    auto Reporter::Error(std::string_view expected, std::size_t position) -> void
    {
        Submit({ Diagnostics::Severity::Error, TranscodeUtf8ToWide(expected), position });
    }

    Reporter::Reporter(Reporter&&) noexcept = default;

    auto Reporter::operator=(Reporter&&) noexcept -> Reporter& = default;

} // namespace N503::Diagnostics
