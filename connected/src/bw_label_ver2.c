/*
 ============================================================================
 Name        : bw_label.c
 Author      : 
 Version     :
 Copyright   : Your copyright notice
 Description : Hello World in C, Ansi-style
 ============================================================================
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>

#define BACKGROUND 255
typedef unsigned char Byte;

#define ROWS 200
#define COLS 100

#define min(x,y) ((x) < (y) ? (x) : (y))
#define min4(a,b,c,d)  (min(min((a),(b)), min((c),(d))))

/*
 * Disjoint union-find taken right from Introduction To Algorithms
 * by Cormen, et al.
 */

int parent[1000]; /* I have no idea how big these should be */
int rank[1000];

static void make_set(int x) {
	parent[x] = x;
    rank[x] = 0;
}

static void link(int x, int y) {
	if (rank[x] > rank[y])
		parent[y] = x;
	else {
		parent[x] = y;
		if (rank[x] == rank[y])
			rank[y] = rank[y] + 1;
	}
}

static int find_set(int x) {
	while (parent[x] != x)
		x = parent[x];
	return x;
}

static void Union(int x, int y) {
    link(find_set(x), find_set(y));
}

static void init_mem(int *image, int val, int n) {
     for (int *p = image; p < (image + n); p++)
    	 *p = val;
}

static int *label(int image[][6], int rows, int cols) {

	int next_label = 1;
    int neighbors[4];
	int nc = 0; /* neighbor count */

    int *labels = (int *) malloc(rows * cols * sizeof(int));


	/*memset(labels, BACKGROUND, rows*cols); */
    init_mem(labels, BACKGROUND, rows*cols);

	for (int r = 1; r < rows - 1; r++) {
		init_mem(&neighbors[0], INT_MAX, 4);
		nc = 0;

		for (int c = 1; c < cols - 1; c++) {
			if (image[r][c] == BACKGROUND) continue;

			if (labels[r*cols + c-1] != BACKGROUND)   /* west */
				neighbors[nc++] = labels[r*cols + c-1];

			if (labels[(r-1)*cols + (c-1)] != BACKGROUND) /* north-west */
				neighbors[nc++] = labels[(r-1)*cols + (c-1)];

			if (labels[(r-1)*cols + c] != BACKGROUND)   /* north */
				neighbors[nc++] = labels[(r-1)*cols + c];

			if (labels[(r-1)*cols + (c+1)] != BACKGROUND) /* north-east */
				neighbors[nc++] = labels[(r-1)*cols + (c+1)];

			if (nc == 0) {
				make_set(next_label);
				labels[r*cols + c] = next_label;
				next_label++;
			}
			else {
                labels[r*cols + c] = min4(neighbors[0],neighbors[1],
                		                  neighbors[2],neighbors[3]);

                /* make sure all neighbors are in the same set */
                for (int i = 0; i < nc; i++)

                	/* for each neighbor union it with _______??? */
                    for (int j = 0; j < nc; j++)
                	   Union(parent[neighbors[i]], neighbors[j]);
			}

		} /* column */

		/* pass 2 */
		for (int r = 1; r < rows - 1; r++)
			for (int c = 1; c < cols - 1; c++)
				if (image[r][c] != BACKGROUND)
					labels[r*cols+c] = find_set(labels[r*cols+c]);
	} /* row */

	return labels;
} /* label */



int main(void) {

	int image[6][6] = {
			{255, 255, 255, 255, 255, 255},
			{255, 000, 000, 000, 255, 255},
			{255, 255, 255, 000, 255, 255},
			{255, 000, 000, 000, 255, 255},
			{255, 255, 255, 255, 000, 255},
			{255, 255, 255, 255, 255, 255}
	};

	int **labeledimage = label(image, 6, 6);
	return 0;
}
