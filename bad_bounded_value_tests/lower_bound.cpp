#include <houseguest/bounded_value.hpp>

int main()
{
    houseguest::bounded_value<int, -100, 0> negative{-52};
    houseguest::bounded_value<int, 1, 100> positive{negative};

    return 0;
}
