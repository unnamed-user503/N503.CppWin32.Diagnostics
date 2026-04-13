#pragma once

#include <N503/Diagnostics/Entry.hpp>
#include <N503/Diagnostics/Sink.hpp>

#include <mutex>
#include <vector>

namespace N503::Diagnostics
{
    class ConsoleSink final : public Sink
    {
    public:
        ConsoleSink() = default;

        virtual ~ConsoleSink() override = default;

        auto Report(std::vector<Entry> entries) -> void override;
    };

} // namespace N503::Diagnostics
