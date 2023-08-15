#include <stdio.h>
#include <getopt.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

#include "lightsensor.h"
#include "event.h"

static struct option opts[] = {
    { "lightsensor", 1, NULL, 'l' },
    { "input", 1, NULL, 'i' },
	{ "enable", 1, NULL, 'e' },
    { "read", 0, NULL, 'r' },
	{ "help", 0, NULL, 'h' },
	{ 0, 0, NULL, 0 }
};

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

static void usage(void)
{
	printf("Usage: lightsensor [OPTION...]\n");
	printf("    -l, --lightsensor     char device path\n");
	printf("    -i, --input           input device path\n");
	printf("    -e, --enable          enable device\n");
    printf("    -r, --read            read event\n");
	printf("    -h, --help            show this help message\n");
	printf("Example:\n");
	printf("    lightsensor -l /dev/lightsensor -i /dev/input/event3 -e 1 -r\n");
	exit(0);
}

int main(int argc, char** argv)
{
    int c, option_index = 0;
    char *ls_path = NULL, *input_path = NULL;
    unsigned int enable = -1;
    int read = 0;

    if (geteuid() != 0) {
		printf("Need to run as root\n");
		exit(1);
	}

    while((c = getopt_long(argc, argv, "l:i:e:r:h", opts, &option_index)) != -1) {
    	switch(c) {
    	case 'l':
    		ls_path = optarg;
    		break;
        case 'i':
    		input_path = optarg;
    		break;
    	case 'e':
    		enable = strtoul(optarg, NULL, 10);
    		break;
        case 'r':
    		read = 1;
    		break;
    	case 'h':
    		usage();
    		break;
    	}
    }

    if (ls_path == NULL || input_path == NULL) {
        usage();
    }

    lightsensor_t* ls = lightsensor_new();
    if (ls == NULL) {
        printf("lightsensor_new() fail\n");
        return -1;
    }

    if (lightsensor_open(ls, ls_path, input_path) != 0) {
        fprintf(stderr, "lightsensor_open(): %s\n", lightsensor_errmsg(ls));
        return -1;
    }

    if (enable != -1) {
        if (lightsensor_set_enable(ls, enable) < 0) {
            fprintf(stderr, "lightsensor_set_enable(): %s\n", lightsensor_errmsg(ls));
            return -1;
        }      
    }
    
    if (lightsensor_close(ls) != 0) {
        fprintf(stderr, "lightsensor_close(): %s\n", lightsensor_errmsg(ls));
        return -1;
    }

    while(read > 0) {
        struct input_event event;
        if (lightsensor_read(ls, &event, 1, 10000) != 1) {
            fprintf(stderr, "lightsensor_close(): %s\n", lightsensor_errmsg(ls));
            return -1;
        }
        print_event(event.type, event.code, event.value, PRINT_LABELS);
    }

    lightsensor_free(ls);
    return 0;
}