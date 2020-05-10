int main() {
    try {
        throw 42;

    }
    catch (int const &i) {
        if (i == 42) {
            return 0;
        }
    }

    return -1;
}