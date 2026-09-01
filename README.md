# Blue Pill スタータープロジェクト / Starter Project

STM32F103C8「Blue Pill」ボード（ARM Cortex-M3）向けのベアメタル C プロジェクトです。

コードを push すると GitHub がビルドし、シミュレータで実際に動かして、LED が
光り、ボタンが読まれ、シリアルに文字が出ることを確認します。

*Bare-metal C for the STM32F103C8 "Blue Pill" board (ARM Cortex-M3). Push your
code and GitHub builds it, then runs it in a simulator that shows the LEDs
lighting, the button being read, and the characters arriving on the serial
line.*

> **この README は日英併記です。** 日本語が先、英語が後に続きます。技術用語
> —— キーワード、レジスタ名、関数名、ファイル名、コンパイラのフラグ —— は
> 英語のままにしてあります。データシートもコンパイラの出力もその表記だからです。
>
> *This README is bilingual, Japanese first. Technical terms stay in English
> because that is how the datasheet and the compiler write them.*

## ファイル構成 / What is here

| File | 内容 / What it does |
| --- | --- |
| `main.c` | エントリポイント。ここから読み始めてください。<br>*Entry point. Start here.* |
| `uart.c` | USART1 のシリアルドライバ。文字が画面に届く仕組み。<br>*USART1 serial driver — how characters reach your screen.* |
| `gpio.c` | LED（PC13 と PA1）とボタン（PA0）。<br>*The LEDs on PC13 and PA1, and the button on PA0.* |
| `startup.c` | ベクタテーブルとリセットハンドラ。`main` より前に動きます。<br>*Vector table and reset handler; runs before `main`.* |
| `link.ld` | メモリ配置。flash は `0x08000000`（64K）、RAM は `0x20000000`（20K）。<br>*Memory layout.* |
| `Makefile` | `make` でビルド。`make sim` はシミュレータを呼びますが、**課金対象**です（下記）。<br>*`make` to build. `make sim` calls the simulator and is **metered** — see below.* |
| `wokwi.toml` | どのファームウェアをシミュレータに読み込ませるか。<br>*Which firmware the simulator loads.* |
| `diagram.json` | 配線図。ボード、LED、抵抗、ボタン、ロジックアナライザ。<br>*The wiring: board, LED, resistor, button, logic analyser.* |
| `.editorconfig` | ファイルを UTF-8 で保存するようエディタに指示します。**変更しないでください。**<br>*Tells your editor to save files as UTF-8. Do not change it.* |

## ビルドする / Build it

`arm-none-eabi-gcc`（14.2）と GNU Make が必要です。

```bash
make
```

`app.elf`（シミュレータが実行するもの）と `app.hex` ができます。

*You need `arm-none-eabi-gcc` (14.2) and GNU Make. Produces `app.elf` — what
the simulator runs — and `app.hex`.*

## 手元で動かす / Run it on your own machine

VS Code の Wokwi 拡張機能を入れると、この `diagram.json` の回路がエディタの
中で動きます。ボードも LED もボタンも画面に出て、ロジックアナライザで PA9 の
波形が見られます。**この方法はシミュレーション時間を消費しません。**

1. `make` でビルドします。
2. VS Code で `F1` → **Wokwi: Start Simulator** を選びます。

*Install the Wokwi extension for VS Code and this circuit runs inside the
editor: the board, the LEDs, the button, and a logic analyser on PA9. **This
does not consume simulation time.** Build with `make`, then `F1` → Wokwi:
Start Simulator.*

## シミュレーション時間について / About simulation time

> **重要。** CI のシミュレータは **従量制** です。トークンを持つアカウントに
> 月あたりの上限があり、そのトークンを使うすべてのリポジトリで共有します。
> 使い切ると、その月は誰もシミュレーションできません。

そのため、この構成は次のようになっています。

- コンパイルが失敗したファームウェアはシミュレータに渡されません。時間を
  消費しません。
- 既定では「何か出力があれば合格、無音なら不合格」です。期待する文字列は
  ワークフローに書き込まれていません。課題ごとに出力が違うため、共通の
  既定値はほとんどの課題にとって誤りになるからです。
- この既定では途中で打ち切れないため、毎回タイムアウトまで実行されます。
  タイムアウトは既定で 2 秒、リポジトリ変数 `WOKWI_TIMEOUT_MS` で変えられます。
- 正確な文字列が必要な課題では、リポジトリ変数 `WOKWI_EXPECT_TEXT` を
  設定してください。その文字列が出た瞬間に終了するので 1 秒未満で済みます。
- 続けて push すると、古い実行は取り消され、1 回分だけが課金されます。
- リポジトリ変数 `WOKWI_SIM_ENABLED` を `false` にすると、シミュレーションを
  すべて止められます。ワークフローを編集する必要はありません。

`make sim` はローカルからでも従量制のシミュレータを呼びます。**まず VS Code
拡張機能で確認してください。**

*The CI simulator is **metered**: the account that owns the token has a monthly
allowance shared by every repository using it. Broken firmware is never
simulated. By default any serial output passes and only silence fails — no
expected string is written into the workflow, because every exercise prints
something different. That default cannot stop early, so every run lasts until
the timeout, which is 2 seconds and set by `WOKWI_TIMEOUT_MS`. An exercise
needing exact output sets `WOKWI_EXPECT_TEXT` and gets the early exit back.
Repeated pushes are billed once. Set `WOKWI_SIM_ENABLED` to `false` to stop
simulating entirely. `make sim` is metered too — check in the VS Code extension
first.*

## push する / Push it

push するたびにファームウェアがビルドされ、シミュレータの確認が走り、
`app.elf` と `serial.log` がアップロードされます。**Actions** タブで進行を
見られます。`serial.log` にはファームウェアが実際に出力した内容が残ります。

*Every push builds your firmware, runs the simulator check, and uploads
`app.elf` and `serial.log`. That log is what the firmware actually printed.*

## 文字コードについて / About file encoding

すべてのファイルを **UTF-8** で保存してください。`.c` や `.h` だけでは
なく、`Makefile` や `wokwi.toml`、`diagram.json` も対象です。AI アシスタント
（下記）は日本語でヒントを書くので、ファイルに日本語が含まれます。エディタが
Shift-JIS で保存し直すと、その日本語が壊れ、ビルドも失敗します。

`.editorconfig` が VS Code に UTF-8 を指示するので、通常は何もしなくて
構いません。もしビルドが `UTF-8 ではありません` というエラーで落ちたら、
VS Code の右下のエンコーディング表示から **Save with Encoding → UTF-8** を
選んで保存し直してください。

*Save every file as UTF-8, not only the C sources — the Makefile, wokwi.toml
and diagram.json count too. The AI assistant writes hints in Japanese, so your
files will contain Japanese text; an editor that re-saves them as Shift-JIS
will corrupt it and break the build. If CI fails with a `not valid UTF-8`
error, use **Save with Encoding → UTF-8** in VS Code.*

## AI ティーチングアシスタント / The AI teaching assistant

担当教員が有効にしている場合、ビルドが壊れたときに AI アシスタントが助けて
くれます。**ビルドが失敗すると**、アシスタントはエラーを読み、あなたのコードに
コメント行としてヒントを書き込み、ブランチに push します。

```c
/* AI-HINT: このループはハードウェアレジスタを読んでいます。コンパイラが
   再読み込みせずにキャッシュしてしまうのを防ぐには何が必要ですか */
while (uart_busy()) { }
```

流れはこうなります。

1. ビルドが通らないコードを push します。
2. 1 分ほど待ってから **`git pull`** します。
3. ヒントがファイルの中、注意すべき行の上に入っています。
4. コードを直します。マーカーは自分で消さないでください。自動的に消えます。
5. もう一度 push します。

ビルドが成功すると、アシスタントは変更内容をレビューしてコミットにコメントし、
GitHub がそれをメールで送ります。同時に `AI-HINT` の行を消すので、完成した
ファイルには何も残りません。

**直し始める前に pull してください。** pull せずに編集すると、アシスタントが
すでにコミットを追加しているため push が拒否されます。待ちたくない場合は
自分で直して push してください。アシスタントはブランチが進んだことに気づき、
手を出しません。

これは **答えではなくヒント** です。修正を自分で考えることが演習です。
アシスタントができるのは自分のコメント行を足すことと消すことだけで、あなたの
コードは 1 行も変えられません。

*It gives **hints, not answers** — working out the fix is the exercise. It can
only add and remove its own comment lines; it cannot change a line of your
code.*

## ビルドが失敗したら / When the build fails

Actions のログで **最初の** エラーを読んでください。後ろのエラーはたいてい
最初のエラーの波及です。

このプロジェクトは `-Wall -Wextra -Werror` でコンパイルするので、**警告は
すべてビルドを止めます**。他の場所なら無害な unused variable も、ここでは
ハードエラーです。これは意図的です。マイコンでは、無視した警告が後で追いかける
バグになります。

*Read the **first** error in the Actions log. This project compiles with
`-Wall -Wextra -Werror`, so any warning stops the build.*

| Message | 意味 / Usually means |
| --- | --- |
| `undefined reference to 'foo'` | リンカが `foo` を見つけられません。その `.c` ファイルは `Makefile` の `SRCS` に入っていますか。<br>*The linker cannot find it. Is its `.c` file listed in `SRCS`?* |
| `implicit declaration of function 'foo'` | 宣言する前に `foo` を呼んでいます。<br>*You called it before declaring it.* |
| `unused variable` / `unused parameter` | `-Werror` が警告をエラーに変えています。<br>*`-Werror` turning a warning into an error.* |
| `region 'FLASH' overflowed` | プログラムが `link.ld` のメモリ予算を超えました。<br>*Your program outgrew the memory budget.* |
| `undefined reference to 'printf'`（`malloc`、`memcpy` も） | 標準ライブラリはありません。`-nostdlib` なので自分で書くか使わないかです。<br>*There is no standard library here.* |
| シミュレーションはタイムアウトしたが `make` は通る | コンパイルはできたが、期待した文字列がシリアルに出ませんでした。<br>*It compiled, but the expected text never reached the serial line.* |
| `... は UTF-8 ではありません` | ファイルが Shift-JIS などで保存されています。UTF-8 で保存し直してください。<br>*Re-save the file as UTF-8.* |

### エラーメッセージの出ないバグ / The bug with no error message

このボードでは、**すべての周辺機器がクロックを止められた状態で起動します**。
`RCC_APB2ENR` の該当ビットを立てるまで、その周辺機器のレジスタは読んでも 0 で、
書いても効きません。

そのため、`RCC_USART1EN` を立て忘れると次のようになります。コンパイルは通り、
警告も出ず、`uart_init` は成功したように見えます。しかし `TXE` は永久に
立たないため、`uart_putc` の待ちループから抜けられません。出力は一文字も
出ず、エラーメッセージもどこにも出ません。

```c
RCC_APB2ENR |= RCC_IOPAEN | RCC_USART1EN;   /* これを忘れると沈黙します */
```

同じことが GPIO にも当てはまります。`RCC_IOPAEN` を忘れれば、ピンは設定した
とおりに動きません。何も光らないときは、まずクロックを疑ってください。

もうひとつ、ハードウェアレジスタを待つループが固まる場合は、そのレジスタが
`volatile` で宣言されているか確認してください。`volatile` がないと、
コンパイラはレジスタを一度だけ読んでその値を使い回すことがあります。
これも警告が一切出ません。

*Every peripheral on this part boots with its clock switched off. Until the
matching bit in `RCC_APB2ENR` is set, its registers read as zero and ignore
writes. Forget `RCC_USART1EN` and the code compiles cleanly, `uart_init`
appears to succeed, `TXE` never becomes set, and `uart_putc` spins forever —
no output and no error message anywhere. The same applies to `RCC_IOPAEN` for
the pins. When nothing lights up, suspect the clock first. Separately, if a
loop waiting on a register hangs, check that the register is declared
`volatile`; that one produces no warning either.*

## 回路を変える / Changing the circuit

`diagram.json` が配線を決めます。部品を足すときは `parts` に追加し、
`connections` で結びます。VS Code 拡張機能なら、変更したあと保存して
シミュレータを再起動するだけで反映されます。

*`diagram.json` defines the wiring. Add to `parts`, then join them in
`connections`. In the VS Code extension, save and restart the simulator.*

## もっと学ぶ / Learning more

- [Wokwi documentation](https://docs.wokwi.com/)
- [Wokwi STM32 Blue Pill part](https://docs.wokwi.com/parts/board-stm32-bluepill)
- [STM32F103 reference manual (RM0008)](https://www.st.com/resource/en/reference_manual/rm0008-stm32f101xx-stm32f102xx-stm32f103xx-stm32f105xx-and-stm32f107xx-advanced-armbased-32bit-mcus-stmicroelectronics.pdf)
