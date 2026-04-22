#pragma once

#include <N503/Diagnostics/Entry.hpp>
#include <N503/Diagnostics/Severity.hpp>
#include <string_view>
#include <vector>

namespace N503::Diagnostics
{

    class Sink
    {
    public:
        virtual ~Sink() = default;

        virtual auto AddEntry(const Entry& entry) -> void;

        virtual auto AddEntry(Diagnostics::Severity severity, std::string_view expected, std::size_t position) -> void;

        virtual auto Report(std::vector<Entry> entries) -> void;

        virtual auto HasError() const -> bool;

        virtual auto GetEntries() const -> const std::vector<Entry>&;

        virtual auto Clear() -> void;

        auto DrainEntries() -> std::vector<Entry>;

    private:
        std::vector<Entry> m_Entries;
    };

} // namespace N503::Diagnostics
