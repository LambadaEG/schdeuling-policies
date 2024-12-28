all: build

build: main.cpp Scheduling.cpp
	g++ main.cpp Scheduling.cpp -o program

test_output/:
	rm -rf test_output
	mkdir -p test_output

test: build test_output/
	@for file in testcases/*input.txt; do \
		echo "Running test $$file"; \
		basename=$$(basename "$$file" | sed 's/input/output/'); \
		./program < "$$file" > test_output/$$basename; \
	done

	@if diff -r testcases test_output -x '*-input.txt'; then \
		echo "ALL TESTS PASS : GG WP GO NEXT TERM"; \
	else \
		echo "Some 7amoo el dod alaa tests fail enta  ج.ض"; \
	fi


