#include <stdio.h>
#include <pthread.h>
#include <stdlib.h>
#include "lab2.h"

int** read_board_from_file(char* filename){
    FILE* fp = NULL;
    int** board = NULL;

    fp = fopen(filename,"r");
    if( fp == NULL ) {
      perror("Error: ");
      return 0;
   }
    board = (int** )malloc(ROW_SIZE * sizeof(int*));
    for (int i = 0; i < ROW_SIZE; i++){
        board[i] = (int*)malloc(COL_SIZE * sizeof(int));
    }
    
    for (int row = 0; row<ROW_SIZE;row++){
        for(int col = 0; col < COL_SIZE; col++){
            if (col == COL_SIZE - 1){
                fscanf(fp,"%d", &(board[row][col]));
                // printf("row: %d, col: %d  value:%d\n", row, col, board[row][col]); //testing
            }else{
                fscanf(fp,"%d,", &(board[row][col]));
                // printf("row: %d, col: %d  value:%d\n", row, col, board[row][col]); //testing
            }
        }
    }
    fclose(fp);
    return board;
}

void* isValid_row(void* param){
    printf("Hello, world");
    param_struct* params = (param_struct*) param;
    int count[COL_SIZE] = {0};
    int i = 0;
    int id = params->id;
    int col = params->starting_col;
    printf("(%d, %d)", id, col);
    printf("Hello, world");
    for(i = 0; i < COL_SIZE; i++){
        count[(sudoku_board[i][col]-1)] += 1;
    }
    for(i = 0; i < COL_SIZE; i++){
        if (count[i] > 1){
            worker_validation[id] = 0;
            pthread_exit(NULL);
        }
    }
    worker_validation[id] = 1;
    pthread_exit(NULL);
}

void* isValid_Col(void* param){
    param_struct* params = (param_struct*) param;
    int count[COL_SIZE] = {0};
    int id = params->id;
    int row = params->starting_row;
    int i = 0;
    for(i = 0; i < ROW_SIZE; i++){
        count[sudoku_board[row][i]-1] += 1;
    }
    for(i = 0; i < COL_SIZE; i++){
        if (count[i] > 1){
            worker_validation[id] = 0;
            pthread_exit(NULL);
        }
    }
    worker_validation[id] = 1;
    pthread_exit(NULL);
}


void* isValid_Grid(void* param){
    param_struct* params = (param_struct*) param;
    int count[COL_SIZE] = {0};
    int row,col;
    int id = params->id;
    int startRow = params->starting_row;
    int startCol = params->starting_col;
    int endingRow = params->ending_row;
    int endingCol = params->ending_col;

    for(row = startRow; row < endingRow; row++){
        for(col = startCol; col < endingCol; col++){
            count[sudoku_board[row][col]-1] += 1;
        }
    }
    for(int i = 0; i < COL_SIZE; i++){
        if (count[i] > 1){
            worker_validation[id] = 0;
            pthread_exit(NULL);
        }
    }
    worker_validation[id] = 1;
    pthread_exit(NULL);
}

// void* test(void* u){
//     printf("Test function");
// }
// void* makeRowThread(param_struct* iparameters,int item){
//     iparameters.starting_row = 0;
//     iparameters.starting_col = x;
//     iparameters.ending_row = 0;
//     iparameters.ending_col = x;
//     pthread_create(&(tid[x], &attr, isValid_row, &parametter));
// }
// void* makeColThread(){}
// void* makeGridThread(){}

int is_board_valid(){
    pthread_t* tid;  /* the thread identifiers */
    pthread_attr_t attr;
    param_struct* parameters;

    tid = (pthread_t*)malloc(sizeof(int)* NUM_OF_THREADS);
    parameters = (param_struct*)malloc(sizeof(param_struct)* NUM_OF_THREADS);
    worker_validation = (int*)malloc(NUM_OF_THREADS * sizeof(int));

    int row, col;
    int item = 0;
    int temp = 0;
    int cnt = 0;
    int grid_position[3][2] = {{0,2},{3,5},{6,8}};
    // for (item = 0; item < COL_SIZE; item++){
    //     makeRowThread();
    //     makeColThread();
    //     makeGridThread();
    // }
    for (item = 0; item < (ROW_SIZE + COL_SIZE); item++){
        if (item < ROW_SIZE){
            parameters[item].id = item;
            parameters[item].starting_row = 0;
            parameters[item].starting_col = item;
            parameters[item].ending_row = 0;
            parameters[item].ending_col = item;
            printf("%d , %d\n", parameters[item].starting_col, parameters[item].id);
            /* Now create the thread passing it params[0] as a parameter */
            //pthread_create(&(tid[item]), &attr, isValid_row, &(parameters[item]));
            if(pthread_create(&(tid[item]), &attr, &isValid_row, &(parameters[item]))){
                perror("ERROR creating thread.");
            }
        } 
        else {
            parameters[item].id = item;
            temp = item % COL_SIZE;
            parameters[item].starting_row = temp;
            parameters[item].starting_col = 0;
            parameters[item].ending_row = temp;
            parameters[item].ending_col = 0;
            printf("%d , %d\n", parameters[item].starting_col, parameters[item].id);
            /* Now create the thread passing it params[0] as a parameter */
            //pthread_create(&(tid[item]), &attr, isValid_Col, &(parameters[item]));
            if(pthread_create(&(tid[item]), &attr, &isValid_row, &(parameters[item]))){
                perror("ERROR creating thread.");
            } 
        }
    }
    for(row = 0; row < 3;row++){
        for(col = 0; col < 3; col++){}
            parameters[item].id = item;
            parameters[item].starting_row = grid_position[item];
            parameters[item].starting_col = cnt;
            parameters[item].ending_row = ;
            parameters[item].ending_col = grid_position[item];
            printf("%d , %d\n", parameters[item].starting_col, parameters[item].id);
            /* Now create the thread passing it params[0] as a parameter */
            //pthread_create(&(tid[item]), &attr, isValid_Grid, &(parameters[item]));
            if(pthread_create(&(tid[item]), &attr, &isValid_row, &(parameters[item]))){
                perror("ERROR creating thread.");
            }
        }
        item++;
    }

    for (int i = 0; i < NUM_OF_THREADS; i++) {
		pthread_join(tid[i], NULL);			// Wait for all threads to finish
        //printf("i: %d\n", i);
	}

    for (int i = 0; i < NUM_OF_THREADS; i++) {
        printf("valid :(%d, %d)", worker_validation[i], i);
        printf("%p", &tid[i]);
		if (worker_validation[i] == 0) {
            printf("\n");
			return 0;
		}
	}
    printf("\n");

    free(tid);
    free(parameters);
    free(worker_validation);

    return 1;
    // replace this comment with your code
}