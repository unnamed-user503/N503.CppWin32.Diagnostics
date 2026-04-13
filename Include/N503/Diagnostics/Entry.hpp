#pragma once

#include <N503/Diagnostics/Severity.hpp>
#include <cstddef>
#include <string_view>
#include <string>

namespace N503::Diagnostics
{

    /// @struct Entry
    /// @brief 診断情報の個別の記録を保持する構造体です。
    struct Entry final
    {
        /// @brief 診断情報の重要度（情報の種類）。
        ::N503::Diagnostics::Severity Severity;

        /// @brief エラーまたは情報が発生したソース内の位置（オフセット）。
        std::size_t Position;

        /// @brief その位置で期待されていたトークンや要素の説明。
        std::string Expected;
    };

} // namespace N503::Diagnostics
