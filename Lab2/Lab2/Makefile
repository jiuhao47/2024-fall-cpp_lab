test: compile
	python3 check.py
		 
test_one: compile
	cp ./src/AstBuilder.cpp ./src/Utils.h ../../Lab1/Lab1-2/src/
	cp ./*.g4 ../../Lab1/Lab1-2
	cp ./*.g4 ../../Lab1/Lab1-1/grammar
	cd ../../Lab1/Lab1-2 && make test
	../../Lab1/Lab1-2/build/astbuilder $(FILE)
	./build/irbuilder $(FILE)

lab2:
	cp ./src/AstBuilder.cpp ./src/Utils.h ../../Lab1/Lab1-2/src/
	cp ./*.g4 ../../Lab1/Lab1-2
	cp ./*.g4 ../../Lab1/Lab1-1/grammar
	cd ../../Lab1/Lab1-2 && make test

compile:
	cd build && make -j8 

gdb: compile
	gdb ./build/irbuilder -ex "init-gef" -ex "r "$(FILE)
	

