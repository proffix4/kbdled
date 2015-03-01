/*-
 *   Copyright (c) 2015 by Tobias Kortkamp
 *   tobias.kortkamp@gmail.com
 *
 *   All rights reserved.
 *
 *   Redistribution and use in source and binary forms, with or without
 *   modification, are permitted provided that the following conditions
 *   are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in
 *       the documentation and/or other materials provided with the
 *       distribution.
 *
 *   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 *   "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 *   LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 *   A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 *   OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 *   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 *   LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 *   DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 *   THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 *   (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 *   OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */
#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/kbio.h>
#include <sysexits.h>
#include <unistd.h>

int initial_led_state = 0;

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

    for(char op = *s; op != 0; op = *++s) {
        int led = 0;
        int skip = 0;

        switch(op) {
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

        if(!skip) {
            state = f(state, led);
            f = led_toggle;
        }
    }

    return state;
}

int
led_state(int fd) {
    int state = 0;
    int status = ioctl(fd, KDGETLED, &state);
    if (status == -1)
        err(EX_IOERR, "cannot get led state");
    return state;
}

void
set_led_state(int fd, int state) {
    int status = ioctl(fd, KDSETLED, state);
    if (status == -1)
        err(EX_IOERR, "cannot set led state");
}

int
main(int argc, char **argv) {
    char *seq;
    char *dev = "/dev/tty";
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
        return EXIT_FAILURE;
    }

    int tty = open(dev, O_RDONLY);
    if (tty == -1)
        errc(EX_IOERR, errno, "opening tty failed");

    int state = led_state(tty);
    initial_led_state = state;

    char **init_argv = argv;
    for (char *seq = *argv; *argv; seq = *++argv) {
        if (strcmp(seq, "loop") == 0) {
            argv = init_argv;
            seq = *argv;
        } else if (*seq == 'w') {
            seq++;
            long s = strtol(seq, NULL, 10);

            if(errno == EINVAL || errno == ERANGE)
                err(EX_DATAERR, "'%s' not a number", seq);

            if(s < 0) {
                errx(EX_DATAERR, "'%s' is not a positive number", seq);
            }

            if(usleep(s * 1000) == -1)
                err(EX_OSERR, "usleep failed");

            continue;
        }

        set_led_state(tty, parse_seq(seq, state));
    }

    close(tty);

    return EXIT_SUCCESS;
}
