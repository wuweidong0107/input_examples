#include <stdio.h>
#include <linux/input.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <err.h>
#include <unistd.h>
#include <string.h>

#include "event.h"

enum {
    PRINT_DEVICE_ERRORS     = 1U << 0,
    PRINT_DEVICE            = 1U << 1,
    PRINT_DEVICE_NAME       = 1U << 2,
    PRINT_DEVICE_INFO       = 1U << 3,
    PRINT_VERSION           = 1U << 4,
    PRINT_POSSIBLE_EVENTS   = 1U << 5,
    PRINT_INPUT_PROPS       = 1U << 6,
    PRINT_HID_DESCRIPTOR    = 1U << 7,

    PRINT_ALL_INFO          = (1U << 8) - 1,

    PRINT_LABELS            = 1U << 16,
};

static const char *get_label(const struct label *labels, int value)
{
    while(labels->name && value != labels->value) {
        labels++;
    }
    return labels->name;
}

static void print_event(int type, int code, int value, int print_flags)
{
    const char *type_label, *code_label, *value_label;

    if (print_flags & PRINT_LABELS) {
        type_label = get_label(ev_labels, type);
        code_label = NULL;
        value_label = NULL;

        switch(type) {
        case EV_SYN:
            code_label = get_label(syn_labels, code);
            break;
        case EV_KEY:
            code_label = get_label(key_labels, code);
            value_label = get_label(key_value_labels, value);
            break;
        case EV_REL:
            code_label = get_label(rel_labels, code);
            break;
        case EV_ABS:
            code_label = get_label(abs_labels, code);
            switch(code) {
            case ABS_MT_TOOL_TYPE:
                value_label = get_label(mt_tool_labels, value);
            }
            break;
            case EV_MSC:
                code_label = get_label(msc_labels, code);
                break;
            case EV_LED:
                code_label = get_label(led_labels, code);
                break;
            case EV_SND:
                code_label = get_label(snd_labels, code);
                break;
            case EV_SW:
                code_label = get_label(sw_labels, code);
                break;
            case EV_REP:
                code_label = get_label(rep_labels, code);
                break;
            case EV_FF:
                code_label = get_label(ff_labels, code);
                break;
            case EV_FF_STATUS:
                code_label = get_label(ff_status_labels, code);
                break;
        }

        if (type_label)
            printf("%-12.12s", type_label);
        else
            printf("%04x        ", type);
        if (code_label)
            printf(" %-20.20s", code_label);
        else
            printf(" %04x                ", code);
        if (value_label)
            printf(" %-20.20s", value_label);
        else
            printf(" %08x            ", value);
    } else {
        printf("%04x %04x %08x", type, code, value);
    }
}

int main(int argc, char** argv)
{
    int fd, n;
    struct input_event event;
    unsigned char key_states[KEY_MAX/8 + 1];

    memset(key_states, 0, sizeof(key_states));

    if ((fd = open("/dev/input/by-path/platform-i8042-serio-0-event-kbd", O_RDWR)) < 0) {
        err(1, "fail to open device");
    }
    ioctl(fd, EVIOCGKEY(sizeof(key_states)), key_states);

    n = read(fd, &event, sizeof(event));
    if (n != sizeof(event)) {
        err(1, "fail to read device");
    }

    print_event(event.type, event.code, event.value, PRINT_LABELS);
    printf("\n");
}