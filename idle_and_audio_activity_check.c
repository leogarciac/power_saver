#include "ps.h"

#define IDLE_TIMEOUT 6  // 6 milliseconds
#define AUDIO_THRESHOLD 100  // Threshold for audio activity detection

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

int is_audio_playing() {
    pa_simple *s = NULL;
    pa_sample_spec ss;
    pa_buffer_attr buf_attr;
    int audio_active = 0;
    ss.format = PA_SAMPLE_S16LE;
    ss.channels = 2;
    ss.rate = 48000;
    buf_attr.maxlength = (uint32_t)-1;
    buf_attr.tlength = (uint32_t)-1;
    buf_attr.prebuf = (uint32_t)-1;
    buf_attr.minreq = (uint32_t)-1;
    s = pa_simple_new(NULL, "power_saver", PA_STREAM_RECORD, NULL, "record", &ss, NULL, &buf_attr, NULL);
    if (s != NULL) {
        uint8_t buf[1024];
        if (pa_simple_read(s, buf, sizeof(buf), NULL) == 0) {
            int avg_level = 0;
            for (int i = 0; i < sizeof(buf); i++) {
                avg_level += buf[i];
            }
            avg_level /= sizeof(buf);
            audio_active = (avg_level > AUDIO_THRESHOLD) ? 1 : 0;
        }
        pa_simple_free(s);
    } else {
        printf("PulseAudio connection failed: %s\n", pa_strerror(pa_context_errno(NULL)));
    }
    return audio_active;
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

int main() {
    Display *display = XOpenDisplay(NULL);
    if (display == NULL) {
        printf("Unable to open X display\n");
        return 1;
    }
    unsigned int idle_time = 0;
    int audio_active = 0;
    int last_brightness = 100;
    while (1) {
        idle_time = get_idle_time(display);
        audio_active = is_audio_playing();
        printf("Audio Avg Level: %d\n", audio_active ? 1 : 0);
        printf("Idle time: %d ms, Audio Active: %d\n", idle_time, audio_active);

        if (idle_time >= IDLE_TIMEOUT && !audio_active) {
            printf("Condition met to reduce brightness.\n");
            if (last_brightness != 30) {
                adjust_brightness(0);
                last_brightness = 30;
            }
        } else {
            printf("Condition met to restore brightness.\n");
            if (last_brightness != 100) {
                adjust_brightness(1);
                last_brightness = 100;
            }
        }
        sleep(1);
    }
    XCloseDisplay(display);
    return 0;
}

