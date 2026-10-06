# Demo 05 — Rules & Guardrails: Pure Domain Logic & Refactoring (C++ & Go)

**Goal:** Understand how repository instructions (`CLAUDE.md`) and Ponytail principles prevent AI agents from introducing unwanted side effects (sleeping, threads, I/O, logging) into legacy domain logic.

## The Problem in Legacy Codebases

When asked to "implement retry logic with exponential delay" in a 25-year-old C++ or Go service, LLMs by default frequently:
1. Inject `std::this_thread::sleep_for(...)`, `time.Sleep(...)`, or Windows `Sleep()` directly into the business logic.
2. Spawn background threads or goroutines.
3. Call logging frameworks or print directly to standard output.
4. Add complex state machines or dynamic memory allocations.

This demo demonstrates how **repository guardrails** in `CLAUDE.md` enforce:
- **Pure calculations:** The function only computes `{retry: bool, delay_ms: int}`. Scheduling and sleep belong to the outer caller/orchestrator.
- **Two-phase workflow:** Plan test cases first (in Plan Mode) before modifying any production code.
- **Ponytail minimalism:** Implement exponential backoff in a single arithmetic expression (`500 * (1 << (attempt - 1))`) without math libraries or loops.

## Structure

- `cpp/` — C++17 retry policy (Windows, MSVC).
- `go/` — Go microservice retry policy (`go test ./...`).
- `CLAUDE.md` — Repository instructions defining the guardrails.
- `task.md` — The feature specification.
- `prompt.md` — Step-by-step prompts for participants.
- `instructor/` — Solution and facilitation guide.

## Participant Flow

1. **Verify Baseline:**
   - **C++ (Windows build script):**
     ```bat
     cd cpp
     build.bat
     ```
   - **C++ (Manual MSVC or Clang on Windows):**
     ```bat
     cd cpp
     cl /nologo /std:c++17 /W4 /EHsc /Iinclude src\retry_policy.cpp tests\retry_policy_test.cpp /Fe:retry_policy_test.exe
     retry_policy_test.exe
     ```
   - **Go:**
     ```sh
     cd go
     go test -v ./...
     ```

   The baseline is intentionally **red**: the tests already contain the `RATE_LIMITED` acceptance cases from `task.md`, so the run fails (`FAIL: rate limited attempt 1 should retry after 500 ms`). Step B turns it green.

2. **Step A — Plan Mode (Test Planning First):**
   - Use the prompt in [prompt.md](prompt.md) (Step A).
   - Require Claude to formulate unit test cases and boundary conditions *without* editing production code.

3. **Step B — Bounded Implementation:**
   - Use the prompt in [prompt.md](prompt.md) (Step B).
   - Ensure the diff satisfies `CLAUDE.md` guardrails (pure function, no `sleep`, no threads, minimal arithmetic expression).

4. **Verify:**
   - Re-run the test suite and confirm clean execution with zero warnings.
