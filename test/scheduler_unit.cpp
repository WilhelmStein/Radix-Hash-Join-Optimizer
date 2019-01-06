
#include <scheduler.hpp>
#include <iostream>
#include <unistd.h>

void * foo(void * args)
{
    int sum = *(int *)args;
    for (volatile std::size_t i = 0UL; i < 10000UL; i++)
        sum++;

    std::cout << "sum = " << sum << std::endl;

    return nullptr;
}

static struct Boo
{
    const char * name;
    const char * surname;
    const char * age;
    
} boo = { "Bill", "Sioros", "21" };

void * bar(void * args)
{
    Boo * boo = (Boo *)args;

    std::cout << boo->name << " " << boo->surname << " " << boo->age << std::endl;
    
    usleep(1000000);

    return nullptr;
}

int main()
{
    int values[] = { 0, 1, 2, 3, 4, 5 }; int i = 0;

    thread_pool::create(3UL);

    thread_pool::schedule(foo, (void *)&values[i]); i++;
    thread_pool::schedule(foo, (void *)&values[i]); i++;
    thread_pool::schedule(foo, (void *)&values[i]); i++;
    thread_pool::schedule(foo, (void *)&values[i]); i++;
    thread_pool::schedule(foo, (void *)&values[i]); i++;
    thread_pool::schedule(foo, (void *)&values[i]); i++;

    thread_pool::schedule(bar, (void *)&boo);

    thread_pool::block();

    thread_pool::destroy();

    return 0;
}
