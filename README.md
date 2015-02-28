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
       kbdled tty [+-][cnsr]
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
kbdled /dev/ttyv8 +cns
```

## License

Copyright (c) 2015 by Tobias Kortkamp

Distributed under the BSD 2-Clause License.
