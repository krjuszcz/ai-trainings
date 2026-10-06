# Demo 07 — Ticket to PR: a delivery loop you can reuse on your own tasks

**Goal:** run one small task through a repeatable loop with Claude Code: **ticket → spec → implementation in a fresh context → verification → adversarial review → draft PR**. The sample is a bounded C++17 task; the same loop applies to a real Jira ticket from your own repository.

```text
fetch ticket → /quick-spec → approve → new chat → /quick-dev → verify (high effort) → /adversarial-review → draft PR
```

Three project skills in `.claude/skills/` carry the loop (`quick-spec`, `quick-dev`, `adversarial-review`). They are short on purpose: they state goals and gotchas, not scripts. Each sets its own model: **Opus plans and reviews, Sonnet implements** (see [MODEL-ROUTING.md](../../MODEL-ROUTING.md)). They are workshop examples, not built-in commands. Keep `instructor/` closed while you work.

## Setup

```powershell
cd palo-alto-training\demos\07-ticket-to-pr
build.bat          # baseline: 2 checks fail by design
claude
```

`build.bat` finds `cl`, `clang++` or `g++`. Run `/skills` to confirm the three skills are listed. Record the compiler and architecture you use.

## Steps

### 1. Get the ticket (5 min)
Use `TICKET.md` (NET-1427) for the sample. For your own task, copy the ticket text from Jira into a local `TICKET.md` in your repository; no integration is needed. Ask Claude to restate the acceptance criteria and list anything ambiguous.

### 2. Spec with an interview (10 min, Opus, `/effort low`)
```text
/quick-spec TICKET.md
```
Answer the questions Claude asks, then approve. Expected: a saved `docs/generated/spec-*.md` with file-level tasks and Given/When/Then criteria, and no production code yet. Low effort is enough here because you are in the loop.

### 3. Implement in a fresh context (15 min, Sonnet, `/effort medium`)
Start a **new conversation** so the spec is the only contract:
```text
/quick-dev docs/generated/spec-<slug>.md
```
Expected: a code change, added tests, and a build result with the compiler and architecture named.

### 4. Verify (10 min, `/effort high`)
Run `build.bat` yourself. Open the diff, not the summary, and walk each acceptance criterion with a concrete input. Raise effort for this step: edge cases are what higher effort catches, not wrong approaches.

### 5. Adversarial review in a fresh context (10 min, Opus)
```text
/adversarial-review
```
Optionally also run `/ponytail-review` for over-engineering. Decide for each finding: fix, reject with a reason, or defer. Rerun `build.bat` after fixes.

### 6. Draft PR (5 min)
Ask Claude for a PR description that lists criteria covered, commands run with results, targets not run, and review findings with decisions. Open the PR with GitHub MCP only if your team allows it; otherwise stop at the description. Never let Claude push or merge unreviewed.

## Autonomous mode: one skill for the whole loop

`.claude/skills/deliver-ticket/` chains the three skills: spec, implement, two blind adversarial reviews with fixes, then a draft-PR description. Each phase runs in its own subagent, so no phase sees another's reasoning.

```text
/deliver-ticket TICKET.md          # stops at docs/generated/pr-NET-1427.md
/deliver-ticket TICKET.md open     # also pushes the branch and opens a draft PR
```

Use it after you have run steps 1–6 by hand once, so you know what the skill is skipping: the spec approval and your own diff reading. Treat the result as a handoff to a reviewer, not a merge candidate. Try it without `open` first; the PR convention (branch, title, body sections) is in the skill file, so change it there to match your team.

## Model routing

The skill `model` field switches the model for that skill's turn. Check which model is active with `/model` and confirm it matches the table above. If a skill does not switch in your version, use `/model opus` before steps 2 and 5 and `/model sonnet` before step 3, or start with `/model opusplan` (Opus in plan mode, Sonnet in execution). Measure it: run the same ticket entirely on Opus once and compare usage and retries.

## Try it on your own task

1. Pick a ticket with a reproducible problem and a narrow scope.
2. Run steps 1–6 in your repository. Copy the three skills into your repository's `.claude/skills/` and add your build command and platform notes to the `Gotchas` sections.
3. Note where Claude was wrong, and add that as a one-line gotcha: the skills improve with every failure you record.

## Stretch: scale it up

- **Parallel verification:** ask Claude to build a dynamic workflow that fans out reviewer agents (correctness, security, 32/64-bit) and synthesizes one report. Use it only for high-value changes; see the reading list.
- **Recurring triage:** `/loop` can rerun a check on an interval.
- **Cost control:** tell Claude a token budget for large tasks, and match effort to the step.

## Evidence to leave in the PR

| Item | Value |
|---|---|
| Ticket and criteria covered | |
| Command, compiler, architecture | |
| Targets not run | |
| Review findings and decisions | |
