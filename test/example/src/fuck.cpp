#include <thread>
#include <iostream>
#include <unistd.h>

static void foo()
{
    std::cout << "foo" << std::endl;
}

static struct bar
{
    bar()
    {
        std::cout << "start" << std::endl;
        std::thread ctx(foo);
        std::cout << "end" << std::endl;
        ctx.detach();
    }
} _;

int main()
{
    std::cout << "main" << std::endl;
}