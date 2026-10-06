#pragma once

namespace demo05 {

enum class FailureCode { Timeout, TemporaryUnavailable, AuthFailed, RateLimited };

struct RetryDecision {
    bool retry;
    int delay_ms;
};

RetryDecision decide_retry(FailureCode code, int attempt);

}  // namespace demo05
