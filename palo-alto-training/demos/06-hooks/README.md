# Demo 06 — Hooks: guardrails Claude cannot forget

**Goal:** show that a hook is a script the Claude Code harness runs on an event, so a rule is enforced every time instead of relying on the model remembering a line in `CLAUDE.md`. Exit code `2` blocks the action and returns the script's stderr to Claude, which then fixes the problem in the same turn.

| Hook | Event | What it does | Why it matters for this team |
|---|---|---|---|
| `guard-bash.ps1` | `PreToolUse` (Bash) | Blocks `git push --force`, `git reset --hard`, `rm -rf`, `terraform apply/destroy` | Four or five repositories and real AWS infrastructure: destructive commands need a human |
| `check-32-64.ps1` | `PostToolUse` (Edit/Write) | Flags `(int)sizeof`, pointer-to-32-bit casts, `int n = v.size()`, `long` assumptions | x86/x64 defects are a known source of legacy bugs |
| `syntax-check.ps1` | `PostToolUse` (Edit/Write) | Syntax-only compile of the edited `.cpp` file (`cl`, else `clang++`, else `g++`) | Claude learns about a broken edit immediately, not after the build |

Hooks are configured in `.claude/settings.json` in this directory, so they apply only when Claude Code is started here. A hook is a command with the user's permissions, so review every hook script before enabling it.

## 1. Dry run without Claude (2 minutes)

```powershell
cd palo-alto-training\demos\06-hooks
powershell -NoProfile -ExecutionPolicy Bypass -File test-hooks.ps1
```

Each line feeds a hook the JSON Claude Code would send and checks the exit code. The two syntax tests run only when `cl` (Developer Command Prompt), `clang++` or `g++` is on `PATH`; otherwise the script prints `SKIP`, because the syntax hook skips silently without a compiler.

## 2. Live demo in Claude Code

```powershell
cd palo-alto-training\demos\06-hooks
claude
```

Run `/hooks` to show the three registered hooks, then try these prompts:

1. **Blocked command:** "Run exactly `git push --force origin HEAD` and show me the output." The guard stops it with its message. Name the exact command: a vague "force-push" prompt lets Claude choose `--force-with-lease`, which the hook deliberately allows, so nothing is blocked. After the block, ask "Now push with `--force-with-lease`" to show the allowed alternative.
2. **Portability check:** "In `src/buffer_utils.cpp` add `int total_bytes(const std::vector<unsigned char>&)` that returns the buffer size." The natural implementation narrows `size_t` to `int` (`int n = v.size();` or `static_cast<int>(v.size())`); the hook reports the line and Claude corrects it or explains the limit.
   Reset the file before the next prompt (`git checkout -- src/buffer_utils.cpp`). The hook scans the whole file, so a leftover `total_bytes` keeps blocking every later edit.
3. **Syntax check:** "Add a function `checksum` to `src/buffer_utils.cpp`." Any compile error comes back to Claude before it reports success.

`sample/risky.cpp` and `sample/broken.cpp` are static files that trigger the hooks when edited.

## Make it your own (10 minutes)

Ask Claude to write one more hook for a rule your team keeps repeating, for example:
- Run `clang-format` on every edited `.cpp`/`.h` file.
- Block edits to a generated or vendored directory.
- Run the focused unit test for the file that changed.

Keep a hook fast (under a few seconds), deterministic, and quiet on success. Move a hook from `.claude/settings.json` in the repository to the team's shared settings only after the team has agreed on it.

## Other hook events worth knowing

`UserPromptSubmit` (add context to every prompt), `Stop` (run tests before Claude declares it is done), `SessionStart` (print branch and build status). Commit-message cleanup belongs in a plain git `commit-msg` hook, not a Claude hook.
