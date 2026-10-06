package retry

import (
	"testing"
)

func TestBaselineRetryPolicy(t *testing.T) {
	tests := []struct {
		name        string
		code        FailureCode
		attempt     int
		wantRetry   bool
		wantDelayMs int
		wantErr     bool
	}{
		{
			name:        "timeout attempt 1",
			code:        Timeout,
			attempt:     1,
			wantRetry:   true,
			wantDelayMs: 1000,
		},
		{
			name:        "timeout attempt 2",
			code:        Timeout,
			attempt:     2,
			wantRetry:   true,
			wantDelayMs: 2000,
		},
		{
			name:        "timeout attempt 3 stops",
			code:        Timeout,
			attempt:     3,
			wantRetry:   false,
			wantDelayMs: 0,
		},
		{
			name:        "auth failed stops immediately",
			code:        AuthFailed,
			attempt:     1,
			wantRetry:   false,
			wantDelayMs: 0,
		},
		// Acceptance tests for task.md: red until RateLimited is implemented.
		{
			name:        "rate limited attempt 1",
			code:        RateLimited,
			attempt:     1,
			wantRetry:   true,
			wantDelayMs: 500,
		},
		{
			name:        "rate limited attempt 4",
			code:        RateLimited,
			attempt:     4,
			wantRetry:   true,
			wantDelayMs: 4000,
		},
		{
			name:        "rate limited attempt 5 stops",
			code:        RateLimited,
			attempt:     5,
			wantRetry:   false,
			wantDelayMs: 0,
		},
		{
			name:    "attempt 0 error",
			code:    Timeout,
			attempt: 0,
			wantErr: true,
		},
	}

	for _, tt := range tests {
		t.Run(tt.name, func(t *testing.T) {
			got, err := DecideRetry(tt.code, tt.attempt)
			if (err != nil) != tt.wantErr {
				t.Fatalf("DecideRetry() error = %v, wantErr %v", err, tt.wantErr)
			}
			if !tt.wantErr {
				if got.Retry != tt.wantRetry {
					t.Errorf("got retry %v, want %v", got.Retry, tt.wantRetry)
				}
				if got.DelayMs != tt.wantDelayMs {
					t.Errorf("got delay %v, want %v", got.DelayMs, tt.wantDelayMs)
				}
			}
		})
	}
}
