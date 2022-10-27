#include "Helpers.h"

#include <coroutine>
#include <functional>
#include <iostream>

namespace Lesson_7_Awaiter {

    // ============================================================================
    // This example demonstrates implementing a custom awaiter object which
    // controls suspension of a coroutine.
    //
    // Awaiter objects must have the following functions:
    //     await_ready: Called at the start of co_await. If it returns true, the
    //                  coroutine is not suspended and just continues execution.
    //   await_suspend: Called immediately after the coroutine is suspended.
    //    await_resume: Called immediately prior to the coroutine being resumed.
    //
    // In this example, the coroutine depends on an item having been found.
    // Before starting execution, initial_suspend will use WasFoundAwaiter to
    // check if an item was found before execution started. If it was, the
    // coroutine will run immediately.
    //
    // If it wasn't, the awaiter object will register a callback for when the
    // item is found, store the coroutine handle, and suspend the coroutine. In
    // the callback
    // ============================================================================

    // ----------------------------------------------------------------------------
    // Type which tracks whether something was found or not.
    // Allows listeners to register a callback for when the item was found.
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

        void Reset()
        {
            m_wasFound = false;
            m_callback = nullptr;
        }

        bool                  m_wasFound = false;
        std::function<void()> m_callback;
    };

    FoundStatus s_foundStatus;

    // ----------------------------------------------------------------------------
    // Awaiter object manages suspension and resumption of the coroutine
    // 
    // Allows the coroutine to continue if the FoundStatus object is already in the
    // desired status when await_ready check is called.
    // 
    // If not, the coroutine is suspended after registering a callback to be
    // notified when the state is met, and resumes coroutine in that callback.
    // ----------------------------------------------------------------------------
    struct WasFoundAwaiter
    {
        WasFoundAwaiter()
        {
            std::cout << "WasFoundAwaiter::WasFoundAwaiter()\n";
        }

        ~WasFoundAwaiter()
        {
            std::cout << "WasFoundAwaiter::WasFoundAwaiter()\n";
        }

        bool await_ready() const
        {
            bool wasFound = s_foundStatus.WasFound();
            std::cout << "WasFoundAwaiter::await_ready() (" << wasFound << ")\n";
            return wasFound;
        }

        void await_suspend(std::coroutine_handle<> handle) const
        {
            std::cout << "WasFoundAwaiter::await_suspend()\n";
            s_foundStatus.SetFoundHandler(
                [handle]()
                {
                    std::cout << "WasFoundAwaiter WasFoundHandler callback\n";
                    handle.resume();
                });
        }

        void await_resume() const
        {
            std::cout << "WasFoundAwaiter::await_resume()\n";
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
                std::cout << "CoroVoid::promise_type::get_return_object()\n";
                return {};
            }

            WasFoundAwaiter initial_suspend()
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

        s_foundStatus.Reset();
        if (startState == StartState::Running)
            s_foundStatus.SetWasFound();

        {
            CoroVoid coroRetval = DoCoroutine();

            if (startState == StartState::Suspended)
                s_foundStatus.SetWasFound();

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

} // namespace Lesson_7_Awaiter