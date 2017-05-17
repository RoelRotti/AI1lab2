#include <stdio.h>
#include <stdlib.h>

#define MAX 0
#define MIN 1

#define INFINITY 9999999

int valuation;

int minValue(int state); /* forward declaration: mutual recursion */


int negamax (int state, int turn) {
	int move, bestmove, max, min;
	if (state == 1) {
		if (turn == MAX){
			valuation = -1;
			return -1; /* Min wins if max is in a terminal state */
		} else {
			valuation = 1;
			printf("Max wins, MIN looses\n");
			return 1;
		}
		printf("Error, shouldn't happen\n");
	}
	if (turn == MAX) {
		max = -INFINITY;
		for (move = 1; move <= 3; move++) {
		  if (state - move > 0) { /* legal move */
			int m = negamax(state - move, 1);
			if (m > max) {
			//	printf("Move: %d State: %d m: %d\n", move, state, m);
			  max = m;
			  bestmove = move;
			}
		  }
		}
		
	} else if (turn == MIN) {
		 min = INFINITY;
		 for (move = 1; move <= 3; move++) {
			if (state - move > 0) { /* legal move */
			  int m = negamax(state - move, 0);
			  if (m < min) {
				min = m;
				bestmove = move;
			  }
			}
		  }
	}
	return bestmove;
}

int maxValue(int state) {
  int move, max = -INFINITY;
  /* terminal state ? */
  if (state == 1) {
    return -1; /* Min wins if max is in a terminal state */
  }
  /* non-terminal state */
  for (move = 1; move <= 3; move++) {
    if (state - move > 0) { /* legal move */
      int m = minValue(state - move);
      if (m > max) max = m;
    }
  }
  return max;
}

int minValue(int state) {
  int move, min = INFINITY;
  /* terminal state ? */
  if (state == 1) {
    return 1; /* Max wins if min is in a terminal state */
  } 
  /* non-terminal state */  
  for (move = 1; move <= 3; move++) {
    if (state - move > 0) { /* legal move */
      int m = maxValue(state - move);
      if (m < min) min = m;
    }
  }
  return min;
}

int minimaxDecision(int state, int turn) {
  int move, bestmove, max, min;
  if (turn == MAX) {
    max = -INFINITY;
    for (move = 1; move <= 3; move++) {
      if (state - move > 0) { /* legal move */
        int m = minValue(state - move);
        if (m > max) {
          max = m;
          bestmove = move;
        }
      }
    }
    return bestmove;
  }
  /* turn == MIN */
  min = INFINITY;
  for (move = 1; move <= 3; move++) {
    if (state - move > 0) { /* legal move */
      int m = maxValue(state - move);
      if (m < min) {
        min = m;
        bestmove = move;
      }
    }
  }
  return bestmove;
}

void playNim(int state, int algo) {
  int turn = 0;
  if (algo == 2){
	  while (state != 1) {
		int action = minimaxDecision(state, turn);
		printf("%d: %s takes %d\n", state, 
			   (turn==MAX ? "Max" : "Min"), action);
		state = state - action;
		turn = 1 - turn;
	  }
  } else {
	  while (state != 1) {
		int action = negamax(state, turn);
		printf("%d: %s takes %d\n", state, 
			   (turn==MAX ? "Max" : "Min"), action);
		state = state - action;
		turn = 1 - turn;
	  }
  }
  printf("1: %s looses\n", (turn==MAX ? "Max" : "Min"));
}

int main(int argc, char *argv[]) {
  if ((argc != 2) || (atoi(argv[1]) < 3)) {
    fprintf(stderr, "Usage: %s <number of sticks>, where ", argv[0]);
    fprintf(stderr, "<number of sticks> must be at least 3!\n");
    return -1;
  }
  int a;
  do{
	  printf("Choose: NEGAMAX() = 1; playNim() = 2: ");
	  scanf("%d", &a);
  } while (a != 1 && a != 2); 
  playNim(atoi(argv[1]), a);
  return 0;
}
