# include "ps.h"

long get_current_time() {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return ts.tv_sec;
}

void exit_terminal(pid_t pid) {
    if (pid > 0) {
        if (kill(pid, SIGTERM) == 0) {
            sleep(1);
            if (kill(pid, 0) == 0) {
                kill(pid, SIGKILL);
            }
        }
    }
}