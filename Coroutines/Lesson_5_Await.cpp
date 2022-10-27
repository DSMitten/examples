#include "Helpers.h"

#include <coroutine>
#include <iostream>

namespace Lesson_5_Await {

    // ============================================================================
    // This example demonstrates calling an awaiter object directly with co_await.
    // A co_await function makes a function a coroutine even if it doesn't have
    // a co_return instruction.
    //
    // The system has already been calling co_await initial_suspend() and
    // co_await final_suspend(), but it did so behind the scenes and we didn't
    // examine what suspend_always or suspend_never do.
    //
    // Now our coroutine will call co_await on both of these.
    // As you might expect:
    //  * suspend_never will just return. It won't suspend the coroutine.
    //  * suspend_always will unconditionally suspend the coroutine.
    //
    // When we co_await suspend_always, control will return to the function that
    // that last resumed the coroutine. It will remain suspended until the next
    // call to resume.
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
            m_promise.m_pFinished = &m_finished;
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
                *m_pFinished = true;
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

            bool* m_pFinished = nullptr;
        };

        std::coroutine_handle<promise_type> GetHandle()
        {
            return std::coroutine_handle<promise_type>::from_promise(m_promise);
        }

        promise_type& m_promise;
        bool          m_finished = false;
    };

    // ----------------------------------------------------------------------------
    // Coroutine function
    // ----------------------------------------------------------------------------
    CoroVoid DoCoroutine()
    {
        size_t stepIndex = 0;
        std::cout << "Coroutine step " << ++stepIndex << "\n";
        co_await std::suspend_always {};

        std::cout << "Coroutine step " << ++stepIndex << "\n";
        co_await std::suspend_always {};

        std::cout << "Coroutine step " << ++stepIndex << "\n";
        co_await std::suspend_never {};
        std::cout << "Coroutine step " << ++stepIndex << "\n";
        co_await std::suspend_never {};
        std::cout << "Coroutine step " << ++stepIndex << "\n";
        co_await std::suspend_always {};

        std::cout << "Coroutine step " << ++stepIndex << "\n";
        co_await std::suspend_always {};

        std::cout << "Exiting coroutine\n";
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

            while (!coroRetval.m_finished)
            {
                WriteCoroutineState(handle);
                std::cout << "Resuming coroutine\n";
                handle.resume();
            }

            WriteCoroutineState(handle);
            std::cout << "Destroying coroutine\n";
            handle.destroy();

            WriteCoroutineState(handle);
            std::cout << "Falling out of scope\n";
        }

        std::cout << "done with coroutine\n";
    }

} // namespace Lesson_5_Await