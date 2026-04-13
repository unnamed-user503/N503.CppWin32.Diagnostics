#pragma once

namespace N503::Diagnostics
{

    /// @enum Severity
    /// @brief 診断情報の重要度（深刻度）を定義する列挙型です。
    enum class Severity
    {
        Info,    ///< 情報：エラーや警告ではない、補足的な通知。
        Warning, ///< 警告：実行は可能ですが、注意が必要な状態。
        Error,   ///< エラー：処理の継続が困難な致命的な状態。
    };

} // namespace N503::Diagnostics
