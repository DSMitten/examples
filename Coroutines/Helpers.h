#pragma once

#include <coroutine>
#include <iostream>

inline void WriteCoroutineState(std::coroutine_handle<> handle)
{
    std::cout << "holding coroutine? " << static_cast<bool>(handle);

    if (handle)
    {
        std::cout << ", done? " << handle.done() << "\n";
    }
    else
    {
        std::cout << "\n";
    }
}
