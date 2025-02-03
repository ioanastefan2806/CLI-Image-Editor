build:
	gcc *.c -o image_editor -Wall -Wextra -lm

.PHONY: clean
clean:
	rm image_editor