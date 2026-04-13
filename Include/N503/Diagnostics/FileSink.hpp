#pragma once

#include <N503/Diagnostics/Entry.hpp>
#include <N503/Diagnostics/Sink.hpp>
#include <Windows.h>
#include <string>
#include <vector>
#include <wil/resource.h>

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
