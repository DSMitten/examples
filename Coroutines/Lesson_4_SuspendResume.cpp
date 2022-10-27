#include "Helpers.h"

#include <coroutine>
#include <iostream>

namespace Lesson_4_SuspendResume {

    // ============================================================================
    // This example demonstrates suspending & resuming the coroutine at the
    // start and end of the coroutine.
    //
    // The initial_suspend function returns an object that controls whether to
    // suspend the coroutine before it starts. This allows a coroutine to be
    // created in a suspended state, and start executing at a later time.
    //
    // The final_suspend function returns an object that controls whether
    // to suspend the coroutine after it finished but before it is destroyed.
    // This allows a coroutine's promise and memory to be accessed after it
    // finishes, and allows for manual control over destruction of the coroutine
    // objects.
    //
    // In this example, we first start using coroutine_handle.
    // coroutine_handle is the owner object for the coroutine. It is used to
    // check coroutine state, resume a suspended coroutine, and destroy the
    // coroutine objects.
    //
    // If final_suspend returns suspend_never, the coroutine handle is told to
    // destroy the coroutine before execution returns to the caller.
    //
    // If final_suspend returns suspend_always, the coroutine objects remain
    // allocated until a manual call to coroutine_handle::destroy is made.
    // ============================================================================

    // ----------------------------------------------------------------------------
    // Return value type for a coroutine
    // ----------------------------------------------------------------------------
    struct CoroVoid
    {
        struct promise_type;

        CoroVoid(promise_type& promise) : m_promise(promise)
        {
            std::cout << "CoroVoid::CoroVoid()\n";
        }

        ~CoroVoid() noexcept
        {
            std::cout << "CoroVoid::~CoroVoid()\n";
        }

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
                return {*this};
            }

            std::suspend_always initial_suspend()
            {
                std::cout << "CoroVoid::promise_type::initial_suspend()\n";
                return {};
            }

            std::suspend_always final_suspend() noexcept
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

        std::coroutine_handle<promise_type> GetHandle()
        {
            return std::coroutine_handle<promise_type>::from_promise(m_promise);
        }

        promise_type& m_promise;
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
            std::coroutine_handle<> handle {};
            WriteCoroutineState(handle);

            CoroVoid coroRetval = DoCoroutine();
            handle = coroRetval.GetHandle();

            WriteCoroutineState(handle);
            std::cout << "Resuming coroutine\n";
            handle.resume();

            WriteCoroutineState(handle);
            std::cout << "Destroying coroutine\n";
            handle.destroy();

            WriteCoroutineState(handle);
            std::cout << "Falling out of scope\n";
        }

        std::cout << "done with coroutine\n";
    }

} // namespace Lesson_4_SuspendResume