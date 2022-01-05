// Make sure that we don't spread function dependencies to arguments.

void foo(char * COUNT(len) buf, int len) {
}

int main(void) 
{
    void *tmp = 0;
    foo(tmp, 5);
    return 0;
}
