#pragma once

#include <N503/Diagnostics/Entry.hpp>
#include <N503/Diagnostics/Sink.hpp>

#include <fstream>
#include <mutex>
#include <string>
#include <vector>

#include <wil/resource.h>

#include <Windows.h>

namespace N503::Diagnostics
{

    class FileSink final : public Sink
    {
    public:
        explicit FileSink(const std::string& path);

        virtual ~FileSink() override;

        auto Report(std::vector<Entry> entries) -> void override;

    private:
        wil::unique_hfile m_Handle{};
    };

} // namespace N503::Diagnostics
