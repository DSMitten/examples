#include <coroutine>
#include <iostream>
#include <memory>

namespace Lesson_1_JustReturn {

    // ============================================================================
    // This example contains a minimal coroutine function.
    // All it does is call co_return.
    //
    // Every coroutine return value must have a promise_type nested class or struct
    // Every promise_type must have a set of functions:
    //       get_return_value: creates the outer return value class
    //        initial_suspend: gives the opportunity to start the coroutine suspended
    //          final_suspend: gives the opportunity to hold onto the coroutine after
    //                         it finished
    //    unhandled_exception: called if the coroutine throws
    //
    // Every promise_type must have one of these:
    //            return_void: called for co_return without a value
    //           return_value: called for co_return with a value
    //            yield_value: called for co_yield
    //
    // A promise_type *may* have these optional functions:
    //        await_transform: supports custom co_await logic
    // ============================================================================

    // ----------------------------------------------------------------------------
    // Return value type for a coroutine
    // ----------------------------------------------------------------------------
    struct CoroVoid
    {
        CoroVoid()
        {
            std::cout << "CoroVoid::CoroVoid()\n";
        }

        ~CoroVoid() noexcept
        {
            std::cout << "CoroVoid::~CoroVoid()\n";
        }

        // Required nested type under the return value type
        struct promise_type
        {
            promise_type()
            {
                std::cout << "CoroVoid::promise_type::promise_type()\n";
            }

            ~promise_type() noexcept
            {
                std::cout << "CoroVoid::promise_type::~promise_type()\n";
            }

            CoroVoid get_return_object()
            {
                std::cout << "CoroVoid::promise_type::get_return_object()\n";
                return {};
            }

            std::suspend_never initial_suspend()
            {
                std::cout << "CoroVoid::promise_type::initial_suspend()\n";
                return {};
            }

            std::suspend_never final_suspend() noexcept
            {
                std::cout << "CoroVoid::promise_type::final_suspend()\n";
                return {};
            }

            void return_void()
            {
                std::cout << "CoroVoid::promise_type::return_void()\n";
            }

            void unhandled_exception()
            {
                std::cout << "CoroVoid::promise_type::unhandled_exception()\n";
            }
        };
    };

    // ----------------------------------------------------------------------------
    // Coroutine function
    // ----------------------------------------------------------------------------
    CoroVoid DoCoroutine()
    {
        std::cout << "DoCoroutine()\n";
        co_return;
    }

    // ----------------------------------------------------------------------------
    // Entry point for the lesson code
    // ----------------------------------------------------------------------------
    void DoLesson()
    {
        std::cout << "\ncalling coroutine\n";

        {
            CoroVoid coroRetval = DoCoroutine();
            std::cout << "Falling out of scope\n";
        }

        std::cout << "done with coroutine\n";
    }

} // namespace Lesson_1_JustReturn