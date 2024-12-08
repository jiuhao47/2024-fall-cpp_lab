test: compile
	python3 check.py
		 
test_one: compile
	./build/irbuilder $(FILE)

compile:
	cd build && make -j8 

gdb: compile
	gdb ./build/irbuilder -ex "init-gef" -ex "r "$(FILE)
	

