#include <chrono>
#include <iostream>
#include <thread>

#include <N503/Diagnostics/ConsoleSink.hpp>
#include <N503/Diagnostics/Entry.hpp>
#include <N503/Diagnostics/FileSink.hpp>
#include <N503/Diagnostics/Reporter.hpp>
#include <N503/Diagnostics/Sink.hpp>

using namespace N503::Diagnostics;

int main()
{
    Reporter reporter{ std::chrono::milliseconds{ 200 } };

    ConsoleSink console;
    FileSink file("diagnostics.log");

    // Producer writes into the sink
    Entry e1{ Severity::Warning, 10, "first expected" };
    Entry e2{ Severity::Error, 20, "fatal expected" };

    console.AddEntry(e1);
    console.AddEntry(e2);

    // Submit sink to reporter to process asynchronously
    reporter.Submit(console); // reporter will DrainEntries() from console and process

    // Give worker time
    std::this_thread::sleep_for(std::chrono::milliseconds{ 300 });

    // Inspect stored entries in sink (after submit, sink should be drained)
    const auto& entries = console.GetEntries();
    std::cout << "Console sink entries after Submit: " << entries.size() << std::endl;

    return 0;
}