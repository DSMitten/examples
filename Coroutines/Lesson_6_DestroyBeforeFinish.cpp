#include "Helpers.h"

#include <coroutine>
#include <iostream>

namespace Lesson_5_DestroyBeforeFinish {

    // ============================================================================
    // This example demonstrates calling coroutine_handle::destroy before the
    // coroutine has finished executing.
    //
    // A coroutine can only be destroyed when it is in a suspended state.
    //
    // If it is destroyed, its promise_type and context objects are destroyed and
    // the coroutine will not finish executing. Any class or struct variables in
    // the coroutine that have been initialized will run their destructors.
    // ============================================================================

    // ----------------------------------------------------------------------------
    // Type that will be constructed & destructed in the coroutine
    // ----------------------------------------------------------------------------
    struct SomeStruct
    {
        SomeStruct(size_t index) : m_index(index)
        {
            std::cout << "SomeStruct::SomeStruct() #" << m_index << "\n";
        }

        ~SomeStruct()
        {
            std::cout << "SomeStruct::~SomeStruct() #" << m_index << "\n";
        }

        size_t m_index;
    };

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
        size_t stepIndex = 0;

        SomeStruct someStruct1 {1};
        std::cout << "Coroutine step " << ++stepIndex << "\n";
        co_await std::suspend_always {};

        SomeStruct someStruct2 {2};
        std::cout << "Coroutine step " << ++stepIndex << "\n";
        co_await std::suspend_always {};

        SomeStruct someStruct3 {3};
        std::cout << "Coroutine step " << ++stepIndex << "\n";
        co_await std::suspend_always {};

        SomeStruct someStruct4 {4};
        std::cout << "Exiting coroutine\n";
    }

    // ----------------------------------------------------------------------------
    // Lesson implementation
    // ----------------------------------------------------------------------------
    void DoLessonInner(bool exitEarly)
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
            std::cout << "Resuming coroutine\n";
            handle.resume();

            if (!exitEarly)
            {
                WriteCoroutineState(handle);
                std::cout << "Resuming coroutine\n";
                handle.resume();

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

    // ----------------------------------------------------------------------------
    // Entry point for the lesson code
    // ----------------------------------------------------------------------------
    void DoLesson()
    {
        DoLessonInner(false);
        DoLessonInner(true);
    }

} // namespace Lesson_5_DestroyBeforeFinish