#include "Helpers.h"

#include <coroutine>
#include <functional>
#include <iostream>

namespace Lesson_9_Awaitable {

    // ============================================================================
    // This example demonstrates implementing an awaitable object that uses an
    // awaiter to suspend and resume the coroutine when its data becomes available.
    //
    // An awaiter object is an object that implements an operator named co_await()
    // which returns the awaiter that actually implements the await support.
    //
    // The co_await() operator is like the initial_suspend() and final_suspend()
    // functions on promise_type. All of these return an awaiter which knows how
    // to determine whether the coroutine should be suspended or not, and handles
    // the suspension process.
    //
    // Separating the awaiter implementation from the awaitable object allows the
    // awaitable to return different awaiters depending on the situation.
    // ============================================================================

    // ----------------------------------------------------------------------------
    // Awaitable object with async state notifications.
    //
    // Is awaitable because it implements the co_await operator.
    // ----------------------------------------------------------------------------
    struct FoundStatus
    {
        bool WasFound()
        {
            return m_wasFound;
        }

        void SetFoundHandler(std::function<void()> callback)
        {
            m_callback = std::move(callback);
        }

        void SetWasFound()
        {
            m_wasFound = true;
            if (m_callback)
            {
                m_callback();
            }
        }

        static FoundStatus& Instance()
        {
            static FoundStatus instance;
            return instance;
        }

        static void Reset()
        {
            Instance().m_wasFound = false;
            Instance().m_callback = nullptr;
        }

        bool                  m_wasFound = false;
        std::function<void()> m_callback;

        // ----------------------------------------------------------------------------
        // Awaiter object manages suspension and resumption of the coroutine
        // ----------------------------------------------------------------------------
        struct WasFoundAwaiter
        {
            bool await_ready() const
            {
                bool wasFound = Instance().WasFound();
                std::cout << "WasFoundAwaiter::await_ready() (" << wasFound << ")\n";
                return wasFound;
            }

            void await_suspend(std::coroutine_handle<> handle) const
            {
                std::cout << "WasFoundAwaiter::await_suspend()\n";
                Instance().SetFoundHandler(
                    [handle]()
                    {
                        std::cout << "WasFoundAwaiter WasFoundHandler callback\n";
                        handle.resume();
                    });
            }

            void await_resume() const noexcept
            {
                std::cout << "WasFoundAwaiter::await_resume()\n";
            }
        };

        WasFoundAwaiter m_awaiter;

        WasFoundAwaiter operator co_await()
        {
            std::cout << "FoundStatus::operator co_await()\n";
            return m_awaiter;
        }
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
    CoroVoid DoCoroutine()
    {
        std::cout << "DoCoroutine()\n";

        std::cout << "Awaiting FoundStatus\n";

        co_await FoundStatus::Instance();

        co_return;
    }

    // ----------------------------------------------------------------------------
    // Lesson implementation
    // ----------------------------------------------------------------------------
    enum StartState
    {
        Suspended,
        Running
    };

    void DoLessonInner(StartState startState)
    {
        std::cout << "\ncalling coroutine\n";

        FoundStatus::Instance().Reset();
        if (startState == StartState::Running)
            FoundStatus::Instance().SetWasFound();

        {
            CoroVoid coroRetval = DoCoroutine();

            if (startState == StartState::Suspended)
                FoundStatus::Instance().SetWasFound();

            std::cout << "Falling out of scope\n";
        }

        std::cout << "done with coroutine\n";
    }

    // ----------------------------------------------------------------------------
    // Entry point for the lesson code
    // ----------------------------------------------------------------------------
    void DoLesson()
    {
        DoLessonInner(StartState::Running);
        DoLessonInner(StartState::Suspended);
    }

} // namespace Lesson_9_Awaitable