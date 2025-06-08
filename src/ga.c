#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include "ga.h"
#include "ga_others.h"
#include "load_sudoku.h"


// INITIALIZE POPULATION
Individual* initialize_population(int **sudoku_problem, int size, int population_size) 
{
    Individual *population = malloc(population_size * sizeof(Individual));

    for (int p = 0; p < population_size; p++)
    {
        population[p].solution = allocate_board(size); // temporary solution (unsolved board)
        copy_board(sudoku_problem, population[p].solution, size); // copy unsolved sudoku ^
        
        // full empty cells with random nums
        for (int i = 0; i < size; i++) 
        {
            for (int j = 0; j < size; j++) 
            {
                if (population[p].solution[i][j] == 0) 
                { 
                    population[p].solution[i][j] = (rand() % size) + 1;
                }
            }
        }
        population[p].fitness = 0; // temporary fitness
    }
    return population;
}


// CALCULATE FITNESS - fitness being number of conflists, the less, the closer algorithm is to solution
// example: fitness: -2 -> 2 same numbers in one row / column / square (2 mistakes in solution)
void calculate_fitness(Individual *individual, int size) 
{
    int conflicts = 0;
    int square_root = (int)sqrt(size);
    
    // checking for conflicts in rows
    for (int i = 0; i < size; i++) 
    {
        int count[size + 1]; 
        memset(count, 0, sizeof(count));
        for (int j = 0; j < size; j++) 
        {
            if (individual->solution[i][j] != 0) 
            {
                 count[individual->solution[i][j]]++;
            }
        }
        for (int k = 1; k <= size; k++)
         {
            if (count[k] > 1) 
            {
                conflicts += (count[k] - 1);
            }
        }
    }
    
    // checking for conflicts in columns
    for (int j = 0; j < size; j++) 
    {
        int count[size + 1]; // counter for times a num appears
        memset(count, 0, sizeof(count));
        for (int i = 0; i < size; i++)
         {
             if (individual->solution[i][j] != 0) 
            {
                count[individual->solution[i][j]]++;
            }
        }
        for (int k = 1; k <= size; k++) 
        {
            if (count[k] > 1) // if num appears more than once
            {
                conflicts += (count[k] - 1); // conflict happens
            }
        }
    }
    
    // chcecking for conflicts in squares
    for (int r_box = 0; r_box < size; r_box += square_root) 
    {
        for (int c_box = 0; c_box < size; c_box += square_root) 
        {
            int count[size + 1];
            memset(count, 0, sizeof(count));
            for (int i = 0; i < square_root; i++) 
            {
                for (int j = 0; j < square_root; j++) 
                {
                    if (individual->solution[r_box + i][c_box + j] != 0)
                    {
                        count[individual->solution[r_box + i][c_box + j]]++;
                    }
                }
            }
            for (int k = 1; k <= size; k++) 
            {
                if (count[k] > 1) 
                {
                    conflicts += (count[k] - 1);
                }
            }
        }
    }
    
    individual->fitness = -conflicts; // fitness being the (negative) number of conflicts, 0 means no conflicts
}

// fuction that compares individuals (for qsort)
int compare_individuals(const void *a, const void *b) 
{
    Individual *ind1 = (Individual *)a;
    Individual *ind2 = (Individual *)b;
    return ind2->fitness - ind1->fitness; // sort by fitness (descending) 
}


// GA FOR SOLVING SUDOKU
int** genetic_algorithm_solve_sudoku(int **sudoku_problem, int size) 
{
    srand(time(NULL));
    
    Individual *population = initialize_population(sudoku_problem, size, POPULATION_SIZE);
    
    for (int i = 0; i < POPULATION_SIZE; i++) // calculate fitness for whole population
    {
        calculate_fitness(&population[i], size);
    }
    qsort(population, POPULATION_SIZE, sizeof(Individual), compare_individuals); // sort descending
    
    int generation = 0;
    while (generation < MAX_GENERATIONS && population[0].fitness < 0) // iterating through all generations untill fitness is 0 
    // (solution without conflics found)
    {
        int elite_count = POPULATION_SIZE * ELITISM_RATE; // calculating how many individuals have to be left alone (elites)
        if (elite_count == 0 && POPULATION_SIZE > 0) elite_count = 1; // if elite_count = 0 -> elite_count = 1
        if (elite_count > POPULATION_SIZE) elite_count = POPULATION_SIZE; // making sure there is not more elites than
                                                                         // inviduals in population

        Individual *new_population = malloc(POPULATION_SIZE * sizeof(Individual)); // new array of inviduals

        // copying the elites to new population
        for (int i = 0; i < elite_count; i++) 
        {
            new_population[i].solution = allocate_board(size);
            copy_board(population[i].solution, new_population[i].solution, size);
            new_population[i].fitness = population[i].fitness;
        }
        
        for (int i = elite_count; i < POPULATION_SIZE; ) 
        {
            // new parents
            Individual parent1 = tournament_selection(population, POPULATION_SIZE); // or roulette_selection
            Individual parent2 = tournament_selection(population, POPULATION_SIZE); // or roulette_selection
            
            // board for new child
            new_population[i].solution = allocate_board(size);
            Individual *child2_ptr = NULL; // null for now

            if (i + 1 < POPULATION_SIZE) // if theres is still room for second child in populaton
            {
                new_population[i+1].solution = allocate_board(size);
                child2_ptr = &new_population[i+1];
            }
            
            int crossover_method = rand() % 3; // randomly choose crossover method
            switch (crossover_method) 
            {
                case 0:
                    single_point_crossover(parent1, parent2, &new_population[i], child2_ptr, size, sudoku_problem);
                    break;
                case 1:
                    two_point_crossover(parent1, parent2, &new_population[i], child2_ptr, size, sudoku_problem);
                    break;
                case 2:
                    uniform_crossover(parent1, parent2, &new_population[i], child2_ptr, size, sudoku_problem);
                    break;
            }
            
            // mutation
            if ((double)rand() / RAND_MAX < 0.7) // 70% chance for mutation occuring
            { 
                int mutation_method = rand() % 2; // randomly choose mutation method
                // (and use it on children (or child1 if child2 doesnt exist))
                if (mutation_method == 0) 
                {
                    square_swap_mutation(&new_population[i], sudoku_problem, size);
                    if (child2_ptr) square_swap_mutation(child2_ptr, sudoku_problem, size);
                } 
                else 
                {
                    random_reset_mutation(&new_population[i], sudoku_problem, size);
                    if (child2_ptr) random_reset_mutation(child2_ptr, sudoku_problem, size);
                }
            }
            
            // calculate fitness for children
            calculate_fitness(&new_population[i], size);
            if (child2_ptr) 
            {
                calculate_fitness(child2_ptr, size);
                i += 2; // if two chidren
            } else 
            {
                i += 1;
            }
        }
        
        for (int j = 0; j < POPULATION_SIZE; j++) 
        {
            free_board(population[j].solution, size);
        }
        free(population);
        
        // making new_population current one and sorting by fitness
        population = new_population;
        qsort(population, POPULATION_SIZE, sizeof(Individual), compare_individuals);
        
        // moving to next generation
        generation++;
        //if (generation % 100 == 0 || population[0].fitness == 0) { 
            printf("Generation %d, Best fitness: %d\n", generation, population[0].fitness);
        //}
    }
    
    // copying best solution
    int **best_solution = allocate_board(size);
    copy_board(population[0].solution, best_solution, size);
    printf("Final best fitness: %d after %d generations.\n", population[0].fitness, generation);
    
    for (int i = 0; i < POPULATION_SIZE; i++) 
    {
        free_board(population[i].solution, size);
    }
    free(population);
    
    return best_solution;
}