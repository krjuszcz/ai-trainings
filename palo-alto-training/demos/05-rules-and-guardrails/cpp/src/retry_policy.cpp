#include "retry_policy.hpp"

#include <stdexcept>

namespace demo05 {

RetryDecision decide_retry(FailureCode code, int attempt) {
    if (attempt < 1) {
        throw std::invalid_argument("attempt must be a positive integer");
    }
    const bool retryable = code == FailureCode::Timeout || code == FailureCode::TemporaryUnavailable;
    if (!retryable || attempt >= 3) {
        return {false, 0};
    }
    return {true, attempt * 1000};
}

}  // namespace demo05
