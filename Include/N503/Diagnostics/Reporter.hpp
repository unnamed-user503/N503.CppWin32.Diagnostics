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

    /// @brief 診断情報をバックグラウンドスレッドで非同期に処理し、各Sinkへ配信するクラスです。
    class Reporter final
    {
    public:
        /// @brief コンストラクタ。ワーカースレッドを起動します。
        Reporter();

        /// @brief デストラクタ。
        ~Reporter();

        /// @brief コピーコンストラクタ禁止。
        Reporter(const Reporter&) = delete;

        /// @brief コピー代入演算子禁止。
        auto operator=(const Reporter&) -> Reporter& = delete;

        /// @brief 診断情報の出力先となるSinkを追加します。
        /// @param sink 追加するSinkの共有ポインタ。
        void AddSink(std::shared_ptr<Sink> sink);

        /// @brief 指定されたSinkからエントリを回収し、非同期処理キューへ投入します。
        /// @param sink エントリの抽出元となるSink。
        void Submit(Sink& sink);

    private:
        /// @brief ワーカースレッドのメインループ処理です。
        /// @param stopToken スレッドの停止要求を検知するためのトークン。
        void Run(std::stop_token stopToken);

    private:
        /// @brief スレッドの待機と通知を制御する条件変数。
        std::condition_variable_any m_ConditionVariable;

        /// @brief メンバ変数へのアクセスを保護するためのミューテックス。
        std::mutex m_Mutex;

        /// @brief 処理を待機している診断エントリのキュー。
        std::vector<Entry> m_PendingEntries;

        /// @brief 登録されている出力先Sinkのリスト。
        std::vector<std::shared_ptr<Sink>> m_Sinks;

        /// @brief 新しいデータが準備できたことを示すフラグ。
        bool m_Ready = false;

        /// @brief 非同期処理を実行するスレッド。
        std::jthread m_Thread;
    };

} // namespace N503::Diagnostics
