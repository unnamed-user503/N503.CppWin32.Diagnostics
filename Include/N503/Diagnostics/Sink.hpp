#pragma once

#include <N503/Diagnostics/Entry.hpp>
#include <vector>

namespace N503::Diagnostics
{
    class Sink
    {
    public:
        virtual ~Sink() = default;
        virtual auto AddEntry(const Entry& entry) -> void;
        virtual auto Report(std::vector<Entry> entries) -> void;
        virtual auto HasError() const -> bool;
        virtual auto GetEntries() const -> const std::vector<Entry>&;
        auto DrainEntries() -> std::vector<Entry>;
        virtual auto Clear() -> void;

    private:
        std::vector<Entry> m_Entries;
    };
} // namespace N503::Diagnostics
