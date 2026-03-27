# N503 Diagnostics

軽量な診断情報（エラー・警告・情報）を扱うためのユーティリティです。  
パーサや字句解析器などで、「どこまで解析が進んだか」を基準に最も有益なエラーを収集する用途を想定しています。

---

## 構成

### Severity

診断情報の重要度を表す列挙型です。

- Info    : 補足的な情報  
- Warning : 実行可能だが注意が必要  
- Error   : 処理継続が困難な致命的状態  

### Entry

1件の診断情報を表す構造体です。

- Severity : 情報の重要度  
- Position : 入力ストリーム中のオフセット  
- Expected : その位置で期待されていた内容  

### Sink

診断情報を収集し、「最も遠くまで解析が進んだ地点」の情報だけを保持するクラスです。

- より遠い位置のエラーが来たら、古い情報は破棄  
- 同じ位置なら複数の候補として蓄積  
- 手前の位置のエラーは無視  

---

## Sink の動作

```cpp
if (entry.Position > m_FurthestPosition)
{
    // より遠い位置でのエラー → 古い情報を捨てて更新
    m_Entries.clear();
    m_FurthestPosition = entry.Position;
    m_Entries.push_back(entry);
}
else if (entry.Position == m_FurthestPosition)
{
    // 同じ位置での別の可能性 → 追加
    m_Entries.push_back(entry);
}
```

この仕組みにより、パーサが複数の分岐を試した場合でも、  
最も深く進んだ地点のエラーだけが残るため、ユーザーにとって有益な診断が得られます。

---

## 使用例

```cpp
N503::Diagnostics::Sink sink;

sink.Report({ Severity::Error, 10, "identifier" });
sink.Report({ Severity::Error, 15, "expression" });
sink.Report({ Severity::Error, 15, "number" });

if (sink.HasError())
{
    for (const auto& e : sink.GetEntries())
    {
        std::cout << "Error at " << e.Position
                  << ": expected " << e.Expected << "\n";
    }
}
```

この場合、位置 10 のエラーは破棄され、位置 15 の 2 件のみが保持されます。
