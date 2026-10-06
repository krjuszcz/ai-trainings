#include "retry_policy.hpp"

#include <exception>
#include <iostream>
#include <stdexcept>
#include <string>

namespace {
void require(bool condition, const std::string& message) {
    if (!condition) throw std::runtime_error(message);
}
}

int main() {
    using demo05::FailureCode;
    try {
        const auto retry = demo05::decide_retry(FailureCode::Timeout, 2);
        require(retry.retry && retry.delay_ms == 2000, "timeout attempt 2 should retry after 2000 ms");

        const auto stop = demo05::decide_retry(FailureCode::Timeout, 3);
        require(!stop.retry && stop.delay_ms == 0, "timeout attempt 3 should stop");

        const auto auth = demo05::decide_retry(FailureCode::AuthFailed, 1);
        require(!auth.retry && auth.delay_ms == 0, "auth failure must not retry");

        // Acceptance tests for task.md: red until RATE_LIMITED is implemented.
        const auto rate1 = demo05::decide_retry(FailureCode::RateLimited, 1);
        require(rate1.retry && rate1.delay_ms == 500, "rate limited attempt 1 should retry after 500 ms");

        const auto rate4 = demo05::decide_retry(FailureCode::RateLimited, 4);
        require(rate4.retry && rate4.delay_ms == 4000, "rate limited attempt 4 should retry after 4000 ms");

        const auto rate5 = demo05::decide_retry(FailureCode::RateLimited, 5);
        require(!rate5.retry && rate5.delay_ms == 0, "rate limited attempt 5 should stop");

        bool rejected = false;
        try {
            (void)demo05::decide_retry(FailureCode::Timeout, 0);
        } catch (const std::invalid_argument&) {
            rejected = true;
        }
        require(rejected, "attempt 0 must be rejected");

        std::cout << "PASS: retry policy baseline tests passed successfully\n";
        return 0;
    } catch (const std::exception& error) {
        std::cerr << "FAIL: " << error.what() << '\n';
        return 1;
    }
}
