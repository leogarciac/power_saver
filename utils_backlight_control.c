#include "dynamic_backlight_control.h"

int directory_exists(const char *path) {
    struct stat info;
    return (stat(path, &info) == 0 && S_ISDIR(info.st_mode));
}

int read_max_brightness(const char *device_path) {
    char max_brightness_path[256];
    snprintf(max_brightness_path, sizeof(max_brightness_path), "%s/max_brightness", device_path);
    FILE *fp = fopen(max_brightness_path, "r");
    if (!fp) {
        perror("Failed to read max_brightness");
        return -1;
    }
    int max_brightness;
    fscanf(fp, "%d", &max_brightness);
    fclose(fp);
    return max_brightness;
}