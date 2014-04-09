all: monitor monitorAux fileMonitor

monitor: monitor.o
	gcc monitor.o -o monitor
monitor.o: monitor.c
	gcc -c monitor.c

monitorAux: monitorAux.o
	gcc monitorAux.o -o monitorAux
monitorAux.o: monitorAux.c
	gcc -c monitorAux.c

fileMonitor: fileMonitor.o
	gcc fileMonitor.o -o fileMonitor
fileMonitor.o: fileMonitor.c
	gcc -c fileMonitor.c
