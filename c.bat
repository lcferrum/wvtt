g++ vt.cpp -static-libgcc -static-libstdc++ -std=c++11 -s -lversion -o vt.exe

g++ vti.cpp -static-libgcc -static-libstdc++ -s -o vti.exe

g++ -c vtc.cpp -o vtc.o
windres -i vtc.rc -o res.o
g++ -s vtc.o res.o -static-libgcc -static-libstdc++ -s -o vtc.exe

rm *.o
del *.o
