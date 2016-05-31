/*
* Progetto di Programmazione Parallela: Mandelbrot
* AA 2011-2012 - Autore: Andrea Mazzotti - Matricola 0000418295
*/


#pragma once
class Slave
{
public:
	Slave(int myrank);
	~Slave(void);
	void run(int myrank);
private:
	int width;
	int height;
	int section_size;
	int* section;
	void calculate(int x, int y);
	void send_data();
};

