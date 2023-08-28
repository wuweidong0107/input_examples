#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <linux/input.h>
#include <err.h>

void usage(int argc, char *argv[])
{
    printf("Usage:\n");
    printf("    %s /dev/input/by-path/platform-i8042-serio-0-event-kbd KEY_LEFTSHIFT\n", argv[0]);
    printf("    %s /dev/input/by-path/platform-gpio-keys-event KEY_KP0\n", argv[0]);

    exit(EXIT_FAILURE);
}

int main(int argc, char *argv[], char *env[])
{
    if (argc != 3)
        usage(argc, argv);

    int key;

    if (strcmp(argv[2], "KEY_LEFTSHIFT") == 0)
        key = KEY_LEFTSHIFT;
    else if (strcmp(argv[2], "KEY_RIGHTSHIFT") == 0)
        key = KEY_RIGHTSHIFT;
    else if (strcmp(argv[2], "KEY_KP0") == 0)
        key = KEY_KP0;
    else if (strcmp(argv[2], "KEY_KP1") == 0)
        key = KEY_KP1;
    else
    {
        printf("Unsupported param\n");
        usage(argc, argv);
    }

    FILE *kbd = fopen(argv[1], "r");
    if (kbd == NULL)
    {
        err(1, "Fail to open device");
    }

    char key_map[KEY_MAX / 8 + 1]; //  Create a byte array the size of the number of keys

    memset(key_map, 0, sizeof(key_map));                     //  Initate the array to zero's
    ioctl(fileno(kbd), EVIOCGKEY(sizeof(key_map)), key_map); //  Fill the keymap with the current keyboard state

    int keyb = key_map[key / 8]; //  The key we want (and the seven others arround it)
    int mask = 1 << (key % 8);   //  Put a one in the same column as out key state will be in;

    printf("%s:%d\n", argv[2], !!(keyb & mask));
    return 0;
}