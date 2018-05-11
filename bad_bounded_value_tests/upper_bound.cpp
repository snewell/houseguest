#include <houseguest/bounded_value.hpp>

int main()
{
    houseguest::bounded_value<int, 1, 100> positive{52};
    houseguest::bounded_value<int, -100, 0> negative{positive};

    return 0;
}
