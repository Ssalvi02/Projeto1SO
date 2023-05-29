all:
	gcc *.c -o semaforo -lpthread -lm
	./semaforo

clean:
	rm semaforo