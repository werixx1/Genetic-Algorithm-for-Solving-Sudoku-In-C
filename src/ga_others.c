#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <string.h>
#include "ga.h"
#include "load_sudoku.h"


// TOURNAMENT SELECTION
Individual tournament_selection(Individual *population, int population_size) 
{
    Individual best_in_tournament;
    best_in_tournament.fitness = -9999999; // random small number for start 
    
    for (int i = 0; i < TOURNAMENT_SIZE; i++) // 5 times
    {
        int idx = rand() % population_size; // drawing a random invidual
        if (population[idx].fitness > best_in_tournament.fitness) // if it has better fitness 
                                        //(than previously assigned) -> becomes temporary best solution
        {
            best_in_tournament = population[idx]; 
        }
    }
    return best_in_tournament;
}

// ROULETTE SELECTION
Individual roulette_selection(Individual *population, int population_size) 
{
    long long total_fitness_shifted = 0; // initializing shift
    int min_fitness = 0;  // initializing fitness

    for(int i=0; i < population_size; ++i) 
    {
        if(population[i].fitness < min_fitness) // if an invidual has smaller fitness -> it becomes current fitness
        {
            min_fitness = population[i].fitness;
        }
    }

    // shifting so that all fitnesses are above 0
    // example: min_fitness = -200, shift_val = 201, fitness becomes 1
    int shift_val = -min_fitness + 1; // calculating shift ^

    for (int i = 0; i < population_size; i++) 
    {
        total_fitness_shifted += (long long)(population[i].fitness + shift_val); // sum of all fitnesses (after shifting)
    }
    
    if (total_fitness_shifted == 0) 
    {
        // if all inviduals have the min possible fitness return random one
        return population[rand() % population_size];
    }

    // random really big value
    unsigned long long random_val_ull = (unsigned long long)rand() * (unsigned long long)RAND_MAX + 
    (unsigned long long)rand();

    unsigned long long random_val = random_val_ull % total_fitness_shifted; // making this num fit this range: [0, total_fitness_shifted]
    
    long long running_sum = 0;
    for (int i = 0; i < population_size; i++) // iterating through inviduals 
    {
        running_sum += (population[i].fitness + shift_val);
        if (running_sum > (long long)random_val) // untill the sum is bigger than random_val -> best invidual is returned
        { 
            return population[i];
        }
    }
    
    return population[population_size - 1]; // if something doesnt work -> return the last invidual
}


// SINGLE POINT CROSSOVER - one cutting point
// child1 gets genes form parent1 and parent2, child2 the same thing but opposite
void single_point_crossover(Individual parent1, Individual parent2, Individual *child1, 
    Individual *child2, int size, int **original_sudoku) 
{
    int crossover_point = rand() % (size * size); // drawing random cutting point
    
    // k - num of column
    for (int k = 0; k < size * size; k++) 
    {
        int r = k / size; // row
        int c = k % size; // column

        if (original_sudoku[r][c] != 0) // cell that cant be modified
        { 
            child1->solution[r][c] = original_sudoku[r][c];
            if (child2) child2->solution[r][c] = original_sudoku[r][c];
        } 
        else 
        { // cell that can be modified
            if (k < crossover_point) 
            {
                child1->solution[r][c] = parent1.solution[r][c]; // child1 gets parents1 value
                if (child2) child2->solution[r][c] = parent2.solution[r][c]; // (if child2 exists) gets parent2 value
            } 
            else // same thing but in opposite way
            {
                child1->solution[r][c] = parent2.solution[r][c];
                if (child2) child2->solution[r][c] = parent1.solution[r][c];
            }
        }
    }
}

// TWO POINT CROSSOBVER - two cutting points
// child1 gets the middle (from point1 to point2) from parent2 and rest from parent1
// child2 the same thing but opposite
void two_point_crossover(Individual parent1, Individual parent2, Individual *child1,
     Individual *child2, int size, int **original_sudoku) 
    {
    int point1 = rand() % (size * size);
    int point2 = rand() % (size * size);
    // drawing two random points and crossing in the middle
    if (point1 > point2) 
    {
        int temp = point1;
        point1 = point2;
        point2 = temp;
    }
    
    for (int k = 0; k < size * size; k++) 
    {
        int r = k / size;
        int c = k % size;

        if (original_sudoku[r][c] != 0) // cell that cant be modified
        { 
            child1->solution[r][c] = original_sudoku[r][c];
            if (child2) child2->solution[r][c] = original_sudoku[r][c];
        } 
        else 
        { // cell that can be modified
            if (k < point1 || k >= point2) 
            { 
                child1->solution[r][c] = parent1.solution[r][c];
                if (child2) child2->solution[r][c] = parent2.solution[r][c];
            } 
            else 
            { 
                child1->solution[r][c] = parent2.solution[r][c];
                if (child2) child2->solution[r][c] = parent1.solution[r][c];
            }
        }
    }
}

// UNIFORM CROSSOVER - random decisions for each cell
void uniform_crossover(Individual parent1, Individual parent2, Individual *child1, 
    Individual *child2, int size, int **original_sudoku) 
    {
    for (int i = 0; i < size; i++) 
    {
        for (int j = 0; j < size; j++) 
        {
            if (original_sudoku[i][j] != 0) 
            { 
                child1->solution[i][j] = original_sudoku[i][j];
                if (child2) child2->solution[i][j] = original_sudoku[i][j];
            } 
            else 
            { 
                if (rand() % 2 == 0) 
                {
                    child1->solution[i][j] = parent1.solution[i][j];
                    if (child2) child2->solution[i][j] = parent2.solution[i][j];
                } 
                else 
                {
                    child1->solution[i][j] = parent2.solution[i][j];
                    if (child2) child2->solution[i][j] = parent1.solution[i][j];
                }
            }
        }
    }
}
 
// SQUARE SWAP MUTATION if squares are alredy correct -> randomly switch two numbers
void square_swap_mutation(Individual *individual, int **original_sudoku, int size) 
{
    if ((double)rand() / RAND_MAX > 0.1) return; // 10% chance of mutation occuring

    int square_root = (int)sqrt(size);
    
    // choosing random square
    int box_row_start = (rand() % square_root) * square_root;
    int box_col_start = (rand() % square_root) * square_root;

    // look for cells that can be mutated (used to be empty)
    int mutable_cells_r[square_root * square_root];
    int mutable_cells_c[square_root * square_root];
    int count = 0;
    
    // iterating through square
    for (int i = 0; i < square_root; i++) 
    {
        for (int j = 0; j < square_root; j++) 
        {
            if (original_sudoku[box_row_start + i][box_col_start + j] == 0) // checking for empty places
            { 
                // saving these places in two arrays
                mutable_cells_r[count] = box_row_start + i; 
                mutable_cells_c[count] = box_col_start + j;
                count++;
            }
        }
    }
    
    // switch random nums in cell
    if (count > 1) 
    {
        int idx1 = rand() % count;
        int idx2 = rand() % count;
        while (idx1 == idx2) // making sure theyre different nums
        { 
            idx2 = rand() % count;
        }
        
        int r1 = mutable_cells_r[idx1]; int c1 = mutable_cells_c[idx1];
        int r2 = mutable_cells_r[idx2]; int c2 = mutable_cells_c[idx2];

        // swap
        int temp = individual->solution[r1][c1];
        individual->solution[r1][c1] = individual->solution[r2][c2];
        individual->solution[r2][c2] = temp;
    }
}

// RANDOM RESET MUTATION - randomly write a num over a solution
void random_reset_mutation(Individual *individual, int **original_sudoku, int size)
 {
    for (int i = 0; i < size; i++) 
    {
        for (int j = 0; j < size; j++) 
        {
            // mutate only if a place used to be empty (0) 
            if (original_sudoku[i][j] == 0 && (double)rand() / RAND_MAX < MUTATION_RATE) 
            {
                individual->solution[i][j] = (rand() % size) + 1;
            }
        }
    }
}

//void square_swap_mutation(Individual *individual, int **original_sudoku, int size) {
//    for (int i = 0; i < size; i++) {
//        for (int j = 0; j < size; j++) {
//
//            if (original_sudoku[i][j] == 0 && (double)rand() / RAND_MAX < MUTATION_RATE) {
//                individual->solution[i][j] = (rand() % size) + 1;
//            }
//        }
//    }
//}