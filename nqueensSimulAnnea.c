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


int main(int argc, char *argv[]) {
  int algorithm;

  do {
    printf ("Number of queens (1<=nqueens<%d): ", MAXQ);
    scanf ("%d", &nqueens);
  } while ((nqueens < 1) || (nqueens > MAXQ));

  do {
    printf ("Algorithm: (1) Random search  (2) Hill climbing  ");
    printf ("(3) Simulated Annealing: ");
    scanf ("%d", &algorithm);
  } while ((algorithm < 1) || (algorithm > 3));
  
  initializeRandomGenerator();

  initiateQueens(1);
  
  printf("\nInitial state:");
  printState();

  switch (algorithm) {
  case 1: randomSearch();       break;
  case 2: hillClimbing();       break;
  case 3: simulatedAnnealing(); break;
  }

  return 0;  
}
