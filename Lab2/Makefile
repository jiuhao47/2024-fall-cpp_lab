test: compile
	python3 check.py
		 

compile:
	cd build && make -j8 

gdb: compile
	gdb ./build/irbuilder -ex "init-gef" -ex "r "$(FILE)
	

