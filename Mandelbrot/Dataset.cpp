/*
* Progetto di Programmazione Parallela: Mandelbrot
* AA 2011-2012 - Autore: Andrea Mazzotti - Matricola 0000418295
*/

#include "StdAfx.h"
#include "Dataset.h"
#include "intarray2bmp.hpp"

Dataset* Dataset::instance=NULL;


/*
*	Il costruttore inizializza la matrice quadrata di interi "image"
*	dove verrà salvato il lavoro.
*/
Dataset::Dataset(int x, int y)
{
	int i,j;
	rows=x;
	columns=y;
	image=(int**)malloc(sizeof(int*)*rows);
	for(i=0; i<rows; i++)
	{
		image[i]=(int*)malloc(sizeof(int)*columns);
		for (j=0; j<columns; j++)
		{
			image[i][j]=0;
		}
	}
}

int Dataset::get_width()
{
	return columns;
}

int Dataset::get_height()
{
	return rows;
}


/*
*	Questa funzione utilizza la libreria intarray2bmp per trasformare la matrice di interi in una immagine bitmap
*/
void Dataset::write_image(string filename, int max_color)
{
	cout << "Almost done" << endl;
	bool result = intarray2bmp::intarray2bmp( filename, image, rows, columns, 0, max_color );
	cout << "Should be done" << endl;
}


Dataset::~Dataset(void)
{
	delete image;
}

Dataset* Dataset::get_instance()
{
	if (instance==NULL)
		return NULL;
	return instance;
}