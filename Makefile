INC = $(wildcard *.h)
all: a.exe 
	a.exe > image.ppm
	-del image.png
	nconvert -out png image.ppm

a.exe: main.cpp $(INC)
	g++ -O2 -m64 -Wall -std=c++17 -I . main.cpp