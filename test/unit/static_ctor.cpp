#include <cstdlib>

static int global_int = 31;

struct side_effect
{
    side_effect()
    {
        global_int = 42;
    }
};

static const side_effect se;

int main()
{
    return global_int == 42 ? EXIT_SUCCESS : EXIT_FAILURE;
}