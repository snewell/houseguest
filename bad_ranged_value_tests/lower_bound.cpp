#include <houseguest/ranged_value.hpp>

int main()
{
    houseguest::ranged_value<int, -100, 0> negative{-52};
    houseguest::ranged_value<int, 1, 100> positive{negative};

    return 0;
}
