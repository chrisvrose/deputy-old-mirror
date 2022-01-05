extern char (COUNT(4) extern1buf)[];

int main() {
    return (extern1buf[1] == 'a');
}
