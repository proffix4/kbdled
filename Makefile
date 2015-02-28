all: kbdled

clean:
	@rm -f kbdled

install: all
	@install kbdled /usr/local/bin

.PHONY: all clean install
