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
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/kbio.h>
#include <unistd.h>

int
main(int argc, char **argv) {
    if(argc < 3) {
        fprintf(stderr, "usage: kbdled tty [cnsr]\n");
        return EXIT_FAILURE;
    }

    int tty = open(argv[1], O_RDONLY);
    if (tty == -1) {
        perror("open");
        return EXIT_FAILURE;
    }

    int state = 0;
    if (ioctl(tty, KDGETLED, &state) == -1) {
        perror("ioctl");
        return EXIT_FAILURE;
    }

    int val = state;
    char *s = argv[2];
    for(char c = *s; c != 0; c = *++s) {
        int led = 0;
        switch(c) {
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
            val = 0;
            break;
        default:
            break;
        }

        /* toggle led */
        val = val & led ? val ^ led : val | led;
    }

    if (ioctl(tty, KDSETLED, val) == -1) {
        perror("ioctl");
        return EXIT_FAILURE;
    }

    close(tty);

    return EXIT_SUCCESS;
}
