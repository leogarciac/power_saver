#include "dynamic_backlight_control.h"

int directory_exists(const char *path) {
    struct stat info;
    return (stat(path, &info) == 0 && S_ISDIR(info.st_mode));
}