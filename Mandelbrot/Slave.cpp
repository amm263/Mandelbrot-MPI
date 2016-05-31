/*
* Progetto di Programmazione Parallela: Mandelbrot
* AA 2011-2012 - Autore: Andrea Mazzotti - Matricola 0000418295
*/

#include "StdAfx.h"
#include "Slave.h"

#define CALC 0
#define DONE 1
#define SHUTDOWN 2

#define MAX_ITERATIONS 1000

Slave::Slave(int myrank)
{
	int i;
	int sizes[3];
	// Ricevo le informazioni dell'immagine dal Broadcast.
	MPI_Bcast(&sizes,3,MPI_INT,0,MPI_COMM_WORLD);
	width=sizes[0];
	height=sizes[1];
	section_size=sizes[2];


	//MPI_Send(NULL,0,MPI_INT,0,0,MPI_COMM_WORLD);


	//Inizializzo la sezione di calcolo.
	section=(int*)malloc(sizeof(int)*(section_size*section_size));
	
	cout << "Slave "<< myrank << " ready for work!" << endl;
}

void Slave::run(int myrank)
{
	MPI_Status status;
	bool work_done=false;

	while(!work_done)
	{
		cout << "Slave " << myrank << " probing for assignments" << endl;
		// C'è del lavoro?
		MPI_Probe(0,MPI_ANY_TAG,MPI_COMM_WORLD,&status);
		if(status.MPI_TAG==CALC)
		{
			cout << "Slave " << myrank << " : work found." << endl;
			int offsets[2];
			MPI_Recv(&offsets,2,MPI_INT,status.MPI_SOURCE,CALC,MPI_COMM_WORLD,&status);
			cout << "Slave" << myrank << " :x " << offsets[0] << " y " << offsets[1] << endl;
			calculate(offsets[0],offsets[1]);
		}
		else if(status.MPI_TAG==SHUTDOWN)
		{
			MPI_Recv(NULL,0,MPI_INT,status.MPI_SOURCE,SHUTDOWN,MPI_COMM_WORLD,&status);
			work_done=true;
		}
		else
			cout << "Slave_Error: Unknown tag value!" << endl;
	}

	cout << "Slave " << myrank << " : work done." << endl;
}

void Slave::calculate(int x, int y)
{
	int section_x, section_y, i;
	int position=0;
	int calc_x=x;
	int calc_y=y;
	double MinRe=-2.0;
	double MaxRe=1.2;
	double MinIm=-1.2;
	double MaxIm=MinIm+(MaxRe-MinRe)*(width/height);

	double Re_factor=(MaxRe-MinRe)/(width-1);
	double Im_factor=(MaxIm-MinIm)/(height-1);

	double c_re;

	double c_im;
	double Z_re,Z_im,Z_re2,Z_im2;
	bool inside;

	calc_x=x*section_size-1;
	for (section_x=0;section_x<section_size;section_x++)
	{
		calc_y=y*section_size-1;
		calc_x++;
		c_re=MinRe+calc_x*Re_factor;
		for(section_y=0;section_y<section_size;section_y++)
		{
			calc_y++;
			c_im=MinIm+(calc_y)*Im_factor;
			Z_re=c_re;
			Z_im=c_im;
			inside=true;
			for(i=0;i<MAX_ITERATIONS;i++)
			{
				Z_re2=Z_re*Z_re;
				Z_im2=Z_im*Z_im;
				if(Z_re2+Z_im2>4) //Diverge
				{
					inside=false;
					break;
				}
				Z_im=(2*Z_re*Z_im)+c_im;
				Z_re=Z_re2-Z_im2+c_re;
			}
			if(inside)
				section[position]=i;
			else
				section[position]=0;
			position++;
		}
	}

	send_data();

}

void Slave::send_data()
{
	cout << "Slave: Sending data" << endl;
	MPI_Send(section,section_size*section_size,MPI_INT,0,DONE,MPI_COMM_WORLD);
}

Slave::~Slave(void)
{
}


