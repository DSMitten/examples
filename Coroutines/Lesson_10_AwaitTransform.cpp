#include "Helpers.h"

#include <coroutine>
#include <functional>
#include <future>
#include <iostream>

namespace Lesson_10_AwaitTransform {

    // ============================================================================
    // This example demonstrates implementing the await_transform function on the
    // promise_type which allows the promise_type to specify its own awaiter for
    // a co_await instruction.
    //
    // This function provides several customization abilities:
    //   * It allows types to be co_await'ed that normally would not be, by
    //     the promise_type supplying the awaiter for those types
    //   * It allows normal awaiters to be replaced or wrapped with an awaiter
    //     chosen by the promise_type
    //   * It allows the promise_type to prevent co_await calls for an object by
    //     deleting the await_transform function for that type.
    // ============================================================================

    // ----------------------------------------------------------------------------
    // Simple type which calls the registered callback when it is signaled.
    // ----------------------------------------------------------------------------
    class Signal
    {
    public:
        void Set()
        {
            if (m_signaled)
            {
                m_signaled();
            }
        }

        std::function<void()> m_signaled;
    };

    // ----------------------------------------------------------------------------
    // Awaiter object manages suspension and resumption of the coroutine
    //
    // Suspends the coroutine and then resumes it when the Signal object signals.
    // ----------------------------------------------------------------------------
    struct SignalAwaiter
    {
        SignalAwaiter(Signal& signal) : m_signal(signal)
        {
            std::cout << "SignalAwaiter::SignalAwaiter()\n";
        }

        ~SignalAwaiter()
        {
            std::cout << "SignalAwaiter::~SignalAwaiter()\n";
        }

        bool await_ready() const
        {
            std::cout << "SignalAwaiter::await_ready()\n";
            return false;
        }

        void await_suspend(std::coroutine_handle<> handle) const
        {
            std::cout << "SignalAwaiter::await_suspend()\n";
            m_signal.m_signaled = [handle]
            {
                std::cout << "Signal callback\n";
                handle.resume();
            };
        }

        void await_resume() const
        {
            std::cout << "SignalAwaiter::await_resume()\n";
            m_signal.m_signaled = nullptr;
        }

        Signal& m_signal;
    };

    // ----------------------------------------------------------------------------
    // Return value type for a coroutine
    // ----------------------------------------------------------------------------
    struct CoroVoid
    {
        struct promise_type;

        CoroVoid()
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

            SignalAwaiter await_transform(Signal& signal)
            {
                return SignalAwaiter(signal);
            }
        };
    };

    // ----------------------------------------------------------------------------
    // Coroutine function
    // ----------------------------------------------------------------------------
    class MyClass
    {
    public:
        CoroVoid DoCoroutine()
        {
            std::cout << "MyClass::DoCoroutine()\n";

            std::cout << "Waiting for some progress\n";
            co_await ProgressSignal;

            std::cout << "Waiting for some more progress\n";
            co_await ProgressSignal;

            std::cout << "Waiting for the last bit of progress\n";
            co_await ProgressSignal;

            std::cout << "MyClass::DoCoroutine is all done\n";
        }

        Signal ProgressSignal;
    };

    // ----------------------------------------------------------------------------
    // Entry point for the lesson code
    // ----------------------------------------------------------------------------
    void DoLesson()
    {
        std::cout << "\ncalling coroutine\n";

        {
            MyClass myClass;

            CoroVoid coroRetval = myClass.DoCoroutine();

            std::cout << "Reporting progress\n";
            myClass.ProgressSignal.Set();

            std::cout << "Reporting progress\n";
            myClass.ProgressSignal.Set();

            std::cout << "Reporting progress\n";
            myClass.ProgressSignal.Set();
            
            std::cout << "Falling out of scope\n";
        }

        std::cout << "done with coroutine\n";
    }

} // namespace Lesson_10_AwaitTransform