#include "raylib.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>

#define BUFFER_SIZE 256
#define WIDTH 800
#define HEIGHT 800
#define TAG "UART Plotter"

void usage(const char *prog_name) {
    printf("Usage: %s -D <device> [-b <baud_rate>] [-p <parity>] [-s <stop_bits>] [-d <data_bits>] [-h]\n", prog_name);
    printf("  -D <device>       Specify the serial device (e.g., /dev/ttyACM0)\n");
    printf("  -b <baud_rate>    Set the baud rate (default: 115200)\n");
    printf("  -p <parity>       Set the parity (options: none, odd, even; default: none)\n");
    printf("  -s <stop_bits>    Set the number of stop bits (options: 1, 2; default: 1)\n");
    printf("  -d <data_bits>    Set the number of data bits (options: 5, 6, 7, 8; default: 8)\n");
    printf("  -h                Show this help message\n");
}

int configure_serial_port(int fd) {
    struct termios options;

    if (tcgetattr(fd, &options) != 0) {
        perror("ERROR: Getting attributes");
        return -1;
    }

    cfsetispeed(&options, B115200);
    cfsetospeed(&options, B115200);

    options.c_cflag &= ~PARENB;
    options.c_cflag &= ~CSTOPB;
    options.c_cflag &= ~CSIZE;
    options.c_cflag |= CS8;
    options.c_cflag &= ~CRTSCTS;
    options.c_cflag |= (CLOCAL | CREAD);

    options.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
    options.c_iflag &= ~(IXON | IXOFF | IXANY);
    options.c_oflag &= ~OPOST;

    if (tcsetattr(fd, TCSANOW, &options) != 0) {
        perror("ERROR: Setting attributes");
        return -1;
    }

    tcflush(fd, TCIFLUSH);
    return 0;
}

int main(int argc, char *argv[]) {
    int opt;
    char *device = NULL;
    int baud_rate = 115200;
    char *parity = "none";
    int stop_bits = 1;
    int data_bits = 8;

    while ((opt = getopt(argc, argv, "D:b:p:s:d:h")) != -1) {
        switch (opt) {
        case 'D':
            device = optarg;
            break;
        case 'b':
            baud_rate = atoi(optarg);
            break;
        case 'p':
            parity = optarg;
            break;
        case 's':
            stop_bits = atoi(optarg);
            break;
        case 'd':
            data_bits = atoi(optarg);
            break;
        case 'h':
            usage(argv[0]);
            return 0;
        default:
            usage(argv[0]);
            return 1;
        }
    }

    if (device == NULL) {
        fprintf(stderr, "Error: Device not specified. Use -D to specify a device.\n");
        usage(argv[0]);
        return 1;
    }

    printf("INFO: Using Configuration:\n");
    printf("INFO:     Device:..... %s\n", device);
    printf("INFO:     Baud rate:.. %d\n", baud_rate);
    printf("INFO:     Parity:..... %s\n", parity);
    printf("INFO:     Stop bits:.. %d\n", stop_bits);
    printf("INFO:     Data bits:.. %d\n", data_bits);

    int fd = open(device, O_RDWR | O_NOCTTY);
    if (fd == -1) {
        printf("ERROR: Failed to open %s: %s\n", device, strerror(errno));
        return -1;
    }

    if (configure_serial_port(fd) != 0) {
        close(fd);
        return -1;
    }

    char buffer[BUFFER_SIZE];
    ssize_t nbytes;
    float value;

    InitWindow(WIDTH, HEIGHT, TAG);
    Font Helvetica = LoadFont("./font/Helvetica-Bold.ttf");
    SetTargetFPS(60);

    printf("Waiting for data from %s...\n", device);
    while (!WindowShouldClose()) {

        /* nbytes = read(fd, buffer, sizeof(buffer) - 1); */
        /* if (nbytes > 0) { */
        /*     buffer[nbytes] = '\0'; */
        /*     if (sscanf(buffer, "%f", &value) == 1) { */
        /*         printf("Received float: %f\n", value); */
        /*     } else { */
        /*         printf("Received non-float data: %s\n", buffer); */
        /*     } */
        /* } else if (nbytes < 0) { */
        /*     perror("Error reading data"); */
        /*     break; */
        /* } */

        BeginDrawing();
        {
            ClearBackground(BLACK);
            /* DrawText("Hello from the plotter", 100, 100, 24, WHITE); */
            Vector2 pos = {.x = 100, .y = 100};
            DrawTextEx(Helvetica, "Hello World", pos, 24, 1, WHITE);

        }
        EndDrawing();
    }

    CloseWindow();
    close(fd);
    return 0;
}
