#pragma once

#include <N503/Diagnostics/Entry.hpp>
#include <N503/Diagnostics/Sink.hpp>
#include <Windows.h>
#include <string>
#include <vector>
#include <wil/resource.h>

namespace N503::Diagnostics
{
    /// @brief 診断情報をファイルに出力するためのSinkクラスです。
    class FileSink final : public Sink
    {
    public:
        /// @brief 出力先パスを指定してFileSinkを初期化します。
        /// @param path ログを出力するファイルのパス。
        explicit FileSink(const std::string& path);

        /// @brief デストラクタ。
        virtual ~FileSink() override = default;

        /// @brief 診断エントリのリストを受け取り、共通フォーマットでファイルへ追記します。
        /// @param entries 書き込み対象となる診断エントリのベクター。
        auto Report(std::vector<Entry> entries) -> void override;

    private:
        /// @brief ファイル操作に使用するWindowsファイルハンドルのスマートポインタ。
        wil::unique_hfile m_Handle{};
    };
} // namespace N503::Diagnostics
