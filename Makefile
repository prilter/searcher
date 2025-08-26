comp_scanner:
	mkdir -p bin bin/objs
	g++ src/web_scanner/*.cpp -c -lcurl $$(xml2-config --cflags) $$(xml2-config --libs)
	mv *.o bin/objs

comp_main:
	mkdir -p bin bin/objs
	g++ src/main.cpp -c
	mv *.o bin/objs

comp_all: comp_main comp_scanner
	g++ bin/objs/*.o -o bin/searcher -lcurl $$(xml2-config --cflags) $$(xml2-config --libs)

run: comp_all
	./bin/searcher https://github.com

clean:
	rm -rf ./bin

git: clean
	git add . && git commit -F - && git push
