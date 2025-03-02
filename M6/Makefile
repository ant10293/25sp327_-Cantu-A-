all:
	gcc -Wall -Wextra -g setuid_demo.c -o setuid_demo

clean:
	rm -f setuid_demo

test:
	@echo "Setting up setuid-root permissions..."
	sudo chown root setuid_demo
	sudo chmod u+s setuid_demo
	@ls -l setuid_demo
	@echo "Running the program as a non-root user..."
	./setuid_demo
