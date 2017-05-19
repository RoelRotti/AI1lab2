/* nqueens.c: (c) Arnold Meijster (a.meijster@rug.nl) */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

#define MAXQ 100

#define FALSE 0
#define TRUE  1

#define ABS(a) ((a) < 0 ? (-(a)) : (a))

int nqueens;      /* number of queens: global variable */
int queens[MAXQ]; /* queen at (r,c) is represented by queens[r] == c */
int neighbor[MAXQ];
int bestNB[MAXQ];

/* Genetic Algorithm */
int k = 8;
int val1, val2, val3, val4, val5, val6, val7, val8; /* current evaluation value for all 'genes' */
int queens1[MAXQ], child1[MAXQ];
int queens2[MAXQ], child2[MAXQ];
int queens3[MAXQ], child3[MAXQ];
int queens4[MAXQ], child4[MAXQ];
int queens5[MAXQ], child5[MAXQ];
int queens6[MAXQ], child6[MAXQ];
int queens7[MAXQ], child7[MAXQ];
int queens8[MAXQ], child8[MAXQ];
int temporary1[MAXQ], temporary2[MAXQ];


void initializeRandomGenerator() {
  /* this routine initializes the random generator. You are not
   * supposed to understand this code. You can simply use it.
   */
  time_t t;
  srand((unsigned) time(&t));
}

/* Generate an initial position.
 * If flag == 0, then for each row, a queen is placed in the first column.
 * If flag == 1, then for each row, a queen is placed in a random column.
 */
void initiateQueens(int flag) {
  int q;
  for (q = 0; q < nqueens; q++) {
    queens[q] = (flag == 0? 0 : random()%nqueens); 
  }
}

void initiateQueensGenetic(int flag) {
  int q;
  for (q = 0; q < nqueens; q++) {
    queens1[q] = (flag == 0? 0 : random()%nqueens); 
    queens2[q] = (flag == 0? 0 : random()%nqueens); 
    queens3[q] = (flag == 0? 0 : random()%nqueens); 
    queens4[q] = (flag == 0? 0 : random()%nqueens); 
    queens5[q] = (flag == 0? 0 : random()%nqueens); 
    queens6[q] = (flag == 0? 0 : random()%nqueens); 
    queens7[q] = (flag == 0? 0 : random()%nqueens); 
    queens8[q] = (flag == 0? 0 : random()%nqueens); 
  }
}

/* returns TRUE if position (row0,column0) is in 
 * conflict with (row1,column1), otherwise FALSE.
 */
int inConflict(int row0, int column0, int row1, int column1) {
  if (row0 == row1) return TRUE; /* on same row, */
  if (column0 == column1) return TRUE; /* column, */
  if (ABS(row0-row1) == ABS(column0-column1)) return TRUE;/* diagonal */
  return FALSE; /* no conflict */
}

/* returns TRUE if position (row,col) is in 
 * conflict with any other queen on the board, otherwise FALSE.
 */
int inConflictWithAnotherQueen(int row, int col) {
  int queen;
  for (queen=0; queen < nqueens; queen++) {
    if (inConflict(row, col, queen, queens[queen])) {
      if ((row != queen) || (col != queens[queen])) return TRUE;
    }
  }
  return FALSE;
}

/* print configuration on screen */
void printState() {
  int row, column; 
  printf("\n");
  for(row = 0; row < nqueens; row++) {
    for(column = 0; column < nqueens; column++) {
      if (queens[row] != column) {
        printf (".");
      } else {
        if (inConflictWithAnotherQueen(row, column)) {
          printf("Q");
        } else {
          printf("q");
        }
      }
    }
    printf("\n");
  }
}

/* move queen on row q to specified column, i.e. to (q,column) */
void moveQueen(int queen, int column) {
  if ((queen < 0) || (queen >= nqueens)) {
    fprintf(stderr, "Error in moveQueen: queen=%d "
      "(should be 0<=queen<%d)...Abort.\n", queen, nqueens);
    exit(-1);
  }
  if ((column < 0) || (column >= nqueens)) {
    fprintf(stderr, "Error in moveQueen: column=%d "
      "(should be 0<=column<%d)...Abort.\n", column, nqueens);
    exit(-1);
  }
  queens[queen] = column;
}

/* returns TRUE if queen can be moved to position
 * (queen,column). Note that this routine checks only that 
 * the values of queen and column are valid! It does not test
 * conflicts!
 */
int canMoveTo(int queen, int column) {
  if ((queen < 0) || (queen >= nqueens)) {
    fprintf(stderr, "Error in canMoveTo: queen=%d "
      "(should be 0<=queen<%d)...Abort.\n", queen, nqueens);
    exit(-1);
  }
  if(column < 0 || column >= nqueens) return FALSE;
  if (queens[queen] == column) return FALSE; /* queen already there */
  return TRUE;
}

/* returns the column number of the specified queen */
int columnOfQueen(int queen) {
  if ((queen < 0) || (queen >= nqueens)) {
    fprintf(stderr, "Error in columnOfQueen: queen=%d "
      "(should be 0<=queen<%d)...Abort.\n", queen, nqueens);
    exit(-1);
  }
  return queens[queen];
}

/* returns the number of pairs of queens that are in conflict */
int countConflictsHill(int param) {
  int cnt = 0;
  int queen, other;
	switch (param) {
		case 1:{
			for (queen=0; queen < nqueens; queen++) {
				for (other=queen+1; other < nqueens; other++) {
				  if (inConflict(queen, queens[queen], other, queens[other])) {
					cnt++;
				  }
				}
			}
		};       break;
		case 2:{
			for (queen=0; queen < nqueens; queen++) {
				for (other=queen+1; other < nqueens; other++) {
				  if (inConflict(queen, neighbor[queen], other, neighbor[other])) {
					cnt++;
				  }
				}
			}
		};       break;
		case 3:{
			for (queen=0; queen < nqueens; queen++) {
				for (other=queen+1; other < nqueens; other++) {
				  if (inConflict(queen, bestNB[queen], other, bestNB[other])) {
					cnt++;
				  }
				}
			}
		};       break;
	}
	return cnt;
}

/* evaluation function. The maximal number of queens in conflict
 * can be 1 + 2 + 3 + 4 + .. + (nquees-1)=(nqueens-1)*nqueens/2.
 * Since we want to do ascending local searches, the evaluation
 * function returns (nqueens-1)*nqueens/2 - countConflicts().
 */
int evaluateStateHill(int param) {
  return (nqueens-1)*nqueens/2 - countConflictsHill(param);
}

/*************************************************************/

/* returns the number of pairs of queens that are in conflict */
int countConflicts() {
  int cnt = 0;
  int queen, other;
  for (queen=0; queen < nqueens; queen++) {
    for (other=queen+1; other < nqueens; other++) {
      if (inConflict(queen, queens[queen], other, queens[other])) {
        cnt++;
      }
    }
  }
  return cnt;
}

/* evaluation function. The maximal number of queens in conflict
 * can be 1 + 2 + 3 + 4 + .. + (nquees-1)=(nqueens-1)*nqueens/2.
 * Since we want to do ascending local searches, the evaluation
 * function returns (nqueens-1)*nqueens/2 - countConflicts().
 */
int evaluateState() {
  return (nqueens-1)*nqueens/2 - countConflicts();
}

/*************************************************************/

int max (){
	int x = val1;
	if (val2 > x){
		x = val2;
	}
	if (val3 > x){
		x = val3;
	}
	if (val4 > x){
		x = val4;
	}
	if (val5 > x){
		x = val5;
	}
	if (val6 > x){
		x = val6;
	}
	if (val7 > x){
		x = val7;
	}
	if (val8 > x){
		x = val8;
	}
	return x;
}

/* returns the number of pairs of queens that are in conflict */
int evaluateStateGenetic() {
  int queen, other;
  int v1=0, v2=0, v3=0, v4=0, v5=0, v6=0, v7=0, v8=0;
  for (queen=0; queen < nqueens; queen++) {
    for (other=queen+1; other < nqueens; other++) {
      if (inConflict(queen, queens1[queen], other, queens1[other])) {
        v1++;
      }
      if (inConflict(queen, queens2[queen], other, queens2[other])) {
        v2++;
      }
      if (inConflict(queen, queens3[queen], other, queens3[other])) {
        v3++;
      }
      if (inConflict(queen, queens4[queen], other, queens4[other])) {
        v4++;
      }
      if (inConflict(queen, queens5[queen], other, queens5[other])) {
        v5++;
      }
      if (inConflict(queen, queens6[queen], other, queens6[other])) {
        v6++;
      }
      if (inConflict(queen, queens7[queen], other, queens7[other])) {
        v7++;
      }
      if (inConflict(queen, queens8[queen], other, queens8[other])) {
        v8++;
      }
    }
  }
  val1 = (nqueens-1)*nqueens/2 - v1;
  val2 = (nqueens-1)*nqueens/2 - v2;
  val3 = (nqueens-1)*nqueens/2 - v3;
  val4 = (nqueens-1)*nqueens/2 - v4;
  val5 = (nqueens-1)*nqueens/2 - v5;
  val6 = (nqueens-1)*nqueens/2 - v6;
  val7 = (nqueens-1)*nqueens/2 - v7;
  val8 = (nqueens-1)*nqueens/2 - v8;
  return max();
}

/*************************************************************/
/* A very silly random search 'algorithm' */
#define MAXITER 1000
void randomSearch() {
  int queen, iter = 0;
  int optimum = (nqueens-1)*nqueens/2;

  while (evaluateState() != optimum) {
    printf("iteration %d: evaluation=%d\n", iter++, evaluateState());
    if (iter == MAXITER) break;  /* give up */
    /* generate a (new) random state: for each queen do ...*/
    for (queen=0; queen < nqueens; queen++) {
      int pos, newpos;
      /* position (=column) of queen */
      pos = columnOfQueen(queen);
      /* change in random new location */
      newpos = pos;
      while (newpos == pos) {
        newpos = random() % nqueens;
      }
      moveQueen(queen, newpos);
    }
  }
  if (iter < MAXITER) {
    printf ("Solved puzzle. ");
  }
  printf ("Final state is");
  printState();
}

/*************************************************************/

/*
function HILL-CLIMBING(problem) returns a state that is a local maximum
current ← MAKE-NODE(problem.INITIAL-STATE)
loop do
  neighbor ← a highest-valued successor of current
  if neighbor.VALUE ≤ current.VALUE then return current.STATE
  current ← neighbor

Figure 4.2 The hill-climbing search algorithm, which is the most basic local search tech-
nique. At each step the current node is replaced by the best neighbor; in this version, that
means the neighbor with the highest VALUE, but if a heuristic cost estimate h is used, we
would find the neighbor with the lowest h.
*/

void hillClimbing() {
	int currentBest = evaluateStateHill(1), best = currentBest;
	int optimum = (nqueens-1)*nqueens/2;
	int better = 0, queen, column, x, iter=0, ctr=1000;
	do {
		printf("iteration %d: evaluation=%d\n", iter++, evaluateStateHill(1));
		for(queen = 0; queen < nqueens; queen++){							/* clone the current state into neighbor every time we change rows */
			for (x=0; x < nqueens; x++){ 									/* copying current best board position */
				neighbor[x] = queens[x];						
				currentBest = best;
			}
			for(column = 0; column < nqueens; column++){
				neighbor[queen] = column;
				better = evaluateStateHill(2);
				if(better == currentBest){ /* found neighbor is equally good to the currentBest best --> if random value > 5, then copy neighbor to bestNB */
					if ((0+random() % (10-0))>=5){ 
						for (x=0; x < nqueens; x++){
							bestNB[x] = neighbor[x];
						}
					}
				} else if(better > currentBest){						/* the found neigbor is better than the last best */
					for (x=0; x < nqueens; x++){
						bestNB[x] = neighbor[x];
					}
					currentBest = better;
				}				
			}
		}
		if (currentBest == best){
			for (x=0; x < nqueens; x++){
					queens[x] = bestNB[x];
			}
			if (ctr == 0){
				break;
			}
			ctr--;
		}
		if (currentBest > best){
			ctr = 1000;
			for (x=0; x < nqueens; x++){
				queens[x] = bestNB[x];
			}
			best = currentBest;
		} 
	} while (best != optimum && ctr != 0);  
	printf("Final evaluation=%d\n", evaluateStateHill(1));
	printf ("Final state is");
	printState();
}

/*************************************************************/
/********************Simulated Annealing**********************/

int timeToTemperature(int t){
	return 1000-t; /* formula containing t: linear dicreas */
}

void randomSuccessor() {
	int queen = (0+random() % (nqueens-1));
	int column = (0+random() % (nqueens-1));
	int i;
	for (i=0; i<nqueens; i++){
		neighbor[i] = queens[i];
	}
	neighbor[queen] = column;
}

int likelihood(int val, int temp){
	return (((random() % (10))*exp(val/temp)) > 5 ? TRUE : FALSE);
}

void simulatedAnnealing() {
	int optimum = (nqueens-1)*nqueens/2, best = evaluateState();
	int t = 0, temp, i, val;
	do {
		t++;
		temp = timeToTemperature(t);
		if (temp == 0){
			break;
		}
		randomSuccessor(); /* neighbor[] = randomSuccessor(queens[]); */ 
		val = evaluateStateHill(2)-evaluateStateHill(1);
		if (val > 0){
			for (i=0; i<nqueens; i++){
				queens[i] = neighbor[i];
			}
		} else if (likelihood(val, temp)) {/* condition containing probability */
			for (i=0; i<nqueens; i++){
				queens[i] = neighbor[i];
			}
		}
		best = evaluateState();
	} while (best != optimum); /* runs until solution (global maximum) is found or T (temp) == 0 */
	printf("Final evaluation=%d\n", evaluateStateHill(1));
	printf ("Final state is");
	printState();
}

/*************************************************************/
/*********************Genetic Algorithm***********************/

int determineSimilarity (int p1, int p2){
	int i, ctr=0;
	switch (p1){ /* copying first array containing parent 1 into temporary 1 (to shorten code) */
		case 1: for (i=0; i < nqueens; i++){
					temporary1[i] = queens1[2];
				};	break;
		case 2: for (i=0; i < nqueens; i++){
					temporary1[i] = queens2[2];
				};	break;
		case 3: for (i=0; i < nqueens; i++){
					temporary1[i] = queens3[2];
				};	break;
		case 4: for (i=0; i < nqueens; i++){
					temporary1[i] = queens4[2];
				};	break;	
		case 5: for (i=0; i < nqueens; i++){
					temporary1[i] = queens5[2];
				};	break;
		case 6: for (i=0; i < nqueens; i++){
					temporary1[i] = queens6[2];
				};	break;
		case 7: for (i=0; i < nqueens; i++){
					temporary1[i] = queens7[2];
				};	break;
		case 8: for (i=0; i < nqueens; i++){
					temporary1[i] = queens8[2];
				};	break;
	}
	switch (p2){ /* copying array containing parent 2 into temporary 2 (to shorten code) */
		case 1: for (i=0; i < nqueens; i++){
					temporary2[i] = queens1[2];
				};	break;
		case 2: for (i=0; i < nqueens; i++){
					temporary2[i] = queens2[2];
				};	break;
		case 3: for (i=0; i < nqueens; i++){
					temporary2[i] = queens3[2];
				};	break;
		case 4: for (i=0; i < nqueens; i++){
					temporary2[i] = queens4[2];
				};	break;	
		case 5: for (i=0; i < nqueens; i++){
					temporary2[i] = queens5[2];
				};	break;
		case 6: for (i=0; i < nqueens; i++){
					temporary2[i] = queens6[2];
				};	break;
		case 7: for (i=0; i < nqueens; i++){
					temporary2[i] = queens7[2];
				};	break;
		case 8: for (i=0; i < nqueens; i++){
					temporary2[i] = queens8[2];
				};	break;
	}
	for (i=0; i < nqueens; i++){ /* counting matches (equal column numbers) between both potential parents */
			if (temporary1[i] == temporary2[i]){
				ctr++;
			}
	}
	return ctr;
}

void generateChild(int chi, int p1, int p2){
	initializeRandomGenerator();
	int cut = rand()%(nqueens-1), j;
	switch (chi){
		case 1:{ 			/* Child 1: copying the different parts up to and from a certain cut point on from parent 1 and 2 respectively */
			for (j=0; j < cut; j++){ /* copying parts of string from parent 1 to child up to cut point */
				switch (p1){
					case 1:	child1[j] = queens1[j];	break;
					case 2:	child1[j] = queens2[j];	break;
					case 3:	child1[j] = queens3[j];	break;
					case 4:	child1[j] = queens4[j];	break;
					case 5:	child1[j] = queens5[j];	break;
					case 6:	child1[j] = queens6[j];	break;
					case 7:	child1[j] = queens7[j];	break;
					case 8:	child1[j] = queens8[j];	break;
				}
			}
			for (; j < nqueens; j++){ /* copying parts of string from parent 1 to child from cut point on */
				switch (p2){
					case 1:	child1[j] = queens1[j];	break;
					case 2:	child1[j] = queens2[j];	break;
					case 3:	child1[j] = queens3[j];	break;
					case 4:	child1[j] = queens4[j];	break;
					case 5:	child1[j] = queens5[j];	break;
					case 6:	child1[j] = queens6[j];	break;
					case 7:	child1[j] = queens7[j];	break;
					case 8:	child1[j] = queens8[j];	break;
				}
			}	break;
		}
		case 2:{ 		
			for (j=0; j < cut; j++){
				switch (p1){
					case 1:	child2[j] = queens1[j];	break;
					case 2:	child2[j] = queens2[j];	break;
					case 3:	child2[j] = queens3[j];	break;
					case 4:	child2[j] = queens4[j];	break;
					case 5:	child2[j] = queens5[j];	break;
					case 6:	child2[j] = queens6[j];	break;
					case 7:	child2[j] = queens7[j];	break;
					case 8:	child2[j] = queens8[j];	break;
				}
			}
			for (; j < nqueens; j++){
				switch (p2){
					case 1:	child2[j] = queens1[j];	break;
					case 2:	child2[j] = queens2[j];	break;
					case 3:	child2[j] = queens3[j];	break;
					case 4:	child2[j] = queens4[j];	break;
					case 5:	child2[j] = queens5[j];	break;
					case 6:	child2[j] = queens6[j];	break;
					case 7:	child2[j] = queens7[j];	break;
					case 8:	child2[j] = queens8[j];	break;
				}
			}	break;
		}
		case 3:{ 		
			for (j=0; j < cut; j++){
				switch (p1){
					case 1:	child3[j] = queens1[j];	break;
					case 2:	child3[j] = queens2[j];	break;
					case 3:	child3[j] = queens3[j];	break;
					case 4:	child3[j] = queens4[j];	break;
					case 5:	child3[j] = queens5[j];	break;
					case 6:	child3[j] = queens6[j];	break;
					case 7:	child3[j] = queens7[j];	break;
					case 8:	child3[j] = queens8[j];	break;
				}
			}
			for (; j < nqueens; j++){
				switch (p2){
					case 1:	child3[j] = queens1[j];	break;
					case 2:	child3[j] = queens2[j];	break;
					case 3:	child3[j] = queens3[j];	break;
					case 4:	child3[j] = queens4[j];	break;
					case 5:	child3[j] = queens5[j];	break;
					case 6:	child3[j] = queens6[j];	break;
					case 7:	child3[j] = queens7[j];	break;
					case 8:	child3[j] = queens8[j];	break;
				}
			}	break;
		}
		case 4:{ 		
			for (j=0; j < cut; j++){
				switch (p1){
					case 1:	child4[j] = queens1[j];	break;
					case 2:	child4[j] = queens2[j];	break;
					case 3:	child4[j] = queens3[j];	break;
					case 4:	child4[j] = queens4[j];	break;
					case 5:	child4[j] = queens5[j];	break;
					case 6:	child4[j] = queens6[j];	break;
					case 7:	child4[j] = queens7[j];	break;
					case 8:	child4[j] = queens8[j];	break;
				}
			}
			for (; j < nqueens; j++){
				switch (p2){
					case 1:	child4[j] = queens1[j];	break;
					case 2:	child4[j] = queens2[j];	break;
					case 3:	child4[j] = queens3[j];	break;
					case 4:	child4[j] = queens4[j];	break;
					case 5:	child4[j] = queens5[j];	break;
					case 6:	child4[j] = queens6[j];	break;
					case 7:	child4[j] = queens7[j];	break;
					case 8:	child4[j] = queens8[j];	break;
				}
			}	break;
		}
		case 5:{ 		
			for (j=0; j < cut; j++){
				switch (p1){
					case 1:	child5[j] = queens1[j];	break;
					case 2:	child5[j] = queens2[j];	break;
					case 3:	child5[j] = queens3[j];	break;
					case 4:	child5[j] = queens4[j];	break;
					case 5:	child5[j] = queens5[j];	break;
					case 6:	child5[j] = queens6[j];	break;
					case 7:	child5[j] = queens7[j];	break;
					case 8:	child5[j] = queens8[j];	break;
				}
			}
			for (; j < nqueens; j++){
				switch (p2){
					case 1:	child5[j] = queens1[j];	break;
					case 2:	child5[j] = queens2[j];	break;
					case 3:	child5[j] = queens3[j];	break;
					case 4:	child5[j] = queens4[j];	break;
					case 5:	child5[j] = queens5[j];	break;
					case 6:	child5[j] = queens6[j];	break;
					case 7:	child5[j] = queens7[j];	break;
					case 8:	child5[j] = queens8[j];	break;
				}
			}	break;
		}
		case 6:{ 		
			for (j=0; j < cut; j++){
				switch (p1){
					case 1:	child6[j] = queens1[j];	break;
					case 2:	child6[j] = queens2[j];	break;
					case 3:	child6[j] = queens3[j];	break;
					case 4:	child6[j] = queens4[j];	break;
					case 5:	child6[j] = queens5[j];	break;
					case 6:	child6[j] = queens6[j];	break;
					case 7:	child6[j] = queens7[j];	break;
					case 8:	child6[j] = queens8[j];	break;
				}
			}
			for (; j < nqueens; j++){
				switch (p2){
					case 1:	child6[j] = queens1[j];	break;
					case 2:	child6[j] = queens2[j];	break;
					case 3:	child6[j] = queens3[j];	break;
					case 4:	child6[j] = queens4[j];	break;
					case 5:	child6[j] = queens5[j];	break;
					case 6:	child6[j] = queens6[j];	break;
					case 7:	child6[j] = queens7[j];	break;
					case 8:	child6[j] = queens8[j];	break;
				}
			}	break;
		}
		case 7:{ 		
			for (j=0; j < cut; j++){
				switch (p1){
					case 1:	child7[j] = queens1[j];	break;
					case 2:	child7[j] = queens2[j];	break;
					case 3:	child7[j] = queens3[j];	break;
					case 4:	child7[j] = queens4[j];	break;
					case 5:	child7[j] = queens5[j];	break;
					case 6:	child7[j] = queens6[j];	break;
					case 7:	child7[j] = queens7[j];	break;
					case 8:	child7[j] = queens8[j];	break;
				}
			}
			for (; j < nqueens; j++){
				switch (p2){
					case 1:	child7[j] = queens1[j];	break;
					case 2:	child7[j] = queens2[j];	break;
					case 3:	child7[j] = queens3[j];	break;
					case 4:	child7[j] = queens4[j];	break;
					case 5:	child7[j] = queens5[j];	break;
					case 6:	child7[j] = queens6[j];	break;
					case 7:	child7[j] = queens7[j];	break;
					case 8:	child7[j] = queens8[j];	break;
				}
			}	break;
		}
		case 8:{ 		
			for (j=0; j < cut; j++){
				switch (p1){
					case 1:	child8[j] = queens1[j];	break;
					case 2:	child8[j] = queens2[j];	break;
					case 3:	child8[j] = queens3[j];	break;
					case 4:	child8[j] = queens4[j];	break;
					case 5:	child8[j] = queens5[j];	break;
					case 6:	child8[j] = queens6[j];	break;
					case 7:	child8[j] = queens7[j];	break;
					case 8:	child8[j] = queens8[j];	break;
				}
			}
			for (; j < nqueens; j++){
				switch (p2){
					case 1:	child8[j] = queens1[j];	break;
					case 2:	child8[j] = queens2[j];	break;
					case 3:	child8[j] = queens3[j];	break;
					case 4:	child8[j] = queens4[j];	break;
					case 5:	child8[j] = queens5[j];	break;
					case 6:	child8[j] = queens6[j];	break;
					case 7:	child8[j] = queens7[j];	break;
					case 8:	child8[j] = queens8[j];	break;
				}
			}	break;
		}
	}
}

void mutateChild (int chi){
	int queen = (0+random() % (nqueens-1));
	int column = (0+random() % (nqueens-1));
	switch (chi){
		case 1:	child1[queen] = column;		break;
		case 2:	child2[queen] = column;		break;
		case 3:	child3[queen] = column;		break;
		case 4:	child4[queen] = column;		break;
		case 5:	child5[queen] = column;		break;
		case 6:	child6[queen] = column;		break;
		case 7:	child7[queen] = column;		break;
		case 8:	child8[queen] = column;		break;
	}
}

double fitnessFunction (int parent){
	switch (parent){
		case 1: return val1*3+(rand()%(3));
		case 2: return val2*3+(rand()%(3));
		case 3: return val3*3+(rand()%(3));
		case 4: return val4*3+(rand()%(3));
		case 5: return val5*3+(rand()%(3));
		case 6: return val6*3+(rand()%(3));
		case 7: return val7*3+(rand()%(3));
		case 8: return val8*3+(rand()%(3));
	}
	return 0; /* not necessary; to satisfy compiler */
}

int randomSelection(){
	initializeRandomGenerator();
	int parent = 1;
	double fitness = fitnessFunction(1), best = fitness;
	fitness = fitnessFunction(2); /* fitnessFunction returns already the [evaluation value of the parents]*[random value] */
	if (fitness > best){
		parent = 2;
		best = fitness;
	}
	fitness = fitnessFunction(3);
	if (fitness > best){
		parent = 3;
		best = fitness;
	}
	fitness = fitnessFunction(4);
	if (fitness > best){
		parent = 4;
		best = fitness;
	}
	fitness = fitnessFunction(5);
	if (fitness > best){
		parent = 5;
		best = fitness;
	}
	fitness = fitnessFunction(6);
	if (fitness > best){
		parent = 6;
		best = fitness;
	}
	fitness = fitnessFunction(7);
	if (fitness > best){
		parent = 7;
		best = fitness;
	}
	fitness = fitnessFunction(8);
	if (fitness > best){
		parent = 8;
		best = fitness;
	}
	return parent;
}

double fitnessFunctionSecond (int p1, int p2){
	initializeRandomGenerator();
	switch (p2){
		case 1: return (val1*2+determineSimilarity(p1, 1)*2)+(rand()%(2));
		case 2: return (val2*2+determineSimilarity(p1, 2)*2)+(rand()%(2));
		case 3: return (val3*2+determineSimilarity(p1, 3)*2)+(rand()%(2)); /* evaluation of state (second parent) has already been done and is stored in valX */
		case 4: return (val4*2+determineSimilarity(p1, 4)*2)+(rand()%(2));
		case 5: return (val5*2+determineSimilarity(p1, 5)*2)+(rand()%(2));
		case 6: return (val6*2+determineSimilarity(p1, 6)*2)+(rand()%(2));
		case 7: return (val7*2+determineSimilarity(p1, 7)*2)+(rand()%(2));
		case 8: return (val8*2+determineSimilarity(p1, 8)*2)+(rand()%(2));
	}
	return 0; /* not necessary; to satisfy compiler */
}


int randomSelectionSecond (int p1){
	/* Parent 1 has already been determined. Now, this function searches
	 * for the second parent: It works as follows: Which parent gets the
	 * best fitness value, gets selcted as the second parent. The fitness
	 * is determined as follows: First, we add the [evaluation value of 
	 * a state] times [factor 2], the [similiarity between parent one and the
	 * different candidates] for being parent2 times [factor 2], and a 
	 * random value between 0 and 4. That sum of the quality of a state 
	 * ((evaluation + similiarity)*2) and some randomness is the base for
	 * selecting parent 2.  
	 */
	initializeRandomGenerator();
	int parent = 1;
	double fitness = fitnessFunctionSecond(p1, 1), best = fitness;
	fitness = fitnessFunctionSecond(p1, 2); /* fitnessFunction returns already the [evaluation value of the parents]*[random value] */
	if (fitness > best){
		parent = 2;
		best = fitness;
	}
	fitness = fitnessFunctionSecond(p1, 3);
	if (fitness > best){
		parent = 3;
		best = fitness;
	}
	fitness = fitnessFunctionSecond(p1, 4);
	if (fitness > best){
		parent = 4;
		best = fitness;
	}
	fitness = fitnessFunctionSecond(p1, 5);
	if (fitness > best){
		parent = 5;
		best = fitness;
	}
	fitness = fitnessFunctionSecond(p1, 6);
	if (fitness > best){
		parent = 6;
		best = fitness;
	}
	fitness = fitnessFunctionSecond(p1, 7);
	if (fitness > best){
		parent = 7;
		best = fitness;
	}
	fitness = fitnessFunctionSecond(p1, 8);
	if (fitness > best){
		parent = 8;
		best = fitness;
	}
	return parent;
}

void printStateGenetic(int best){ 
	/* Function first copies the best string found into queens-array, 
	 * from where it gets printed via the normal printState()-function.
	 * That is some extra computations needed, however at most 100 iterations
	 * to copy the whole array (MAXQ-size). Advantage: Not even lengthier code because 
	 * we employ only one print-function containing one array and not the same
	 * code 8 times, just for 8 different arrays. That makes the code significantly
	 * shorter and looks much cleaner from our point of view.
	 */ 
	int i;
	if (best == val1){
		for (i=0; i < nqueens; i++){
			queens[i] = queens1[i];
		}
	} else if (best == val2){
		for (i=0; i < nqueens; i++){
			queens[i] = queens2[i];
		}
	} else if (best == val3){
		for (i=0; i < nqueens; i++){
			queens[i] = queens3[i];
		}
	} else if (best == val4){
		for (i=0; i < nqueens; i++){
			queens[i] = queens4[i];
		}
	} else if (best == val5){
		for (i=0; i < nqueens; i++){
			queens[i] = queens5[i];
		}
	} else if (best == val6){
		for (i=0; i < nqueens; i++){
			queens[i] = queens6[i];
		}
	} else if (best == val7){
		for (i=0; i < nqueens; i++){
			queens[i] = queens7[i];
		}
	} else if (best == val8){
		for (i=0; i < nqueens; i++){
			queens[i] = queens8[i];
		}
	}
	printState();
}


void geneticAlgorithm(){
	int optimum = (nqueens-1)*nqueens/2;
	int ctr=10000, i, j, x, y, best = evaluateStateGenetic();
	while (optimum != best && ctr > 0){
		ctr--; /* a maximum of 10000 iterations */
		for (i=0; i < k; i++){ /* for the number of k=8 children */
			x = randomSelection();
			y = randomSelectionSecond(x);
			generateChild(i, x, y);
			if ((rand() % 1) > 0.85){ /* determines whether a child gets mutated */
				 for (j=0; j < (1+rand()%(3-1)); j++){ /* determines the number of mutations in the string */
					 mutateChild(i);
				 }
			}
		}
		for (i=0; i < nqueens; i++){ /* establishing new population by making children becoming new population */
			queens1[i] = child1[i];
			queens2[i] = child2[i];
			queens3[i] = child3[i];
			queens4[i] = child4[i];
			queens5[i] = child5[i];
			queens6[i] = child6[i];
			queens7[i] = child7[i];
			queens8[i] = child8[i];
		}
		best = evaluateStateGenetic(); /* evaluates all new parents and saves results in the global variables valX */
	}
	printf("final evaluation: %d\n", best);
	printf ("Final state is");
	printStateGenetic(best);
}


int main(int argc, char *argv[]) {
  int algorithm;

  do {
    printf ("Number of queens (1<=nqueens<%d): ", MAXQ);
    scanf ("%d", &nqueens);
  } while ((nqueens < 1) || (nqueens > MAXQ));

  do {
    printf ("Algorithm: (1) Random search  (2) Hill climbing  ");
    printf ("(3) Simulated Annealing  (4) Genetic Algo.: ");
    scanf ("%d", &algorithm);
  } while ((algorithm < 1) || (algorithm > 4));
  initializeRandomGenerator();
  if (algorithm == 4){
	  initiateQueensGenetic(1);
  } else {
	initiateQueens(1);
  }
  
  if (algorithm < 4){
	printf("\nInitial state:");
	printState();
  }

  switch (algorithm) {
  case 1: randomSearch();       break;
  case 2: hillClimbing();       break;
  case 3: simulatedAnnealing(); break;
  case 4: geneticAlgorithm(); 	break;
  }

  return 0;  
}
