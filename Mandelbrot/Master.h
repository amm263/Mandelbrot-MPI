/*
* Progetto di Programmazione Parallela: Mandelbrot
* AA 2011-2012 - Autore: Andrea Mazzotti - Matricola 0000418295
*/

#pragma once

#include "Dataset.h"

class Master
{
public:
	Master(int numofnodes);
	~Master(void);
	void run();
private:
	int* slaves;
	int idle_slaves;
	int total_slaves;
	void manage_slaves();
	void assign_work(int slave,int x,int y);
	void catch_slave();
	Dataset* dataset;
	struct offsets{
		int x;
		int y;
	};
	struct offsets* slaves_offsets;
};

