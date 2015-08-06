all: build

# Clean up
clean:
	rm bin/config_parser
	rm bin/item_test
	rm bin/parser_test

# Build only
build:
	g++ -Wall -Wno-unused-variable -std=c++11 main.cc config/handler.cc config/item.cc config/parser.cc -o bin/config_parser

# Build and run
run: build
	./bin/config_parser

# There will be no output from the executable if all tests pass.
test:
	@echo "\n> 1 of 2: Running item_test.cc..."
	g++ -Wall -Wno-unused-variable -Wno-missing-braces -std=c++11 config/item.cc config/item_test.cc -o bin/item_test
	./bin/item_test
	@echo "Done!"
	@echo "\n> 2 of 2: Running parser_test.cc..."
	g++ -Wall -Wno-unused-variable -Wno-missing-braces -std=c++11 config/parser.cc config/parser_test.cc config/item.cc -o bin/parser_test
	./bin/parser_test
	@echo "Done!"
