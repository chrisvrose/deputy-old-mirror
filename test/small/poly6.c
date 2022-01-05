struct notifier_block {
    int (*callback)(TV(t));
    TV(t) data;
};

struct notifier_block TP(char * NTS) * reboot_notifier_list;

int notifier_chain_register(struct notifier_block TP(TV(t)) ** list,
                            struct notifier_block TP(TV(t)) * nb) {
    return 0;
}

int register_reboot_notifier(struct notifier_block TP(char * NTS) * nb)
{
    return notifier_chain_register(&reboot_notifier_list, nb);
}

int main() {
    struct notifier_block TP(char * NTS) nb;
    return register_reboot_notifier(&nb);
}
