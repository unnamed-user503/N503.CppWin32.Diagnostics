#pragma once

#include <N503/Diagnostics/Entry.hpp>
#include <N503/Diagnostics/Severity.hpp>
#include <string_view>
#include <vector>

namespace N503::Diagnostics
{

    class Sink
    {
        static constexpr auto MaxCapacity = 256;

    public:
        Sink();

        virtual ~Sink() = default;

        virtual auto AddEntry(const Entry& entry) -> void;

        virtual auto AddEntry(Diagnostics::Severity severity, std::string_view expected, std::size_t position) -> void;

        virtual auto Verbose(std::string_view expected) -> void;

        virtual auto Warning(std::string_view expected) -> void;

        virtual auto Error(std::string_view expected) -> void;

        virtual auto AddEntry(Diagnostics::Severity severity, std::wstring_view expected, std::size_t position) -> void;

        virtual auto Verbose(std::wstring_view expected) -> void;

        virtual auto Warning(std::wstring_view expected) -> void;

        virtual auto Error(std::wstring_view expected) -> void;

        virtual auto Report(std::vector<Entry> entries) -> void;

        virtual auto HasError() const -> bool;

        virtual auto GetEntries() const -> const std::vector<Entry>&;

        virtual auto Clear() -> void;

        auto DrainEntries() -> std::vector<Entry>;

    private:
        std::vector<Entry> m_Entries;

        bool m_Overheated{ false };
    };

} // namespace N503::Diagnostics
