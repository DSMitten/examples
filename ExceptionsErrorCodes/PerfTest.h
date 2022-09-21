#pragma once

#include <chrono>
#include <string>

namespace PerfTest {

    inline constexpr size_t c_Iterations = 5000;

    struct AlgorithmOutput
    {
        double                   FailureRate;
        int                      Result;
        std::chrono::nanoseconds Elapsed;
    };

    namespace UseExceptions {

        AlgorithmOutput DoPerfTest(size_t iterations, double failureRate);

    }

    namespace UseOptionals {

        AlgorithmOutput DoPerfTest(size_t iterations, double failureRate);

    }

} // namespace PerfTest
