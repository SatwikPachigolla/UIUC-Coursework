
CXX = clang++
LD = clang++

CXXFLAGS = $(CS225) -std=c++1y -stdlib=libc++ -c -g -O0 -Wall -Wextra -Werror -pedantic
LDFLAGS = -std=c++1y -stdlib=libc++ -lc++abi -lpthread -lm

intro: main.o HSLAPixel.o PNG.o intro.o lodepng.o
	$(LD) main.o HSLAPixel.o PNG.o intro.o lodepng.o $(LDFLAGS) -o intro

main.o: main.cpp intro.h cs225/PNG.h
	$(CXX) $(CXXFLAGS) main.cpp

HSLAPixel.o: cs225/HSLAPixel.h cs225/HSLAPixel.cpp
	$(CXX) $(CXXFLAGS) cs225/HSLAPixel.cpp

PNG.o: cs225/HSLAPixel.h cs225/PNG.h cs225/PNG.cpp cs225/lodepng/lodepng.h cs225/RGB_HSL.h
	$(CXX) $(CXXFLAGS) cs225/PNG.cpp

intro.o: intro.cpp intro.h cs225/PNG.h cs225/HSLAPixel.h
	$(CXX) $(CXXFLAGS) intro.cpp

lodepng.o: cs225/lodepng/lodepng.cpp cs225/lodepng/lodepng.h
	$(CXX) $(CXXFLAGS) cs225/lodepng/lodepng.cpp

test: tests.o PNG.o HSLAPixel.o lodepng.o intro.o
	$(LD) tests.o PNG.o HSLAPixel.o lodepng.o intro.o $(LDFLAGS) -o test

tests.o: tests/tests.cpp tests/catch.hpp cs225/PNG.h cs225/HSLAPixel.h
	$(CXX) $(CXXFLAGS) tests/tests.cpp

clean:
	rm intro test *.o
