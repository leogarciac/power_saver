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
#include <unistd.h>

#define TIMEOUT 5              // Timeout in seconds
#define INPUT_DIR "/dev/input" // Input devices directory

long get_current_time() {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC, &ts);
  return ts.tv_sec;
}

int ss(pid_t *pid) {
  const char *art = "       / \\__\n"
                    "      (    @\\___\n"
                    "       /         O\n"
                    "     /   (_____ / \n"
                    "    /_____ /   U\n"
                    "  /  |  |  |  |\n"
                    " /   |  |  |  |\n"
                    "   /    |  |  |\n"
                    "    /   |  |  |\n"
                    "  (_____|_____|_\n"
                    "    A COOL DOG!";

  char command[1024];
  snprintf(command, sizeof(command),
           "gnome-terminal --full-screen -- bash -c \"PS1='' clear; echo '%s'; "
           "read -p ' '\"",
           art);

  int result = system(command);
  if (result == -1) {
    result = system("xterm -fullscreen -e \"clear; echo 'SCREEN SAVER'; read "
                    "-p 'Press enter to exit...'\"");
  }
  if (result == -1) {
    printf("No suitable terminal emulator found.\n");
    return 1;
  }

  FILE *fp = popen("pgrep -n gnome-terminal", "r");
  if (fp == NULL) {
    fp = popen("pgrep -n xterm", "r");
  }
  if (fp != NULL) {
    fscanf(fp, "%d", pid);
    fclose(fp);
  }

  return 0;
}

void exit_terminal(pid_t pid) {
  if (pid > 0) {
    kill(pid, SIGKILL);
  }
}

int open_devices(struct libevdev **devices, int *fds, int max_devices) {
  struct dirent *entry;
  DIR *dir = opendir(INPUT_DIR);
  if (!dir) {
    perror("Failed to open input directory");
    return -1;
  }

  int count = 0;
  while ((entry = readdir(dir)) != NULL && count < max_devices) {
    if (strncmp(entry->d_name, "event", 5) == 0) {
      char path[256];
      snprintf(path, sizeof(path), "%s/%s", INPUT_DIR, entry->d_name);
      int fd = open(path, O_RDONLY | O_NONBLOCK);
      if (fd < 0)
        continue;

      struct libevdev *dev = NULL;
      if (libevdev_new_from_fd(fd, &dev) < 0) {
        close(fd);
        continue;
      }

      printf("Found device: %s (%s)\n", libevdev_get_name(dev), path);

      fds[count] = fd;
      devices[count] = dev;
      count++;
    }
  }

  closedir(dir);
  return count;
}

int main() {
  struct libevdev *devices[64];
  int fds[64];
  int device_count = open_devices(devices, fds, 64);

  if (device_count <= 0) {
    fprintf(stderr, "No input devices found.\n");
    return 1;
  }

  printf("Monitoring activity from detected devices...\n");

  struct pollfd fds_poll[64];
  for (int i = 0; i < device_count; i++) {
    fds_poll[i].fd = fds[i];
    fds_poll[i].events = POLLIN;
  }

  long last_activity = get_current_time();
  pid_t terminal_pid = -1;

  while (1) {
    int ret = poll(fds_poll, device_count, 1000);
    if (ret < 0) {
      perror("Error during poll");
      break;
    }

    if (ret > 0) {
      for (int i = 0; i < device_count; i++) {
        if (fds_poll[i].revents & POLLIN) {
          struct input_event ev;
          while (libevdev_next_event(devices[i], LIBEVDEV_READ_FLAG_NORMAL,
                                     &ev) == 0) {
            if (ev.type == EV_KEY || ev.type == EV_REL) {
              last_activity = get_current_time();
              if (terminal_pid != -1) {
                exit_terminal(terminal_pid);
                terminal_pid = -1;
              }
            }
          }
        }
      }
    }

    long current_time = get_current_time();
    if (current_time - last_activity >= TIMEOUT) {
      if (terminal_pid == -1) {
        ss(&terminal_pid);
      }
      last_activity = current_time;
    }
  }

  for (int i = 0; i < device_count; i++) {
    libevdev_free(devices[i]);
    close(fds[i]);
  }

  return 0;
}
