/*
* Progetto di Programmazione Parallela: Mandelbrot
* AA 2011-2012 - Autore: Andrea Mazzotti - Matricola 0000418295
*/

#include "stdafx.h"
#include "Master.h"
#include "Slave.h"

int main(int argc,char *argv[])
{
	int myrank, numofnodes;
	Master* master;
	Slave* slave;

	MPI_Init(&argc,&argv);
	MPI_Comm_rank(MPI_COMM_WORLD,&myrank);
	MPI_Comm_size(MPI_COMM_WORLD,&numofnodes); 

	// Se sono il primo processo, divento il Master.
	if(myrank==0)
	{
		master= new Master(numofnodes);
		master->run();
	}
	// Se non sono il primo processo, sono uno Slave.
	else
	{
		slave= new Slave(myrank);
		slave->run(myrank);
	}

	MPI_Finalize();
	return 0;
}