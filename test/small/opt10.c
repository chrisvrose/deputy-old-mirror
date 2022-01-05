struct list_head {
    struct list_head *next;
};
struct signal_struct {
    struct list_head cpu_timers[3];
};
void foo(struct signal_struct *sig) {
    (&sig->cpu_timers[1])->next = &sig->cpu_timers[1];
}
int main() {
    struct signal_struct s;
    foo(&s);
    return 0;
}
