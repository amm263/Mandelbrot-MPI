/*
* Progetto di Programmazione Parallela: Mandelbrot
* AA 2011-2012 - Autore: Andrea Mazzotti - Matricola 0000418295
*/

#pragma once

#include "stdafx.h"

using namespace std;

class Dataset
{
public:
	Dataset(int x, int y);
	~Dataset(void);
	static Dataset* get_instance();
	void write_image(string filename, int max_color);
	int** image;
	int get_width();
	int get_height();
private: 
	static Dataset* instance;
	int rows;
	int columns;
};

