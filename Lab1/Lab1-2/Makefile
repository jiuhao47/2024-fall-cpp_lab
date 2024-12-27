test: compile
	sh go.sh true
		 

debug: compile
	sh go.sh false

compile:
	cd build && make -j8 

gdb: compile
	gdb ./build/astbuilder -ex "init-gef" -ex "r "$(FILE)
	

