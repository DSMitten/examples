#include "Helpers.h"

#include <coroutine>
#include <functional>
#include <iostream>
#include <string>
#include <string_view>

namespace Lesson_11_Pitfall {

    // ============================================================================
    // This example demonstrates that it is important to ensure that data
    // referenced by a coroutine outlives the coroutine itself.
    // ============================================================================

    // ----------------------------------------------------------------------------
    // Simple observable type which calls the registered callback when it changes.
    // ----------------------------------------------------------------------------
    struct Signal
    {
        void Set()
        {
            std::coroutine_handle<> handle = m_awaiter.m_handle;
            m_awaiter.m_handle = nullptr;
            handle.resume();
        }

        struct SignalAwaiter
        {
            bool await_ready() const
            {
                return false;
            }

            void await_suspend(std::coroutine_handle<> handle)
            {
                m_handle = handle;
            }

            void await_resume() const
            {
            }

            std::coroutine_handle<> m_handle;
        };

        SignalAwaiter m_awaiter;

        SignalAwaiter& operator co_await()
        {
            return m_awaiter;
        }
    };

    Signal s_signal;

    // ----------------------------------------------------------------------------
    // Return value type for a coroutine
    // ----------------------------------------------------------------------------
    struct CoroVoid
    {
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
                return {};
            }

            std::suspend_never initial_suspend()
            {
                return {};
            }

            std::suspend_never final_suspend() noexcept
            {
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
    CoroVoid DoCoroutine(std::string_view id)
    {
        std::cout << "DoCoroutine(" << id << ")\n";

        std::cout << "Awaiting step 2 for" << id << "\n";
        co_await s_signal;

        std::cout << "Awaiting step 3 for " << id << "\n";
        co_await s_signal;

        std::cout << "Exiting\n";
    }

    // ----------------------------------------------------------------------------
    // Entry point for the lesson code
    // ----------------------------------------------------------------------------
    void DoLesson()
    {
        std::cout << "\ncalling coroutine\n";

        {
            CoroVoid coroRetval;

            {
                std::string id {"D799569E-F718-4686-A261-0185306D89A8"};
                coroRetval = DoCoroutine(id);
            }

            std::cout << "Signal step 2\n";
            s_signal.Set();

            std::cout << "Signal step 3\n";
            s_signal.Set();

            std::cout << "Falling out of scope\n";
        }

        std::cout << "done with coroutine\n";
    }

} // namespace Lesson_11_Pitfall