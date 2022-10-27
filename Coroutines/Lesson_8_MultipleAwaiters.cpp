#include "Helpers.h"

#include <coroutine>
#include <functional>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

namespace Lesson_8_MultipleAwaiters {

    // ============================================================================
    // This example demonstrates multiple coroutines waiting on the same state and
    // resuming in response to the same event.
    //
    // Note that the coroutines are not running in parallel in a multithreaded
    // environment. They are created on the same thread, resume in response to
    // the same event, and run sequentially.
    // ============================================================================

    // ----------------------------------------------------------------------------
    // Type which tracks whether something was found or not.
    // It is not thread safe, but this lesson is single-threaded.
    // ----------------------------------------------------------------------------
    struct FoundStatus
    {
        bool WasFound()
        {
            return m_wasFound;
        }

        void AddFoundHandler(std::function<void()> callback)
        {
            m_callbacks.emplace_back(std::move(callback));
        }

        void SetWasFound()
        {
            m_wasFound = true;
            for (const std::function<void()>& callback : m_callbacks)
            {
                callback();
            }
        }

        void Reset()
        {
            m_wasFound = false;
            m_callbacks.clear();
        }

        bool                               m_wasFound = false;
        std::vector<std::function<void()>> m_callbacks;
    };

    FoundStatus s_foundStatus;

    // ----------------------------------------------------------------------------
    // Awaiter object manages suspension and resumption of the coroutine
    // ----------------------------------------------------------------------------
    struct WasFoundAwaiter
    {
        WasFoundAwaiter(std::string instancePrefix) : m_instancePrefix(instancePrefix)
        {
            std::cout << m_instancePrefix << "WasFoundAwaiter::WasFoundAwaiter()\n";
        }

        ~WasFoundAwaiter()
        {
            std::cout << m_instancePrefix << "WasFoundAwaiter::~WasFoundAwaiter()\n";
        }

        bool await_ready() const
        {
            bool wasFound = s_foundStatus.WasFound();
            std::cout << m_instancePrefix << "WasFoundAwaiter::await_ready() (" << wasFound << ")\n";
            return wasFound;
        }

        void await_suspend(std::coroutine_handle<> handle) const
        {
            std::cout << m_instancePrefix << "WasFoundAwaiter::await_suspend()\n";
            s_foundStatus.AddFoundHandler(
                [handle, instancePrefix = m_instancePrefix]()
                {
                    std::cout << instancePrefix << "WasFoundAwaiter WasFoundHandler callback\n";
                    handle.resume();
                });
        }

        void await_resume() const
        {
            std::cout << m_instancePrefix << "WasFoundAwaiter::await_resume()\n";
        }

        std::string m_instancePrefix;
    };

    // ----------------------------------------------------------------------------
    // Return value type for a coroutine
    // ----------------------------------------------------------------------------
    struct CoroVoid
    {
        struct promise_type;

        CoroVoid(std::string instancePrefix) : m_instancePrefix(instancePrefix)
        {
            std::cout << m_instancePrefix << "CoroVoid::CoroVoid()\n";
        }

        CoroVoid(CoroVoid&& other) : m_instancePrefix(std::move(other.m_instancePrefix))
        {
        }

        ~CoroVoid() noexcept
        {
            ++m_destructedCount;

            // if m_instancePrefix is empty then a move-constructor stole the data
            // so don't log that the return value is being destroyed; it's just been moved.
            if (!m_instancePrefix.empty())
            {
                std::cout << m_instancePrefix << "CoroVoid::~CoroVoid()\n";
            }
        }

        struct promise_type
        {
            std::string m_instancePrefix {GetNextInstancePrefix()};

            promise_type()
            {
                std::cout << m_instancePrefix << "CoroVoid::promise_type::promise_type()\n";
            }

            ~promise_type() noexcept
            {
                std::cout << m_instancePrefix << "CoroVoid::promise_type::~promise_type()\n";
            }

            CoroVoid get_return_object()
            {
                std::cout << m_instancePrefix << "CoroVoid::promise_type::get_return_object()\n";
                return {m_instancePrefix};
            }

            WasFoundAwaiter initial_suspend()
            {
                std::cout << m_instancePrefix << "CoroVoid::promise_type::initial_suspend()\n";
                return {m_instancePrefix};
            }

            std::suspend_never final_suspend() noexcept
            {
                std::cout << m_instancePrefix << "CoroVoid::promise_type::final_suspend()\n";
                return {};
            }

            void return_void()
            {
                std::cout << m_instancePrefix << "CoroVoid::promise_type::return_void()\n";
            }

            void unhandled_exception()
            {
                std::cout << m_instancePrefix << "CoroVoid::promise_type::unhandled_exception()\n";
            }

            inline static size_t s_NextInstanceId = 1;

            static std::string GetNextInstancePrefix()
            {
                size_t nextInstanceId = s_NextInstanceId++;

                std::stringstream ss;
                ss << std::string(nextInstanceId * 50, ' ');
                ss << nextInstanceId << ": ";
                return ss.str();
            }

            static void Reset()
            {
                s_NextInstanceId = 1;
            }
        };

        std::string m_instancePrefix;
        size_t      m_destructedCount = 0;
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
        CoroVoid::promise_type::Reset();

        if (startState == StartState::Running)
            s_foundStatus.SetWasFound();

        {
            std::vector<CoroVoid> coroRetvals;
            for (size_t index = 0; index < 3; index++)
            {
                coroRetvals.emplace_back(DoCoroutine());
            }

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

} // namespace Lesson_8_MultipleAwaiters