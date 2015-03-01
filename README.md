# kbdled

A small utility for controlling your keyboard LEDs.

Tested under FreeBSD 10.1.

## Installation

```bash
make install
```

## Usage

```
usage: kbdled [+-][cnsr]
       kbdled -t tty [+-][cnsr]
```

Toggle your Caps Lock LED:
```bash
kbdled c
```

Toggle Num Lock LED:
```bash
kbdled n
```

Toggle Scroll Lock LED:
```bash
kbdled s
```

Reset all LEDs to off:
```bash
kbdled r
```

You can combine options to toggle multiple LEDs.

Toggle all LEDs:
```bash
kbdled cns
```

Prefix an LED with `+` to enable it and with `-` to disable it.

Enable the Caps Lock LED:
```bash
kbdled +c
```

Disable the Caps Lock LED:
```bash
kbdled -c
```

Enable all LEDs not matter their previous state:
```bash
kbdled +c+n+s
```

You can specify a tty like this:
```bash
kbdled -t /dev/ttyv8 +cns
```

There is some simple support for animating your LEDs.
The following example enables your Caps Lock LED, waits 250 ms and then disables it again.
```bash
kblded +c w250 -c
```

You can loop with `loop`. This makes your Caps Lock LED blink forever:
```bash
kbdled +c w250 -c w250 loop
```

To make all LEDs "bounce" back and forth:
```bash
kbdled r+n w250 -n+c w250 -c+s w250 -s+c w250 -c+n w250 loop
```

## License

Copyright (c) 2015 by Tobias Kortkamp

Distributed under the BSD 2-Clause License.
