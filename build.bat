del /s /q obj\Debug\*.*
del bin\Debug\SpineMiner.exe
mingw32-g++ -Wall -fexceptions -g -c database.cpp -o obj\Debug\database.o
mingw32-g++ -Wall -fexceptions -g -c dbscan.cpp -o obj\Debug\dbscan.o
mingw32-g++ -Wall -fexceptions -g -c functions.cpp -o obj\Debug\functions.o
mingw32-g++ -Wall -fexceptions -g -c helpers.cpp -o obj\Debug\helpers.o
mingw32-gcc -Wall -fexceptions -g -c kdtree\kdtree.c -o obj\Debug\kdtree.o
mingw32-g++ -Wall -fexceptions -g -c main.cpp -o obj\Debug\main.o
mingw32-g++ -Wall -fexceptions -g -c ovlreader.cpp -o obj\Debug\ovlreader.o
mingw32-gcc -Wall -fexceptions -g -c sqlite\sqlite3.c -o obj\Debug\sqlite3.o
mingw32-g++ -o bin\Debug\SpineMiner.exe obj\Debug\database.o obj\Debug\dbscan.o obj\Debug\functions.o obj\Debug\helpers.o obj\Debug\kdtree.o obj\Debug\main.o obj\Debug\ovlreader.o obj\Debug\sqlite3.o -lgdi32 -liphlpapi -lws2_32
