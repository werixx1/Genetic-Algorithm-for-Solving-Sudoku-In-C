#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <string.h> 
#include "load_sudoku.h"
#include "utils.h"
#include "ga.h"
#include "ga_others.h"

int main() 
{
    int size = 9; 
    int empty_cells = 40; 
    
    int** sudoku = generate_sudoku(size, empty_cells);

    printf("\n-- SUDOKU TO SOLVE (empty cells = %d) --\n", empty_cells);
    print_board(size, sudoku);
    
    printf("\n-- STARTING GENETIC ALGORITHM --\n\n");
    int** solution = genetic_algorithm_solve_sudoku(sudoku, size);
    
    printf("\n\t-- SOLVED SUDOKU: --\n");
    print_board(size, solution);
    
    // checking if solution is correct (calculating num of conflicts for best invidual)
    Individual final_check;
    final_check.solution = solution;
    calculate_fitness(&final_check, size);
    // if fitness == 0 (meaning there are no conflicting numbers in any row, column or square)
    printf("\nFitness of returned solution: %d (0 means solved)\n\n", final_check.fitness);

    free_board(sudoku, size);
    free_board(solution, size); 

    return 0;
}
