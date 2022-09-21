#pragma once

#include <chrono>
#include <string>

namespace ErrorHandlingTest {

    inline constexpr size_t c_Iterations = 10;

    struct AlgorithmOutput
    {
        std::string               Text;
        std::chrono::milliseconds Elapsed;
    };

    namespace UseExceptions {

        AlgorithmOutput DoWork(size_t iterations, bool insertFailure);

    }

    namespace UseOptionals {

        AlgorithmOutput DoWork(size_t iterations, bool insertFailure);

    }

} // namespace ErrorHandlingTest
