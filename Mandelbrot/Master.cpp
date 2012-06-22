/*
* Progetto di Programmazione Parallela: Mandelbrot
* AA 2011-2012 - Autore: Andrea Mazzotti - Matricola 0000418295
*/

#include "StdAfx.h"
#include "Master.h"
#include "Dataset.h"

#define CALC 0
#define DONE 1
#define SHUTDOWN 2
#define SECTION_SIZE 100

Master::Master(int numofnodes)
{
	int width, height, temp;

	// Inizializzo il numero di schiavi --> Numero di processi attivi -1.
	total_slaves= numofnodes-1;

	// Chiedo all'utente di inserire larghezza e lunghezza dell'immagine.
	cout << "Enter width of graph (pixels):";
	fflush(stdout); 
	scanf_s("%d",&width); 
	cout << "Enter height of graph (pixels):";
	fflush(stdout);
	scanf_s("%d",&height);  
	// Se le dimensioni inserite sono troppo piccole, le inserisco di default.
	if(&width==NULL | width<640) width=640;
	if(&height==NULL |height<640) height=640;
	cout << "Creating Dataset..." << endl << endl;
	temp= width/SECTION_SIZE;
	width= temp*SECTION_SIZE;

	temp= height/SECTION_SIZE;
	height= temp*SECTION_SIZE;

	//Inizializzo l'oggetto dataset. 
	dataset= new Dataset(height, width);
	//Mando a tutti gli schiavi le dimensioni dell'immagine.
	int sizes[3];
	sizes[0]=width;
	sizes[1]=height;
	sizes[2]=SECTION_SIZE;
	MPI_Bcast(&sizes,3,MPI_INT,0,MPI_COMM_WORLD);
}

void Master::run()
{
	int i;
	//Comincio a gestire il lavoro.
	cout << "Managing slaves..." << endl << endl;
	manage_slaves();

	cout << "Writing image." << endl;
	dataset->write_image("mandelbrot.bmp",1000);

	//Dico a tutti gli schiavi di spegnersi.
	for (i = 1; i <= total_slaves; i++)
	{    
		cout << "Master: Shutting down Slave " << i << endl;
		MPI_Send (NULL, 0, MPI_INT, i, SHUTDOWN, MPI_COMM_WORLD);
	}
}

void Master::manage_slaves()
{
	int x,y,i;
	int sections_y= dataset->get_width()/SECTION_SIZE;
	int sections_x= dataset->get_height()/SECTION_SIZE;
	int offset=0; // Offset necessario per la suddivisone dell'area di lavoro.

	//Inizializzo l'array per tenere traccia degli schiavi liberi.
	slaves=(int*)malloc(sizeof(int)*(total_slaves));
	for (i=0;i<total_slaves;i++)
	{
		slaves[i]=0;
	}
	//Tutti gli schiavi inizialmente non hanno lavoro assegnato.
	idle_slaves=total_slaves;

	//Array necessario per tenere in memoria gli offsets degli slave.
	slaves_offsets=(struct offsets*)malloc(sizeof(struct offsets)*total_slaves);

	cout << "Master: Starting the work cycle" << endl;

	for (x=0;x<sections_x;x++)
	{
		for (y=0;y<sections_y;)
		{
			if (idle_slaves>0) //Se c'è almeno uno schiavo libero
			{
				cout << "Master: There are idle slaves" << endl;
				for(i=0;i<total_slaves;i++) // Lo cerco
				{
					if (slaves[i]==0)
					{
						cout << "Master: found idle slave "<< i << endl;
						assign_work(i,x,y); //Gli assegno del lavoro
						slaves[i]=1; // Lo schiavo è occupato.
						idle_slaves--;
						y++; // Y viene incrementato qui, in alternativa perderei un loop ogni qual volta non vengono trovati schiavi liberi.
						break;
					}
				}
			}
			else //Se nessuno degli schiavi è libero
			{
				catch_slave(); //Aspetto che qualcuno mi consegni il lavoro
			}
		}
	}
	// Aspetto che tutti abbiano finito il lavoro prima di terminare.
	while(idle_slaves!=total_slaves)
	{
		catch_slave();
	}
}

void Master::assign_work(int slave,int x, int y)
{
	cout << "Master: Sending work to slave " << slave << ". X= "<< x << " Y= " << y << endl;
	int slave_offsets[2];
	slave_offsets[0]=x;
	slave_offsets[1]=y;
	// Assegno il lavoro allo schiavo
	MPI_Send(&slave_offsets,2,MPI_INT,slave+1,CALC,MPI_COMM_WORLD);
	// Mi salvo gli offsets appena mandati
	slaves_offsets[slave].x=x;
	slaves_offsets[slave].y=y;
	cout << "Master: Work sent" << endl;
}

void Master::catch_slave()
{
	int i=0;
	int x,y;
	MPI_Status stat;
	int* section_done=(int*)malloc(sizeof(int)*(SECTION_SIZE*SECTION_SIZE));
	cout << "Master: Catching slave " << endl;
	// Ricevo dallo slave una sezione completata
	MPI_Recv(section_done,SECTION_SIZE*SECTION_SIZE,MPI_INT,MPI_ANY_SOURCE,DONE,MPI_COMM_WORLD,&stat);
	cout << "Masted: cought " << stat.MPI_SOURCE  << endl;
	// La salvo nella matrice-immagine.
	for (x=0;x<SECTION_SIZE;x++)
	{
		for (y=0;y<SECTION_SIZE;y++)
		{
			if ((slaves_offsets[stat.MPI_SOURCE-1].x*SECTION_SIZE+x<=dataset->get_height()) && (slaves_offsets[stat.MPI_SOURCE-1].y*SECTION_SIZE+y<=dataset->get_width()))
			{
				//cout << " " << section_done[i];
				//cout << "Slave offsets: x = " << slaves_offsets[stat.MPI_SOURCE-1].x << " | y= " << slaves_offsets[stat.MPI_SOURCE-1].y << endl;
				//cout << "Image [" << slaves_offsets[stat.MPI_SOURCE-1].x*SECTION_SIZE+x << "][" << slaves_offsets[stat.MPI_SOURCE-1].y*SECTION_SIZE+y << "]" << endl;
				dataset->image[slaves_offsets[stat.MPI_SOURCE-1].x*SECTION_SIZE+x][slaves_offsets[stat.MPI_SOURCE-1].y*SECTION_SIZE+y]=section_done[i];
			}
			i++;
		}
	}
	// Libero lo slave.
	slaves[stat.MPI_SOURCE-1]=0;
	idle_slaves++;
	cout << "Master: One slave is free" << endl;
}

Master::~Master(void)
{

}
