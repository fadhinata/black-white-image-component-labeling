
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <assert.h>  /* for unit tests only */

/*
 * The value of a background pixel.
 */
#define BACKGROUND 0

/*
 * The maximum number of components allowed in this image.
 */
#define MAX_COMPONENTS 1024

/*
 * Disjoint union-find taken right from Introduction To Algorithms
 * by Cormen, et al.
 */

/*
 * The parent array in the disjoint union forest. See Cormen reference.
 * We dynamically allocate this so we don't chew up static memory.
 */
static int *parent;

/*
 * rank[x] is the height of the tree that x belongs to. This is used in
 * path compression to make union find fast. See Cormen reference.
 * We dynamically allocate this so we don't chew up static memory.
 */
static int *rank;

/*
 * Allocate the memory parent and rank arrays. We assume that there will
 * MAX_COMPONENTS sets.
 */
static void init_set() {
    parent = (int *) malloc(sizeof(int) * MAX_COMPONENTS);
    rank = (int *) malloc(sizeof(int) * MAX_COMPONENTS);
}

/*
 * Free up the memory used by parent and rank.
 */
static void destroy_set() {
    free(parent);
    parent = NULL;
    free(rank);
    rank = NULL;
}

/*
 * make-set: make a singleton set { x } in the forest.
 *
 * precondition: x < MAX_COMPONENTS. See Cormen reference.
 */
static void make_set(int x) {
    parent[x] = x;
    rank[x] = 0;
}

/*
 * This is a helper function called by Union. See Cormen reference.
 *
 * precondition: x and y are already the representative objects for their
 *               respective sets (equivalence classes).
 *
 *               This unions the two "sets" but ensures that the shorter
 *               tree is connected to the larger tree there by compressing
 *               the paths in the tree.
 *
 */
static void link(int x, int y) {
    if (rank[x] > rank[y])
        parent[y] = x;
    else {
        parent[x] = y;
        if (rank[x] == rank[y])
            rank[y] = rank[y] + 1;
    }
}

/*
 *
 * Return the "representative" object for the set that x is in. Just
 * keep following parents until you hit a root. This is the recursive version
 * that also does path compression by making all elements along the
 * path point to the root. We don't worry about the statck size here
 * because the recursion will not be very deep.
 *
 * For example, if we have the tree 4 -> 3 -> 2 -> 1
 * after find_set(4) we will have ...
 *
 *        1
 *      / | \
 *     4  3  2
 *
 */
static int find_set(int x) {

    if (x != parent[x])
        parent[x] = find_set(parent[x]);

    return parent[x];
}


/*
 * This unions to the two disjoint sets that x and y are in. See Cormen
 * reference.
 *
 * precondition: x and y are in two distinct sets. That is,
 *               find_set(x) != find_set(y);
 *
 * We could probably make this a little more efficient by
 * not having the overhead to call link; just inline link
 * in this function.
 */
static void Union(int x, int y) {
    link(find_set(x), find_set(y));
}

/*
 * Initialize a memory region to val. Eventually we'll change this to use
 * memset when we are just storing bytes.
 *
 * precondition:
 *      1) image != NULL and there is enough memory allocated in
 *         image to accommodate n ints.
 *      2) n > 0
 */
static void init_mem(int *image, int val, int n) {
    for (int *p = image; p < (image + n); p++)
        *p = val;
}

/*
 * Find smallest element of vec from vec[0] ... vec[n-1]
 */
static int minvec(int *vec, int n) {
    int small = vec[0];
    for (int i = 0; i < n; i++)
        if (vec[i] < small)
            small = vec[i];
    return small;
}

/*
 * Labels the components of a binary image based on algorithm from
 * http://en.wikipedia.org/wiki/Connected-component_labeling
 *
 * This modifies the image in place.
 *
 * I'm not sure of the original reference for this algorithm.
 *
 * This assumes a one-pixel BACKGROUND border around the image to make
 * checking the neighborhood around a pixel easier.
 */
static void label(int *image, int rows, int cols) {

    int next_label = 1;
    int neighbors[4];
    int nc; /* neighbor count */

    init_set();  /* initialize the disjoint union forest */

    for (int r = 1; r < rows - 1; r++) {
        for (int c = 1; c < cols - 1; c++) {

            if (image[r * cols + c] == BACKGROUND) continue;

            /*
             * reinitialize our vector of neighbors. I think
             * we can get away without this since the variable nc keeps track of
             * exactly how many values in neighbors are being used.
             */
            init_mem(&neighbors[0], INT_MAX, 4);
            nc = 0;

            /*
             * Keep track of which neighbors are part of the image.
             */
            if (image[r*cols + c-1] != BACKGROUND)   /* west */
                neighbors[nc++] = image[r*cols + c-1];

            if (image[(r-1)*cols + (c-1)] != BACKGROUND) /* north-west */
                neighbors[nc++] = image[(r-1)*cols + (c-1)];

            if (image[(r-1)*cols + c] != BACKGROUND)   /* north */
                neighbors[nc++] = image[(r-1)*cols + c];

            if (image[(r-1)*cols + (c+1)] != BACKGROUND) /* north-east */
                neighbors[nc++] = image[(r-1)*cols + (c+1)];

            /*
             * If none of the neighbors are part of the image and this
             * pixel is we *may* have found a new component.
             */
            if (nc == 0) {
                make_set(next_label);
                image[r*cols + c] = next_label;
                next_label++;
            }
            else {

                /*
                 * Label this pixel with the smallest label of the neighbor
                 * and also make sure all neighbors are in the same set
                 */
                image[r*cols + c] = minvec(neighbors, nc);

                for (int i = 0; i < nc; i++)
                    for (int j = i+1; j < nc; j++)
                        if (neighbors[i] != neighbors[j])
                            Union(neighbors[i], neighbors[j]);
            }

        } /* column */
    } /* row */

    /*
     * pass 2 patches up the components so that each component
     * has the same label represented from its set.
     */
    for (int r = 1; r < rows - 1; r++)
        for (int c = 1; c < cols - 1; c++)
            if (image[r * cols + c] != BACKGROUND)
                image[r*cols + c] = find_set(image[r * cols + c]);


    destroy_set(); /* we don't need the disjoint union forest any longer */
} /* label */


/*
 * -----------------------------------------------------------------------------
 * ---------------------- T E S T   M A I N ------------------------------------
 * -----------------------------------------------------------------------------
 */

/*
 * Some tests on the union-find algorithm
 */
static void test_union_find() {
    init_set();

    /*
     * Make a singleton set with zero in it and make sure its
     * representative is 0
     */
    make_set(0);
    assert(find_set(0) == 0);


    /*
     * Make a forest of singleton sets: 1, 2, 3, 4, 5, ..., 255
     */
    for(int i = 1; i < 256; i++) {
        make_set(i);
        assert(find_set(i) == i);
    }

    /* 1 and 2 better be in the same set after we union them. */
    Union(1,2);
    assert(find_set(1) == find_set(2));

    /* 1,2,3 better all be in the same set after this */
    Union(2, 3);
    assert(find_set(1) == find_set(2) &&
            find_set(1) == find_set(3) &&
            find_set(2) == find_set(3));

    /* 1 and 4 better not be in the same set */
    assert(find_set(1) != find_set(4));

    destroy_set();
    return;

}

/*
 * Assert that two images are equal
 */
static int assert_images_equal(int *image1, int *image2, int rows, int cols) {
    for (int i = 0; i < rows * cols; i++)
        assert(image1[i] == image2[i]);
    return 1;
}


/*
 * Unit Test 1
 *
 * This image has only one component, so the labeled image is
 * the same as the original image.
 */
static void test1() {
    int image[6][6] = {
            {0, 0, 0, 0, 0, 0},
            {0, 1, 1, 1, 0, 0},
            {0, 0, 0, 1, 0, 0},
            {0, 1, 1, 1, 0, 0},
            {0, 0, 0, 0, 1, 0},
            {0, 0, 0, 0, 0, 0}
    };

    int image_answer[6][6] = {
            {0, 0, 0, 0, 0, 0},
            {0, 1, 1, 1, 0, 0},
            {0, 0, 0, 1, 0, 0},
            {0, 1, 1, 1, 0, 0},
            {0, 0, 0, 0, 1, 0},
            {0, 0, 0, 0, 0, 0}
    };


    label(image[0], 6, 6);
    assert_images_equal(image[0], image_answer[0], 6, 6);
}


/*
 * Unit Test 2
 *
 * This image has two components.
 */
static void test2() {

    int image[6][6] = {
            {0, 0, 0, 0, 0, 0},
            {0, 1, 0, 0, 0, 0},
            {0, 1, 0, 1, 0, 0},
            {0, 1, 0, 1, 1, 0},
            {0, 1, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0}
    };

    int image_answer[6][6] = {
            {0, 0, 0, 0, 0, 0},
            {0, 1, 0, 0, 0, 0},
            {0, 1, 0, 2, 0, 0},
            {0, 1, 0, 2, 2, 0},
            {0, 1, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0}
    };

    label(image[0], 6, 6);
    assert_images_equal(image[0], image_answer[0], 6, 6);
}

/*
 * Unit Test 3
 *
 * This Test has two components but they are shaped a little weird.
 * The test case represents that X is one components and # the other.
 *

               X  X  X
                     X
               X     X
               X  X  X        #
                        X     #
                  X     X     #
                  X  X  X     #
                              #
                  #           #
               #     #  #  #
               #

 */
static void test3() {

    int image[13][8] = {
            /* 0 */     {0, 0, 0, 0, 0, 0, 0, 0},
            /* 8 */     {0, 1, 1, 1, 0, 0, 0, 0},
            /* 16 */    {0, 0, 0, 1, 0, 0, 0, 0},
            /* 24 */    {0, 1, 0, 1, 0, 0, 0, 0},
            /* 32 */    {0, 1, 1, 1, 0, 0, 1, 0},
            /* 40 */    {0, 0, 0, 0, 1, 0, 1, 0},
            /* 48 */    {0, 0, 1, 0, 1, 0, 1, 0},
            /* 56 */    {0, 0, 1, 1, 1, 0, 1, 0},
            /* 64 */    {0, 0, 0, 0, 0, 0, 1, 0},
            /* 72 */    {0, 0, 1, 0, 0, 0, 1, 0},
            /* 80 */    {0, 1, 0, 1, 1, 1, 0, 0},
            /* 88 */    {0, 1, 0, 0, 0, 0, 0, 0},
            /* 96 */    {0, 0, 0, 0, 0, 0, 0, 0}
    };

    int image_answer[13][8] = {
            {0, 0, 0, 0, 0, 0, 0, 0},
            {0, 1, 1, 1, 0, 0, 0, 0},
            {0, 0, 0, 1, 0, 0, 0, 0},
            {0, 1, 0, 1, 0, 0, 0, 0},
            {0, 1, 1, 1, 0, 0, 3, 0},
            {0, 0, 0, 0, 1, 0, 3, 0},
            {0, 0, 1, 0, 1, 0, 3, 0},
            {0, 0, 1, 1, 1, 0, 3, 0},
            {0, 0, 0, 0, 0, 0, 3, 0},
            {0, 0, 3, 0, 0, 0, 3, 0},
            {0, 3, 0, 3, 3, 3, 0, 0},
            {0, 3, 0, 0, 0, 0, 0, 0},
            {0, 0, 0, 0, 0, 0, 0, 0}
    };

    label(image[0], 13, 8);
    assert_images_equal(image[0], image_answer[0], 13, 8);
}

/*
 * Read a text file of zeros and ones and build an image object so we can
 * label it. The image is assumed to be 640x480.
 */
int* read_bin() {
   char line[1280];
   static int image[480][640];
   int x = 0;
   int y = 0;
   int val = 0;

   FILE *fp = fopen("connected_test_binary.txt", "r");
   while (fgets(line, 2000, fp)) {  /* use a number > than the line length */
	   char *tok = strtok(line, " ");
	   x = 0;
	   do {
		   val = atoi(tok);
		   image[y][x++] = val;
	   } while ((tok = strtok(NULL, " ")));
	   y++;
   }
   return image[0];
}

/*
 * This is a real test where we test this on a binary image generated
 * from connected_test.png
 */
void test4() {

	int *image = read_bin();
    label(image, 480, 640);
    for (int y = 0; y < 480; y++) {
    	for (int x = 0; x < 640; x++)
    		printf("%d ", image[y*640 + x]);
    	printf("\n");
    }

	return;
}

int main(void) {
    test_union_find();
    test1();
    test2();
    test3();
	test4();
    return 0;
}
