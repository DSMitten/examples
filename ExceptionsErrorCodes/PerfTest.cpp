#include "PerfTest.h"

#include <Windows.h>

#include <cstdlib>

namespace PerfTest {

    bool ShouldInjectError(int randomValue, double failureRate)
    {
        return static_cast<double>(randomValue) / static_cast<double>(RAND_MAX) <= failureRate;
    }

    int DoRealWork(int input)
    {
        HWND hwnd = GetDesktopWindow();

        RECT rect;
        (void)GetWindowRect(hwnd, &rect);

        LONG width = rect.right - rect.left;
        LONG height = rect.bottom - rect.top;

        (void)PostMessageW(hwnd, WM_NULL, (WPARAM)width, (WPARAM)height);

        return input + height / 100;
    }

    namespace UseExceptions {

        int CalculateSomething(int existingValue, double failureRate)
        {
            // existingValue = DoRealWork(existingValue);

            int randomValue = std::rand();

            if (ShouldInjectError(randomValue, failureRate))
            {
                throw std::runtime_error("Sompin wrong here uhuh");
            }

            return existingValue + (randomValue % 3);
        }

        AlgorithmOutput DoPerfTest(size_t iterations, double failureRate)
        {
            auto start = std::chrono::high_resolution_clock::now();

            int result = 0;
            for (size_t index = 0; index < c_Iterations; index++)
            {
                try
                {
                    result = CalculateSomething(result, failureRate);
                }
                catch (const std::exception&)
                {
                }
            }

            std::chrono::nanoseconds elapsed =
                std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start);

            return {failureRate, result, elapsed};
        }

    } // namespace UseExceptions

    namespace UseOptionals {

        std::optional<int> CalculateSomething(int existingValue, double failureRate)
        {
            // existingValue = DoRealWork(existingValue);

            int randomValue = std::rand();

            if (ShouldInjectError(randomValue, failureRate))
            {
                return std::nullopt;
            }

            return existingValue + (randomValue % 3);
        }

        AlgorithmOutput DoPerfTest(size_t iterations, double failureRate)
        {
            auto start = std::chrono::high_resolution_clock::now();

            int result = 0;
            for (size_t index = 0; index < c_Iterations; index++)
            {
                std::optional<int> newResult = CalculateSomething(result, failureRate);
                if (newResult.has_value())
                {
                    result = newResult.value();
                }
            }

            std::chrono::nanoseconds elapsed =
                std::chrono::duration_cast<std::chrono::nanoseconds>(std::chrono::high_resolution_clock::now() - start);

            return {failureRate, result, elapsed};
        }

    } // namespace UseOptionals

} // namespace PerfTest