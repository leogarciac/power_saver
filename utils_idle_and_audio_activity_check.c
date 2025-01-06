#include "idle_and_audio_activity_check.h"

unsigned int get_idle_time(Display *display) {
    XScreenSaverInfo *info;
    info = XScreenSaverAllocInfo();
    if (info != NULL) {
        XScreenSaverQueryInfo(display, DefaultRootWindow(display), info);
        unsigned int idle_time = info->idle;
        XFree(info);
        return idle_time;
    }
    return 0;
}

void adjust_brightness(int restore) {
    if (restore) {
        printf("Restoring brightness to 100%%.\n");
        if (system("brightnessctl set 100%") != 0) {
            printf("Failed to restore brightness.\n");
        }
    } else {
        printf("Reducing brightness due to inactivity.\n");
        if (system("brightnessctl set 30%") != 0) {
            printf("Failed to reduce brightness.\n");
        }
    }
}