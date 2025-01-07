#ifndef PS_H
# define PS_H

#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <libevdev-1.0/libevdev/libevdev.h>
#include <poll.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>
#include "dynamic_backlight_control.h"
#include "idle_and_audio_activity_check.h"


#define TIMEOUT 5
#define INPUT_DIR "/dev/input"
#define MAX_RETRIES 5

long get_current_time();
void exit_terminal(pid_t pid);
void set_brightness_dynamically(int brightness_percentage);

#endif