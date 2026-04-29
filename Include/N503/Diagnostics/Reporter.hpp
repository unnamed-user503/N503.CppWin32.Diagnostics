#pragma once

// 1. Project Headers

// 2. Project Dependencies
#include <N503/Abi/Api.hpp>
#include <N503/Diagnostics/Sink.hpp>
#include <N503/Diagnostics/Types.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries

namespace N503::Diagnostics
{

    /// @brief
    class N503_API Reporter final
    {
    public:
        /// @brief
        /// @param title
        /// @param width
        /// @param height
        Reporter();

        /// @brief
        ~Reporter();

        /// @brief
        /// @param
        Reporter(const Reporter&) = delete;

        /// @brief
        /// @param
        /// @return
        auto operator=(const Reporter&) -> Reporter& = delete;

        /// @brief
        /// @param
        Reporter(Reporter&&) noexcept;

        /// @brief
        /// @param
        /// @return
        auto operator=(Reporter&&) noexcept -> Reporter&;

    public:
        /// @brief
        /// @return
        auto AddSink(std::unique_ptr<Sink> sink) -> void;

        /// @brief
        /// @return
        auto Report() -> void;

        /// @brief
        /// @return
        auto Submit(Diagnostics::Entry&& entry) -> void;

        /// @brief
        /// @param expected
        /// @param position
        /// @return
        auto Verbose(std::wstring_view expected, std::size_t position = 0) -> void
        {
            Submit({ Diagnostics::Severity::Verbose, std::wstring(expected), position });
        }

        /// @brief
        /// @param expected
        /// @param position
        /// @return
        auto Warning(std::wstring_view expected, std::size_t position = 0) -> void
        {
            Submit({ Diagnostics::Severity::Warning, std::wstring(expected), position });
        }

        /// @brief
        /// @param expected
        /// @param position
        /// @return
        auto Error(std::wstring_view expected, std::size_t position = 0) -> void
        {
            Submit({ Diagnostics::Severity::Error, std::wstring(expected), position });
        }

        /// @brief
        /// @param expected
        /// @param position
        /// @return
        auto Verbose(std::string_view expected, std::size_t position = 0) -> void;

        /// @brief
        /// @param expected
        /// @param position
        /// @return
        auto Warning(std::string_view expected, std::size_t position = 0) -> void;

        /// @brief
        /// @param expected
        /// @param position
        /// @return
        auto Error(std::string_view expected, std::size_t position = 0) -> void;

    public:
        /// @brief 実装の詳細を隠蔽するための不透明な構造体。
        struct Entity;

#ifdef N503_DLL_EXPORTS
        /// @brief 内部の実装実体（Entity）への参照を取得します。
        /// @note このメソッドはライブラリ内部（DLL境界の内側）でのみ使用されます。
        /// @return Entity を管理する unique_ptr への参照。
        [[nodiscard]]
        auto GetEntity() -> std::unique_ptr<Entity>&
        {
            return m_Entity;
        }
#endif

    private:
#pragma warning(push)
#pragma warning(disable : 4251) // DLL境界を越える際の unique_ptr に関する警告を抑制
        /// @brief
        std::unique_ptr<Entity> m_Entity;
#pragma warning(pop)
    };

} // namespace N503::Diagnostics
