# Instructor Notes — Demo 05

## Didactic Goal
Demonstrate how repository guardrails in `CLAUDE.md` prevent hallucinated side effects (`sleep`, threads, I/O logging) in legacy codebases, and how Ponytail minimalism achieves exponential backoff with a single arithmetic expression.

## Reference Solution (C++)

### Implementation (`src/retry_policy.cpp`):
```cpp
#include "retry_policy.hpp"
#include <stdexcept>

namespace demo05 {

RetryDecision decide_retry(FailureCode code, int attempt) {
    if (attempt < 1) {
        throw std::invalid_argument("attempt must be a positive integer");
    }
    if (code == FailureCode::AuthFailed) {
        return {false, 0};
    }
    if (code == FailureCode::RateLimited) {
        if (attempt > 4) {
            return {false, 0};
        }
        // ponytail: closed-form exponential backoff using bitshift
        return {true, 500 * (1 << (attempt - 1))};
    }
    const bool retryable = code == FailureCode::Timeout || code == FailureCode::TemporaryUnavailable;
    if (!retryable || attempt >= 3) {
        return {false, 0};
    }
    return {true, attempt * 1000};
}

}  // namespace demo05
```

## Reference Solution (Go)

### Implementation (`retry/retry.go`):
```go
func DecideRetry(code FailureCode, attempt int) (Decision, error) {
	if attempt < 1 {
		return Decision{}, fmt.Errorf("attempt must be a positive integer, got %d", attempt)
	}
	if code == AuthFailed {
		return Decision{Retry: false, DelayMs: 0}, nil
	}
	if code == RateLimited {
		if attempt > 4 {
			return Decision{Retry: false, DelayMs: 0}, nil
		}
		// ponytail: closed-form exponential backoff using bitshift
		return Decision{Retry: true, DelayMs: 500 * (1 << (attempt - 1))}, nil
	}
	retryable := code == Timeout || code == TemporaryUnavailable
	if !retryable || attempt >= 3 {
		return Decision{Retry: false, DelayMs: 0}, nil
	}
	return Decision{Retry: true, DelayMs: attempt * 1000}, nil
}
```
