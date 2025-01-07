# include "ps.h"

int set_brightness_for_device(const char *device_path, int brightness_percent) {
    int max_brightness = read_max_brightness(device_path);
    if (max_brightness < 0)
        return -1;
    int scaled_brightness = max_brightness * brightness_percent / 100;
    char brightness_path[256];
    snprintf(brightness_path, sizeof(brightness_path), "%s/brightness", device_path);
    FILE *fp = fopen(brightness_path, "w");
    if (!fp) {
        perror("Failed to set brightness");
        return -1;
    }
    fprintf(fp, "%d", scaled_brightness);
    fclose(fp);
    printf("Brightness for device '%s' set to %d (%d%% of max).\n", device_path, scaled_brightness, brightness_percent);
    return 0;
}

void set_brightness_dynamically(int brightness_percent) {
    const char *backlight_dir = "/sys/class/backlight/";
    if (!directory_exists(backlight_dir)) {
        printf("No backlight devices found on this system.\n");
        return;
    }
    glob_t globbuf;
    char pattern[256];
    snprintf(pattern, sizeof(pattern), "%s*", backlight_dir);
    if (glob(pattern, 0, NULL, &globbuf) == 0) {
        int devices_found = 0;
        for (size_t i = 0; i < globbuf.gl_pathc; i++) {
            const char *device_path = globbuf.gl_pathv[i];
            devices_found++;
            printf("Found backlight device: %s\n", device_path);
            set_brightness_for_device(device_path, brightness_percent);
        }
        if (devices_found == 0)
            printf("No usable backlight devices found.\n");
    }
    else
        perror("Failed to list backlight devices");
    globfree(&globbuf);
}


int main(int argc, char **argv) {
    if (argc == 1) {
    int brightness_percent == atoi(argv[1]);
    printf("Enter brightness percentage (0-100): ");
    if (scanf("%d", &brightness_percent) != 1 || brightness_percent < 0 || brightness_percent > 100) {
        printf("Invalid input. Please enter a number between 0 and 100.\n");
        return 1;
    }
    set_brightness_dynamically(brightness_percent);
    return 0;
    }
    return -1;
}
