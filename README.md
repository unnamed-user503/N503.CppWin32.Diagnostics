# N503::Diagnostics

## シンプルなコード

```cpp
// 準備
auto reporter = std::make_unique<Diagnostics::Reporter>();
auto console  = std::make_shared<Diagnostics::ConsoleSink>();

reporter->AddSink(console);

// 実行時
Diagnostics::Sink tempSink;
tempSink.AddEntry({ Diagnostics::Severity::Error, 10, "Expected ';'" });

// Reporterに投げれば、別スレッドでコンソールとファイルの両方に出力される
reporter->Submit(tempSink);
```
