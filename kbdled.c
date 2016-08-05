/*
 * Copyright (c) 2015-2016 Tobias Kortkamp <t@tobik.me>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY
 * SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION
 * OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN
 * CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include <err.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/capsicum.h>
#include <sys/ioctl.h>
#include <sys/kbio.h>
#include <unistd.h>

int led_on(int, int);
int led_off(int, int);
int led_toggle(int, int);
int parse_seq(char*, int);
void sandbox(int);

int
led_on(int state, int led) {
    return state | led;
}

int
led_off(int state, int led) {
    return (state | led) ^ led;
}

int
led_toggle(int state, int led) {
    return state & led ? led_off(state, led) : led_on(state, led);
}

int
parse_seq(char *s, int state) {
    int (*f)(int, int) = led_toggle;

    for (char op = *s; op != 0; op = *++s) {
        int led = 0;
        int skip = 0;

        switch (op) {
        case 'c':
            led = LED_CAP;
            break;
        case 'n':
            led = LED_NUM;
            break;
        case 's':
            led = LED_SCR;
            break;
        case 'r': /* reset */
            state = 0;
            break;
        case '+':
            f = led_on;
            skip = 1;
            break;
        case '-':
            f = led_off;
            skip = 1;
            break;
        default:
            warnx("unknown op: %c", op);
            break;
        }

        if (!skip) {
            state = f(state, led);
            f = led_toggle;
        }
    }

    return state;
}

void
sandbox(int fd) {
    cap_rights_t rights;
    if (cap_enter() < 0)
        err(1, "cap_enter");

    if (cap_rights_limit(fd, cap_rights_init(&rights, CAP_IOCTL)) < 0)
        err(1, "cap_rights_limit");

    if (cap_ioctls_limit(fd, (unsigned long[]){KDSETLED, KDGETLED}, 2) < 0)
        err(1, "cap_ioctls_limit");
}

int
main(int argc, char **argv) {
    char **init_argv;
    char *seq;
    const char *dev = "/dev/tty";
    const char *errstr;
    int fd;
    int maxms = 1000*60*60*24 /* a dau */;
    int state;
    unsigned int ms;

    if (argc >= 3) {
        if (strcmp(argv[1], "-t") == 0) {
            argv++;
            dev = *++argv;
        }
        seq = *++argv;
    } else {
        fprintf(stderr,
                "usage: kbdled [+-][cnsr]\n"
                "       kbdled -t tty [+-][cnsr]\n");
        return 1;
    }

    fd = open(dev, O_RDONLY);
    if (fd == -1)
        err(1, "open");

    sandbox(fd);

    init_argv = argv;

    if (ioctl(fd, KDGETLED, &state) == -1)
        err(1, "ioctl");

    for (seq = *argv; *argv; seq = *++argv) {
        if (strcmp(seq, "loop") == 0) {
            argv = init_argv;
            seq = *argv;
        } else if (*seq == 'w') {
            seq++;

            ms = (unsigned int)strtonum(seq, 0, maxms, &errstr);
            if (errstr != NULL)
                errx(1, "'%s' not a number: %s", seq, errstr);

            if (usleep(ms * 1000) == -1 && errno != EINTR)
                err(1, "usleep");

            continue;
        }

        state = parse_seq(seq, state);
        if (ioctl(fd, KDSETLED, state) == -1)
            err(1, "cannot set led state");
    }

    close(fd);

    return 0;
}
