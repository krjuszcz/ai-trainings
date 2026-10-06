# PostToolUse hook (matcher: Edit|Write). Flags x86/x64 portability risks in C++ files Claude just edited.
# Exit 2 returns the findings to Claude so it fixes them in the same turn.
$payload = [Console]::In.ReadToEnd() | ConvertFrom-Json
$file = [string]$payload.tool_input.file_path

if ($file -notmatch '\.(cpp|cc|cxx|h|hpp)$' -or -not (Test-Path -LiteralPath $file)) { exit 0 }

$checks = @(
    @{ Pattern = '\(\s*(int|long|unsigned|DWORD|ULONG)\s*\)\s*sizeof'; Why = 'sizeof yields size_t (64-bit on x64); cast to a 32-bit type truncates.' },
    @{ Pattern = '\(\s*(int|long|unsigned( long)?|DWORD|ULONG)\s*\)\s*&?\s*\w*(ptr|Ptr|p_)\w*'; Why = 'Pointer cast to a 32-bit integer; use uintptr_t or INT_PTR/DWORD_PTR.' },
    @{ Pattern = '\b(int|long|unsigned|DWORD)\s+\w+\s*=\s*\w+\.(size|length)\(\)'; Why = 'Container size stored in a 32-bit type; use size_t.' },
    @{ Pattern = 'static_cast\s*<\s*(int|long|unsigned( long| int)?|DWORD|ULONG)\s*>\s*\(.*(size|length|bytes|count)'; Why = 'static_cast of a size to a 32-bit type truncates silently on x64; return size_t.' },
    @{ Pattern = '\blong\s+\w+\s*=.*\b(size_t|ptrdiff_t)\b'; Why = 'long is 32-bit on both Windows x86 and x64, so it cannot hold a 64-bit size; use int64_t or size_t.' }
)

$findings = @()
$lineNo = 0
foreach ($line in Get-Content -LiteralPath $file) {
    $lineNo++
    foreach ($c in $checks) {
        if ($line -match $c.Pattern) { $findings += "${file}:${lineNo}: $($c.Why)`n    $($line.Trim())" }
    }
}

if ($findings.Count -gt 0) {
    [Console]::Error.WriteLine("check-32-64 hook found portability risks in the edited file:`n" + ($findings -join "`n") + "`nFix these, then state which targets (x86/x64) you actually built and tested.")
    exit 2
}
exit 0
