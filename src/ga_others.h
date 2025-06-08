#ifndef GA_OTHERS_H
#define GA_OTHERS_H
#include "ga.h"

Individual tournament_selection(Individual *population, int population_size);
Individual roulette_selection(Individual *population, int population_size);
void single_point_crossover(Individual parent1, Individual parent2, Individual *child1, Individual *child2, 
    int size, int **original_sudoku);
void two_point_crossover(Individual parent1, Individual parent2, Individual *child1, 
    Individual *child2, int size, int **original_sudoku);
void uniform_crossover(Individual parent1, Individual parent2, Individual *child1, 
    Individual *child2, int size, int **original_sudoku);
void square_swap_mutation(Individual *individual, int **original_sudoku, int size);
void square_swap_mutation(Individual *individual, int **original_sudoku, int size);
void random_reset_mutation(Individual *individual, int **original_sudoku, int size);

#endif