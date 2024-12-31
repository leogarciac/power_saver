#include <errno.h>
#include <fcntl.h>
#include <linux/input.h>
#include <poll.h>   // For poll()
#include <signal.h> // For kill()
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> // For kill()
#include <time.h>      // For clock_gettime()
#include <unistd.h>    // For read() and close()

#define TIMEOUT 5 // Timeout in seconds

long get_current_time() {
  struct timespec ts;
  clock_gettime(CLOCK_MONOTONIC,
                &ts); // Monotonic clock to avoid system time changes
  return ts.tv_sec;
}

int ss(pid_t *pid) {
  // Cool ASCII Art
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

  // Format the command to pass to system()
  char command[1024];
  snprintf(command, sizeof(command),
           "gnome-terminal --full-screen -- bash -c \"PS1='' clear; echo '%s'; "
           "read -p ' '\"",
           art);

  // Run the command
  int result = system(command);

  // If GNOME Terminal is not available, try XTerm
  if (result == -1) {
    result = system("xterm -fullscreen -e \"clear; echo 'SCREEN SAVER'; read "
                    "-p 'Press enter to exit...'\"");
  }

  // If neither works, display an error message
  if (result == -1) {
    printf("No suitable terminal emulator found.\n");
    return 1;
  }

  // Finding the PID of the terminal after launch.
  // Using a method like pgrep to get the terminal's PID, which assumes that the
  // terminal is the latest launched process with the name 'gnome-terminal' or
  // 'xterm'.
  FILE *fp = popen("pgrep -n gnome-terminal", "r");
  if (fp == NULL) {
    fp = popen("pgrep -n xterm", "r");
  }
  if (fp != NULL) {
    fscanf(fp, "%d", pid); // Store the terminal PID
    fclose(fp);
  }

  return 0;
}

void exit_terminal(pid_t pid) {
  // Send a kill signal to the terminal process to close it
  if (pid > 0) {
    kill(pid, SIGKILL); // Terminate the terminal using SIGKILL
  }
}

int main() {
  const char *mouse = "/dev/input/event13";    // Path to mouse event
  const char *keyboard = "/dev/input/event25"; // Path to keyboard event

  int fd1 = open(mouse, O_RDONLY);
  if (fd1 == -1) {
    perror("Error opening mouse device");
    return 1;
  }

  int fd2 = open(keyboard, O_RDONLY);
  if (fd2 == -1) {
    perror("Error opening keyboard device");
    close(fd1);
    return 1;
  }

  printf("Monitoring activity from keyboard and mouse...\n");

  // Prepare for poll
  struct pollfd fds[2];
  fds[0].fd = fd1;
  fds[0].events = POLLIN; // Wait for input events
  fds[1].fd = fd2;
  fds[1].events = POLLIN;

  long last_activity = get_current_time();
  pid_t terminal_pid = -1; // Variable to store terminal process ID

  while (1) {
    int ret = poll(fds, 2, 1000); // Wait for events with a timeout of 1000 ms
    if (ret < 0) {
      perror("Error during poll");
      break;
    }

    if (ret > 0) {
      struct input_event ev;
      for (int i = 0; i < 2; i++) {
        if (fds[i].revents & POLLIN) {
          ssize_t bytes = read(fds[i].fd, &ev, sizeof(struct input_event));
          if (bytes == sizeof(struct input_event)) {
            last_activity = get_current_time(); // Reset timer

            // If there's activity, exit the terminal and reset the timer
            if (terminal_pid != -1) {
              exit_terminal(terminal_pid); // Close the terminal
              terminal_pid = -1;           // Reset the terminal PID
            }
          }
        }
      }
    }

    // Check for inactivity
    long current_time = get_current_time();
    if (current_time - last_activity >= TIMEOUT) {
      if (terminal_pid == -1) {
        // If the terminal is not running, start it
        ss(&terminal_pid);
      }
      // Reset timer after printing
      last_activity = current_time;
    }
  }

  close(fd1);
  close(fd2);
  return 0;
}
