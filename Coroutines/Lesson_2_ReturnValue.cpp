#include <coroutine>
#include <iostream>

namespace Lesson_2_ReturnValue {

    // ============================================================================
    // This example demonstrates returning a value from the coroutine to the caller
    // 
    // Note that when co_return is called, the value is passed to
    // promise_type::return_value, where it stores the value in the return value
    // object for retrieval later.
    // 
    // The parameter type for promise_type::return_value must be convertible from
    // the type returned by co_return or a compiler error will occur.
    // ============================================================================

    // ----------------------------------------------------------------------------
    // Return value type for a coroutine
    // ----------------------------------------------------------------------------
    struct CoroInt
    {
        struct promise_type;

        CoroInt(promise_type& promise) : m_promise(promise)
        {
            std::cout << "CoroInt::CoroInt()\n";
            m_promise.m_pValue = &m_value;
        }

        ~CoroInt() noexcept
        {
            std::cout << "CoroInt::~CoroInt()\n";
        }

        struct promise_type
        {
            promise_type()
            {
                std::cout << "CoroInt::promise_type::promise_type()\n";
            }

            ~promise_type() noexcept
            {
                std::cout << "CoroInt::promise_type::~promise_type()\n";
            }

            CoroInt get_return_object()
            {
                std::cout << "CoroInt::promise_type::get_return_object()\n";
                return {*this};
            }

            std::suspend_never initial_suspend()
            {
                std::cout << "CoroInt::promise_type::initial_suspend()\n";
                return {};
            }

            std::suspend_never final_suspend() noexcept
            {
                std::cout << "CoroInt::promise_type::final_suspend()\n";
                return {};
            }

            void return_value(int value)
            {
                std::cout << "CoroInt::promise_type::return_value(" << value << ")\n";
                *m_pValue = value;
            }

            void unhandled_exception()
            {
                std::cout << "CoroInt::promise_type::unhandled_exception()\n";
            }

            int* m_pValue = nullptr;
        };

        promise_type& m_promise;
        int           m_value;
    };

    // ----------------------------------------------------------------------------
    // Coroutine function
    // ----------------------------------------------------------------------------
    CoroInt DoCoroutine(int value)
    {
        std::cout << "DoCoroutine(" << value << ")\n";
        co_return value* value;
    }

    // ----------------------------------------------------------------------------
    // Entry point for the lesson code
    // ----------------------------------------------------------------------------
    void DoLesson()
    {
        std::cout << "\ncalling coroutine\n";

        {
            CoroInt coroRetval = DoCoroutine(5);
            std::cout << "return value: " << coroRetval.m_value << "\n";
            std::cout << "Falling out of scope\n";
        }

        std::cout << "done with coroutine\n";
    }

} // namespace Lesson_2_ReturnValue