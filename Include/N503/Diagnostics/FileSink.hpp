#pragma once
// 1. Project Headers

// 2. Project Dependencies
#include <N503/Diagnostics/Entry.hpp>
#include <N503/Diagnostics/Sink.hpp>

// 3. WIL (Windows Implementation Library)
#include <wil/resource.h>

// 4. Third-party Libraries

// 5. Windows Headers

// 6. C++ Standard Libraries
#include <string>
#include <vector>

namespace N503::Diagnostics
{

    class FileSink final : public Sink
    {
    public:
        explicit FileSink(const std::string& path);

        virtual ~FileSink() override = default;

        auto Report(std::vector<Entry> entries) -> void override;

    private:
        wil::unique_hfile m_Handle{};
    };

} // namespace N503::Diagnostics
