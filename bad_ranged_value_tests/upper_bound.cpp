#include <houseguest/ranged_value.hpp>

int main()
{
    houseguest::ranged_value<int, 1, 100> positive{52};
    houseguest::ranged_value<int, -100, 0> negative{positive};

    return 0;
}
