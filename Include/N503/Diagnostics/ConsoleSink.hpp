#pragma once

// 1. Project Headers

// 2. Project Dependencies
#include <N503/Diagnostics/Entry.hpp>
#include <N503/Diagnostics/Sink.hpp>

// 3. WIL (Windows Implementation Library)

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
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
