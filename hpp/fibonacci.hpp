#ifndef FIBONACCI_HPP
#define FIBONACCI_HPP

#include <iostream>
#include <vector>
#include <algorithm>

// Forward declaration for CPU affinity function (defined in binary_search_single_core.hpp)
void set_cpu_affinity(int core_id);

// Iterative Fibonacci implementation
long long fibonacci_iterative(int n)
{
    if (n <= 1) {
        return n;
    }

    long long prev2 = 0;
    long long prev1 = 1;
    long long current;

    for (int i = 2; i <= n; i++) {
        current = prev1 + prev2;
        prev2 = prev1;
        prev1 = current;
    }

    return current;
}

// Recursive Fibonacci implementation (naive version)
long long fibonacci_recursive(int n)
{
    if (n <= 1) {
        return n;
    }
    return fibonacci_recursive(n - 1) + fibonacci_recursive(n - 2);
}

// Memoized Fibonacci implementation
long long fibonacci_memoized_helper(int n, std::vector<long long>& memo)
{
    if (n <= 1) {
        return n;
    }

    if (memo[n] != -1) {
        return memo[n];
    }

    memo[n] = fibonacci_memoized_helper(n - 1, memo) + fibonacci_memoized_helper(n - 2, memo);
    return memo[n];
}

long long fibonacci_memoized(int n)
{
    if (n <= 1) {
        return n;
    }

    std::vector<long long> memo(n + 1, -1);
    return fibonacci_memoized_helper(n, memo);
}

// Functions to run Fibonacci on a specific core
long long fibonacci_iterative_on_core(int n, int core_id = 0)
{
    set_cpu_affinity(core_id);  // Set affinity to the specified core
    return fibonacci_iterative(n);
}

long long fibonacci_recursive_on_core(int n, int core_id = 0)
{
    set_cpu_affinity(core_id);  // Set affinity to the specified core
    return fibonacci_recursive(n);
}

long long fibonacci_memoized_on_core(int n, int core_id = 0)
{
    set_cpu_affinity(core_id);  // Set affinity to the specified core
    return fibonacci_memoized(n);
}

#endif // FIBONACCI_HPP