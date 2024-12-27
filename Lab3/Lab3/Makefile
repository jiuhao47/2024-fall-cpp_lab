compile:
	cd build && make -j8

run:
	./build/optirbuilder $(FILE)

test_one: compile
	./build/optirbuilder $(FILE)


all_one: compile
	rm -rf output.txt && ./build/optirbuilder $(FILE) > output.txt && /bin/cat output.txt; \
	lvim output.txt
	
