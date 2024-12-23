int res[5] = {0, 1, 2, 3, 4};
int idx = 0;

int sub(int a, int b) {
    if (a > b) {
        return a - b;
    } else {
        return b - a;
    }
}
