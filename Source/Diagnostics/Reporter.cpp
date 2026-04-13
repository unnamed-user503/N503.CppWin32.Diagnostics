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
        /// @note ワーカースレッドを起動し、Runメソッドにて非同期処理を開始します。
        /// @note jthreadを使用しているため、stop_tokenが自動的に渡されます。
        m_Thread = std::jthread(
            [this](std::stop_token stopToken)
            {
                Run(stopToken);
            });
    }

    /// @param sink 登録する出力先Sinkの共有ポインタ。
    void Reporter::AddSink(std::shared_ptr<Sink> sink)
    {
        /// @note Sinkリストの更新をスレッドセーフに行います。
        std::lock_guard lock(m_Mutex);
        m_Sinks.push_back(std::move(sink));
    }

    /// @param sink 診断情報が蓄積されたソースSink。
    void Reporter::Submit(Sink& sink)
    {
        {
            /// @note キュー（m_PendingEntries）へのデータ追加を保護します。
            std::lock_guard lock(m_Mutex);
            
            /// @note 引数のSinkからエントリをすべて奪い、ムーブイテレータを用いて効率的に転送します。
            auto newEntries = sink.DrainEntries();
            m_PendingEntries.insert(m_PendingEntries.end(), 
                                    std::make_move_iterator(newEntries.begin()), 
                                    std::make_move_iterator(newEntries.end()));
            
            /// @note データ準備完了フラグをセットします。
            m_Ready = true;
        }
        /// @note 待機中のワーカースレッドに処理の開始を通知します。
        m_ConditionVariable.notify_one();
    }

    /// @param stopToken スレッド停止要求を監視するためのトークン。
    void Reporter::Run(std::stop_token stopToken)
    {
        while (true)
        {
            std::vector<Entry> workingEntries;
            std::vector<std::shared_ptr<Sink>> targetSinks;

            {
                std::unique_lock lock(m_Mutex);
                
                /// @note 通知があるか停止要求が来るまでスレッドをブロックして待機します。
                m_ConditionVariable.wait(lock,
                                         stopToken,
                                         [&]
                                         {
                                             return m_Ready || stopToken.stop_requested();
                                         });

                /// @note 停止要求があり、かつ処理すべき残データがない場合にループを抜けます。
                if (stopToken.stop_requested() && m_PendingEntries.empty())
                    break;

                /// @note 処理対象のエントリと現在のSinkリストをローカルにコピー/ムーブし、ロック時間を最小化します。
                workingEntries = std::move(m_PendingEntries);
                targetSinks = m_Sinks;
                m_Ready = false;
            }

            /// @note 処理すべきデータがない場合は次の待機へ移ります。
            if (workingEntries.empty())
                continue;

            /// @note 登録されているすべてのSinkに対して、収集したエントリを配信します。
            for (const auto& sink : targetSinks)
            {
                try
                {
                    /// @note 各SinkのReportを呼び出します。
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
