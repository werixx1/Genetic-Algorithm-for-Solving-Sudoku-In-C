#ifndef GA_H
#define GA_H

#define POPULATION_SIZE 70000 // just to be sure hehe
#define MAX_GENERATIONS 3000
#define ELITISM_RATE 0.1
#define MUTATION_RATE 0.05 // used in random_reset_mutation
#define TOURNAMENT_SIZE 5

// structure that holds one sudoku solution and how well it solves it (fitness)
typedef struct {
    int **solution;
    int fitness;
} Individual;


Individual* initialize_population(int **sudoku_problem, int size, int population_size);
void calculate_fitness(Individual *individual, int size);
int compare_individuals(const void *a, const void *b);
int** genetic_algorithm_solve_sudoku(int **sudoku_problem, int size);


#endif