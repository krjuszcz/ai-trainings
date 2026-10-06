---
name: deliver-ticket
description: Deliver a ticket autonomously from spec to draft PR by chaining quick-spec, quick-dev, two adversarial reviews and a PR description. Use when the user gives a ticket file and asks to deliver, ship, or run the whole loop without stopping at each step.
---

# Deliver ticket (mock autonomous delivery)

Goal: one command from `TICKET.md` to a reviewed branch and a draft-PR description, with a report a human can audit in two minutes. Argument: ticket path, optionally followed by `open` to push and open the draft PR. Without `open`, stop at the description.

Run each phase in its own subagent (Agent tool) so the spec, not the chat history, is the contract. Pass paths and the previous phase's report; never pass the author's reasoning to a reviewer.

1. **Spec.** Subagent on Opus runs the `quick-spec` skill on the ticket. Autonomous mode: no approval stop. Answer open questions from the ticket and `CLAUDE.md`; if one still changes the implementation, stop and ask the user. Output: `docs/generated/spec-<slug>.md`.
2. **Branch.** `git switch -c feature/<TICKET-ID>-<slug>` from the current branch. Refuse to work on `main`.
3. **Implement.** Subagent on Sonnet runs `quick-dev` on the spec. It must run `build.bat` and report compiler, architecture, and targets not run.
4. **Review 1.** Fresh Opus subagent runs `adversarial-review` on `git diff` plus the spec. Fix every `blocker` and `major` (Sonnet subagent, same rules as step 3); record each `minor` as fixed or deferred with a reason. Rebuild.
5. **Review 2.** A new fresh Opus subagent reviews the updated diff. It must not see review 1's findings, so it can catch what the first pass missed. Fix blockers and majors once more and rebuild.
6. **Stop condition.** If blockers remain after review 2, or the build is red, stop. Do not open a PR. Report what failed.
7. **Commit.** One commit per logical change, message `<TICKET-ID>: <imperative summary>`. No force push, no `--no-verify`.
8. **PR description.** Write `docs/generated/pr-<TICKET-ID>.md` using the convention below.
9. **Open (only with `open`).** `git push -u origin HEAD`, then `gh pr create --draft --title "<TICKET-ID>: <summary>" --body-file docs/generated/pr-<TICKET-ID>.md`. Never mark ready, never merge.

## PR convention

- Branch: `feature/<TICKET-ID>-<slug>`. Title: `<TICKET-ID>: <imperative summary>`, under 72 characters. Always a draft.
- Body sections, in order: **Ticket** (link or id), **What changed**, **Acceptance criteria** (each with the input that proves it), **Verification** (exact commands, compiler, architecture, result), **Not run** (every target and check skipped), **Review** (table of findings from both passes: severity, decision, reason).

## Final report to the user

Branch name, commit list, build result, number of findings per review pass and their decisions, anything deferred, and whether a PR was opened (with URL) or only described.

## Gotchas

- Autonomous does not mean unreviewed: the draft PR is a handoff for a human, not a merge.
- Two review passes catch more only if the second starts blind; reusing context makes it repeat the first.
- Passing tests on one architecture do not prove another; the PR must say which one ran.
- If a step contradicts `CLAUDE.md` (for example C++20 on an MSVC 19.16 target), stop and report instead of working around it.
