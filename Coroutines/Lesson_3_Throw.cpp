#include <coroutine>
#include <exception>
#include <iostream>
#include <random>

namespace Lesson_3_Throw {

    // ============================================================================
    // Returns a value
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
            m_promise.m_pExceptionPtr = &m_exceptionPtr;
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
                *m_pExceptionPtr = std::current_exception();
            }

            std::exception_ptr* m_pExceptionPtr = nullptr;
        };

        void Get()
        {
            std::rethrow_exception(m_exceptionPtr);
        }

        promise_type&      m_promise;
        std::exception_ptr m_exceptionPtr;
    };

    // ----------------------------------------------------------------------------
    // Coroutine function
    // ----------------------------------------------------------------------------
    CoroVoid DoCoroutine(bool shouldThrow)
    {
        std::cout << "DoCoroutine()\n";
        if (shouldThrow)
        {
            throw std::runtime_error("huh, this is wierd");
        }

        co_return;
    }

    // ----------------------------------------------------------------------------
    // Entry point for the lesson code
    // ----------------------------------------------------------------------------
    void DoLesson()
    {
        std::cout << "\ncalling coroutine\n";

        {
            CoroVoid coroRetval = DoCoroutine(rand() % 2 == 1);

            try
            {
                coroRetval.Get();
            }
            catch (std::runtime_error ex)
            {
                std::cout << "runtime_error: " << ex.what() << "\n";
            }

            std::cout << "Falling out of scope\n";
        }

        std::cout << "done with coroutine\n";
    }

} // namespace Lesson_3_Throw