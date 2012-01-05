#include <stdio.h>
#include <stdlib.h>

typedef unsigned char Byte;

/*
 * Disjoint set using union-find. Algorithm taken from
 * from Cormen, Leiserson, Rivest, and Stein
 */
typedef struct Item * Itemptr;

/* A set is just a pointer to the a root item. */
typedef Itemptr Set;
struct Item {
	int label;
	Itemptr parent;
	int rank;
};

Itemptr make_set(int label) {
	Itemptr item = (Itemptr) malloc(sizeof(struct Item));
	item->parent = NULL;
	item->label = label;
	item->rank = 0;
	return item;
}

Itemptr link(Itemptr x, Itemptr y) {
	if (x->rank > y->rank) {
		y->parent = x;
		return x;
	}
	else {
		x->parent = y;
		if (x->rank == y->rank)
			y->rank = y->rank + 1;
		return y;
	}
}

Itemptr find_set(Itemptr x) {
	while (x->parent != NULL)
		x = x->parent;
	return x;
}

Itemptr djunion(Itemptr x, Itemptr y) {
	return link(find_set(x), find_set(y));
}

void test_union_find() {
	Itemptr x = make_set(3);
	Itemptr y = make_set(4);
	Itemptr z = make_set(5);
	Itemptr a = djunion(x,y);
	Itemptr b = djunion(a,z);
}

Byte *label(Byte *image, int rows, int cols) {
	Byte *lim = (Byte *) malloc(rows*cols);

	int r = 0;
	int c = 0;



}

int main(void) {
	return 0;
}
