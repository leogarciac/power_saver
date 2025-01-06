#ifndef IDLE_AND_AUDIO_ACTIVITY_CHECK
# define IDLE_AND_AUDIO_ACTIVITY_CHECK

#define IDLE_TIMEOUT 6  // 6 milliseconds
#define AUDIO_THRESHOLD 100  // Threshold for audio activity detection

#include <stdio.h>
#include <stdlib.h>
#include <X11/extensions/scrnsaver.h>
#include <unistd.h>
#include <string.h>
#include <pulse/simple.h>
#include <pulse/error.h>

unsigned int    get_idle_time(Display *display);
void            adjust_brightness(int restore);

#endif