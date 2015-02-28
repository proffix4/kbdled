# kbdled

A small utility for controlling your keyboard LEDs.

Tested under FreeBSD 10.1.

## Installation

```bash
make install
```

## Usage

Find out what tty your on and substitute `/dev/ttyv2` with your current tty.

Toggle your Caps Lock LED:
```bash
kbdled /dev/ttyv2 c
```

Num Lock LED:
```bash
kbdled /dev/ttyv2 n
```

Scroll Lock LED:
```bash
kbdled /dev/ttyv2 s
```

Reset all LEDs to off:
```bash
kbdled /dev/ttyv2 r
```

You can combine options to toggle multiple LEDs.

Toggle all LEDs:
```bash
kbdled /dev/ttyv2 cns
```

Enable all LEDs not matter their state:
```bash
kbdled /dev/ttyv2 rcns
```

## License

Copyright (c) 2015 by Tobias Kortkamp

Distributed under the BSD 2-Clause License.
