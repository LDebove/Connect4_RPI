all : joypi.o puissance4.o main.c
	gcc joypi.o puissance4.o main.c -lwiringPi -o exe
puissance4.o : puissance4.c puissance4.h
	gcc -c puissance4.c
joypi.o : joypi.c joypi.h
	gcc -c joypi.c -lwiringPi

clean :
	rm -rf *.o exe

	#/home/pi/Desktop/RPI/Projet/puissance4.o
	#/home/pi/Desktop/RPI/Projet/joypi.o