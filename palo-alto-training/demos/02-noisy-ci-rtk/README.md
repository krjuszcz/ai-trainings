# Demo 02 — Raw vs Compact Terminal Output (RTK)

**Goal:** Understand how noisy terminal and test runner outputs consume precious context window tokens in Claude Code, and how output filtering (like RTK) preserves diagnostic signal while cutting token usage (the simulated example below shows a large drop; real savings depend on the command).

## Context: Legacy Builds & Test Suites

In a 25-year-old C++ Windows codebase or Go microservice suite:
- C++ compilers (MSVC/Clang) often dump hundreds of lines of template instantiation warnings or informational notes.
- Test suites run 100+ passing tests before a single failure occurs.
- If an engineer pastes or pipes the raw 150-line output into Claude Code, ~1,200+ tokens are consumed by noise. The model's attention is diffused across irrelevant passes.
- A compact filter extracts only the failing test name, source line, expected vs actual values, and summary (~50 tokens).

## Install RTK for Claude Code (Windows, optional)

The demo programs below only simulate RTK output. To try the real tool, install [RTK](https://github.com/rtk-ai/rtk), a single Rust binary that rewrites common shell commands so their output is compressed before Claude reads it.

1. Install from a terminal (Command Prompt, PowerShell, or Windows Terminal). Do not double-click `rtk.exe`.
   ```bat
   winget install rtk-ai.rtk
   ```
   Without winget, download `rtk-x86_64-pc-windows-msvc.zip` from the project's releases page and add `rtk.exe` to your `PATH`.
2. Optional: some filters use ripgrep. `winget install BurntSushi.ripgrep.MSVC` avoids warnings.
3. Register the Claude Code hook and restart Claude Code:
   ```bat
   rtk init -g
   ```
4. Verify:
   ```bat
   rtk --version
   rtk gain
   ```
5. Remove it later with `rtk init -g --uninstall`, then uninstall the binary (`cargo uninstall rtk` if it was installed through Cargo).

The hook rewrites Bash commands only. Claude Code's built-in `Read`, `Grep`, and `Glob` tools bypass it, so those outputs are not compressed. The project claims 60–90% token reduction on common dev commands; measure it on your own builds with `rtk gain` before quoting a number. Check the [RTK README](https://github.com/rtk-ai/rtk) for current instructions, because installation details can change.

## Example: `ls` with and without RTK

The two blocks below come from [RTK's documentation](https://mintlify.wiki/rtk-ai/rtk/concepts/token-savings), a third-party mirror of the project docs. They are illustrative, not captured from a run on this machine. The sample uses Unix-style output, and the token counts are the docs' own estimates. Run `rtk ls` on a real folder to get your own numbers.

On Windows PowerShell, `ls` is an alias, not an executable, so `rtk ls` fails with `Binary 'ls' not found on PATH`. Add Git's Unix tools to `PATH` for the session first:

```powershell
$env:Path += ";C:\Program Files\Git\usr\bin"
ls.exe -la
rtk ls -la
```

Plain `ls -la` fails in PowerShell (`A parameter cannot be found that matches parameter name 'la'`) because the alias points to `Get-ChildItem`. Use `ls.exe` to run the real binary for the uncompressed side of the comparison.

Standard `ls -la` (about 45 lines, ~800 tokens):

```text
drwxr-xr-x  15 user  staff    480 Jan 23 10:00 .
drwxr-xr-x   5 user  staff    160 Jan 23 09:00 ..
-rw-r--r--   1 user  staff   1234 Jan 23 10:00 Cargo.toml
-rw-r--r--   1 user  staff   5678 Jan 23 10:00 Cargo.lock
drwxr-xr-x   8 user  staff    256 Jan 23 10:00 src
...
```

`rtk ls` (12 lines, ~150 tokens):

```text
📁 my-project/
├── src/ (8 files)
│   ├── main.rs
│   ├── lib.rs
│   └── filter.rs
├── tests/ (3 files)
├── Cargo.toml
├── Cargo.lock
├── README.md
└── .gitignore
```

The docs report 81% savings (650 tokens saved). Permission strings, owners, sizes, and dates are dropped; the structure and file names Claude needs remain.

## Modes

Both C++ and Go versions generate a simulated test suite of 120 passing checks and 1 failure:
- `--mode raw`: Full verbose runner output (all passes + failure).
- `--mode compact`: Filtered diagnostic signal (failure + line location + summary only).
- `--mode compare`: Comparison table of lines, bytes, and approximate tokens.

*(Note: Both programs exit with code `1` by design to simulate a failing CI check).*

## Running the Demo

### C++ (Windows build script):
```bat
cd cpp
build.bat
```

### C++ (Manual MSVC or Clang on Windows):
```bat
cd cpp
cl /nologo /std:c++17 /EHsc src\noisy_ci.cpp /Fe:noisy_ci.exe
noisy_ci.exe --mode compare
```

### Go:
```sh
cd go
go run ./cmd/noisy-ci --mode compare
```

### Fallback without compiling:
Inspect [fixtures/raw-output-excerpt.txt](fixtures/raw-output-excerpt.txt) and [fixtures/compact-output.txt](fixtures/compact-output.txt). Compare what information was kept and what was dropped.
