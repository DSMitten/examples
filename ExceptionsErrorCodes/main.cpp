#include "BadExceptionsExample.h"
#include "ErrorHandlingTest.h"
#include "PerfTest.h"

#include <cstdlib>
#include <ctime>
#include <iomanip>
#include <iostream>
#include <string_view>
#include <unordered_map>
#include <vector>

void DoPerfTest()
{
    using namespace PerfTest;

    std::vector<AlgorithmOutput> exceptionOutputs;

    // Warmup - run perf test but don't store value
    UseExceptions::DoPerfTest(c_Iterations, 0.5);
    exceptionOutputs.push_back(UseExceptions::DoPerfTest(c_Iterations, 0.0));
    exceptionOutputs.push_back(UseExceptions::DoPerfTest(c_Iterations, 0.01));
    exceptionOutputs.push_back(UseExceptions::DoPerfTest(c_Iterations, 0.1));
    exceptionOutputs.push_back(UseExceptions::DoPerfTest(c_Iterations, 1.0));

    std::vector<AlgorithmOutput> errorCodeOutputs;

    // Warmup - run perf test but don't store value
    UseOptionals::DoPerfTest(c_Iterations, 0.5);
    errorCodeOutputs.push_back(UseOptionals::DoPerfTest(c_Iterations, 0.0));
    errorCodeOutputs.push_back(UseOptionals::DoPerfTest(c_Iterations, 0.01));
    errorCodeOutputs.push_back(UseOptionals::DoPerfTest(c_Iterations, 0.1));
    errorCodeOutputs.push_back(UseOptionals::DoPerfTest(c_Iterations, 1.0));

    std::cout << "Iterations: " << c_Iterations << "\n\n";

    std::cout << "\n----- Exceptions -----\n";
    std::cout << "Failure rate          Result         Total Time      Per Iteration\n";
    for (const AlgorithmOutput& output : exceptionOutputs)
    {
        std::cout << std::fixed << std::setprecision(2) << std::setw(12) << output.FailureRate;
        std::cout << std::setw(16) << output.Result;
        std::cout << std::setw(16) << std::chrono::duration_cast<std::chrono::microseconds>(output.Elapsed).count() << " us";
        std::cout << std::setw(16) << (output.Elapsed.count() / c_Iterations) << " ns";
        std::cout << "\n";
    }

    std::cout << "\n----- Optionals -----\n";
    std::cout << "Failure rate          Result         Total Time      Per Iteration\n";
    for (const AlgorithmOutput& output : errorCodeOutputs)
    {
        std::cout << std::fixed << std::setprecision(2) << std::setw(12) << output.FailureRate;
        std::cout << std::setw(16) << output.Result;
        std::cout << std::setw(16) << std::chrono::duration_cast<std::chrono::microseconds>(output.Elapsed).count() << " us";
        std::cout << std::setw(16) << (output.Elapsed.count() / c_Iterations) << " ns";
        std::cout << "\n";
    }

    std::cout << "\nException time / Optionals time: "
              << exceptionOutputs[exceptionOutputs.size() - 1].Elapsed.count()
                     / errorCodeOutputs[errorCodeOutputs.size() - 1].Elapsed.count()
              << "\n\n";
}

void DoErrorHandlingTest()
{
    using namespace ErrorHandlingTest;

    // Warmup pass
    (void)UseExceptions::DoWork(1, false);
    (void)UseOptionals::DoWork(1, false);
    (void)UseExceptions::DoWork(1, true);
    (void)UseOptionals::DoWork(1, true);

    // Real pass
    AlgorithmOutput exoutput = UseExceptions::DoWork(c_Iterations, false);
    AlgorithmOutput ecoutput = UseOptionals::DoWork(c_Iterations, false);
    AlgorithmOutput exerroutput = UseExceptions::DoWork(c_Iterations, true);
    AlgorithmOutput ecerroutput = UseOptionals::DoWork(c_Iterations, true);

    std::cout << "\n\n=============================================================\n";
    std::cout << exoutput.Text << "\n";
    std::cout << "\n\n=============================================================\n";
    std::cout << ecoutput.Text << "\n";
    std::cout << "\n\n=============================================================\n";
    std::cout << exerroutput.Text << "\n";
    std::cout << "\n\n=============================================================\n";
    std::cout << ecerroutput.Text << "\n";
    std::cout << "\n\n";
    std::cout << "Iterations run: " << c_Iterations << "\n";
    std::cout << "time elapsed - exceptions  (no error inserted): " << exoutput.Elapsed.count() << " ms\n";
    std::cout << "time elapsed - error codes (no error inserted): " << ecoutput.Elapsed.count() << " ms\n";
    std::cout << "time elapsed - exceptions     (error inserted): " << exerroutput.Elapsed.count() << " ms\n";
    std::cout << "time elapsed - error codes    (error inserted): " << ecerroutput.Elapsed.count() << " ms\n";
}

void DoBadExceptionsExample()
{
    using namespace BadExcceptionsExample;

    std::unordered_map<int, int> map;
    map[1] = 1;
    map[2] = 2;
    map[3] = 3;

    DoubleEntry_Worser(map, 5);
    DoubleEntry_Gooder(map, 5);
}

int main()
{
    std::srand(static_cast<unsigned int>(std::time(0))); // use current time as seed for random generator

    DoPerfTest();
    // DoBadExceptionsExample();
    // DoErrorHandlingTest();
}
