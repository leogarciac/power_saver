# include "ps.h"

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
int set_brightness_for_device(const char *device_path, int brightness_percentage) {
    int max_brightness = read_max_brightness(device_path);
    if (max_brightness < 0) return -1;
    int scaled_brightness = max_brightness * brightness_percentage / 100;
    char brightness_path[256];
    snprintf(brightness_path, sizeof(brightness_path), "%s/brightness", device_path);
    FILE *fp = fopen(brightness_path, "w");
    if (!fp) {
        perror("Failed to set brightness");
        return -1;
    }
    fprintf(fp, "%d", scaled_brightness);
    fclose(fp);
    printf("Brightness for device '%s' set to %d (%d%% of max).\n", device_path, scaled_brightness, brightness_percentage);
    return 0;
}

void set_brightness_dynamically(int brightness_percentage) {
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
            set_brightness_for_device(device_path, brightness_percentage);
        }
        if (devices_found == 0)
            printf("No usable backlight devices found.\n");
    }
    else
        perror("Failed to list backlight devices");
    globfree(&globbuf);
}


/* int main() {
    int brightness_percentage;
    printf("Enter brightness percentage (0-100): ");
    if (scanf("%d", &brightness_percentage) != 1 || brightness_percentage < 0 || brightness_percentage > 100) {
        printf("Invalid input. Please enter a number between 0 and 100.\n");
        return 1;
    }
    set_brightness_dynamically(brightness_percentage);
    return 0;
} */
