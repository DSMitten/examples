# Overview

This project demonstrates two different approaches to error handling: throwing exceptions and returning null/empty values.

It contains three tests:

* A performance test, implemented in PerfTest.cpp. This is to measure the cost of throwing exceptions vs returning `std::nullopt` in error cases.
* An error handling test (demonstration), implemented in ErrorHandlingTest_*. This is to highlight the way code should look when using exceptions and illustrate the benefits provided by using exceptions for error handling.
* An example of using exceptions when a non-throwing function would be more appropriate. This is implemented in BadExceptionExample.cpp. In this case, because the function itself intends to handle the error case, it is better to call a non-throwing function than to call a throwing one.

For the perf measurements to be usable, be sure to build the Release config. Debug config omits optimizations and introduces overhead which can affect performance numbers.