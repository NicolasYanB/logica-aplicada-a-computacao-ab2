OBJECTS=build/equivalence.o build/variable_hash.o build/test.o

main: build_dir $(OBJECTS)
	gcc -g -o build/$@ $(OBJECTS)

build_dir:
	mkdir -p build

build/test.o: test.c equivalence.h
	gcc -g -c -o $@ $<

build/equivalence.o: equivalence.c equivalence.h node_types.h
	gcc -g -c -o $@ $<

build/variable_hash.o: variable_hash.c variable_hash.h node_types.h
	gcc -g -c -o $@ $<

clear:
	rm -rf build

