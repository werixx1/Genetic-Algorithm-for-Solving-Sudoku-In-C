#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h> 
#include "load_sudoku.h"
#include "utils.h"

int main()
{
    int size = 9;
    printf("-------- ORIGINAL BOARD ---------\n");
    int** board =  generate_sudoku_with_solution(size, 40);
    print_board(size, board);
 
    // removes k-digits from the board for ga algorithm to solve
    int **board_to_solve = return_unsolved_sudoku(board);
    print_board(size, board_to_solve);

    return 0;
}