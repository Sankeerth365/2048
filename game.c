#include "game.h"

//Partner: anara32
//Intro Paragraph: This implementation of 2047 creates a variable-sized grid where players can slide and merge tiles
//using W,A,S,D keys. Th core functionality includes creating and managing teh game board, handling tile movements, merging identical tiles,
//updating the score when merges occur, and checking for valid moves. The program uses a row-major layout to store teh game board as a 1D array 
//of cells, with efficieny memory management through proper allocation and deallocation. A key feature is the preventation of 
//double merges in a single move using a tracking array. The implmentation supports game reset with new dimensions via the 'n' key and provides a proper
//game over detection when no legal moves remain. 
game * make_game(int rows, int cols) {
    game * mygame = malloc(sizeof(game));
    mygame->cells = malloc(rows*cols*sizeof(cell));

    // Initialize game variables
    mygame->rows = rows;
    mygame->cols = cols;
    mygame->score = 0;

    // Initialize all cells to -1
    for (int i = 0; i < rows * cols; i++) {
        mygame->cells[i] = -1;
    }

    return mygame;
}

void remake_game(game ** _cur_game_ptr, int new_rows, int new_cols) {
    free((*_cur_game_ptr)->cells);
    (*_cur_game_ptr)->cells = malloc(new_rows*new_cols*sizeof(cell));

    // Re-initialize game variables
    (*_cur_game_ptr)->rows = new_rows;
    (*_cur_game_ptr)->cols = new_cols;
    (*_cur_game_ptr)->score = 0;

    // Initialize all cells to -1
    for (int i = 0; i < new_rows * new_cols; i++) {
        (*_cur_game_ptr)->cells[i] = -1;
    }
}

void destroy_game(game * cur_game) {
    free(cur_game->cells);
    free(cur_game);
    cur_game = NULL;
    return;
}

cell * get_cell(game * cur_game, int row, int col) {
    // Check if row and col are within bounds
    if (row < 0 || row >= cur_game->rows || col < 0 || col >= cur_game->cols) {
        return NULL;
    }
    
    // Return pointer to cell using row-major layout
    return &cur_game->cells[row * cur_game->cols + col];
}

int move_w(game * cur_game) {
    int moved = 0;
    int rows = cur_game->rows;
    int cols = cur_game->cols;
    int *merged = calloc(rows * cols, sizeof(int)); // Track merged cells

    // Process each column
    for (int col = 0; col < cols; col++) {
        // Process each row from top to bottom
        for (int row = 1; row < rows; row++) {
            if (*get_cell(cur_game, row, col) == -1) continue;

            int curr_row = row;
            while (curr_row > 0) {
                cell *current = get_cell(cur_game, curr_row, col);
                cell *above = get_cell(cur_game, curr_row - 1, col);
                
                if (*above == -1) {
                    // Move up to empty cell
                    *above = *current;
                    *current = -1;
                    moved = 1;
                    curr_row--;
                } else if (*above == *current && !merged[(curr_row-1)*cols + col]) {
                    // Merge with cell above if values match and not already merged
                    *above *= 2;
                    *current = -1;
                    cur_game->score += *above;
                    merged[(curr_row-1)*cols + col] = 1;
                    moved = 1;
                    break;
                } else {
                    break;
                }
            }
        }
    }

    free(merged);
    return moved;
}

int move_s(game * cur_game) {
    int moved = 0;
    int rows = cur_game->rows;
    int cols = cur_game->cols;
    int *merged = calloc(rows * cols, sizeof(int));

    // Process each column
    for (int col = 0; col < cols; col++) {
        // Process each row from bottom to top
        for (int row = rows - 2; row >= 0; row--) {
            if (*get_cell(cur_game, row, col) == -1) continue;

            int curr_row = row;
            while (curr_row < rows - 1) {
                cell *current = get_cell(cur_game, curr_row, col);
                cell *below = get_cell(cur_game, curr_row + 1, col);
                
                if (*below == -1) {
                    *below = *current;
                    *current = -1;
                    moved = 1;
                    curr_row++;
                } else if (*below == *current && !merged[(curr_row+1)*cols + col]) {
                    *below *= 2;
                    *current = -1;
                    cur_game->score += *below;
                    merged[(curr_row+1)*cols + col] = 1;
                    moved = 1;
                    break;
                } else {
                    break;
                }
            }
        }
    }

    free(merged);
    return moved;
}

int move_a(game * cur_game) {
    int moved = 0;
    int rows = cur_game->rows;
    int cols = cur_game->cols;
    int *merged = calloc(rows * cols, sizeof(int));

    // Process each row
    for (int row = 0; row < rows; row++) {
        // Process each column from left to right
        for (int col = 1; col < cols; col++) {
            if (*get_cell(cur_game, row, col) == -1) continue;

            int curr_col = col;
            while (curr_col > 0) {
                cell *current = get_cell(cur_game, row, curr_col);
                cell *left = get_cell(cur_game, row, curr_col - 1);
                
                if (*left == -1) {
                    *left = *current;
                    *current = -1;
                    moved = 1;
                    curr_col--;
                } else if (*left == *current && !merged[row*cols + (curr_col-1)]) {
                    *left *= 2;
                    *current = -1;
                    cur_game->score += *left;
                    merged[row*cols + (curr_col-1)] = 1;
                    moved = 1;
                    break;
                } else {
                    break;
                }
            }
        }
    }

    free(merged);
    return moved;
}

int move_d(game * cur_game) {
    int moved = 0;
    int rows = cur_game->rows;
    int cols = cur_game->cols;
    int *merged = calloc(rows * cols, sizeof(int));

    // Process each row
    for (int row = 0; row < rows; row++) {
        // Process each column from right to left
        for (int col = cols - 2; col >= 0; col--) {
            if (*get_cell(cur_game, row, col) == -1) continue;

            int curr_col = col;
            while (curr_col < cols - 1) {
                cell *current = get_cell(cur_game, row, curr_col);
                cell *right = get_cell(cur_game, row, curr_col + 1);
                
                if (*right == -1) {
                    *right = *current;
                    *current = -1;
                    moved = 1;
                    curr_col++;
                } else if (*right == *current && !merged[row*cols + (curr_col+1)]) {
                    *right *= 2;
                    *current = -1;
                    cur_game->score += *right;
                    merged[row*cols + (curr_col+1)] = 1;
                    moved = 1;
                    break;
                } else {
                    break;
                }
            }
        }
    }

    free(merged);
    return moved;
}

int legal_move_check(game * cur_game) {
    int rows = cur_game->rows;
    int cols = cur_game->cols;

    // Check for empty cells
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            if (*get_cell(cur_game, row, col) == -1) {
                return 1;
            }
        }
    }

    // Check for adjacent equal values
    for (int row = 0; row < rows; row++) {
        for (int col = 0; col < cols; col++) {
            cell current = *get_cell(cur_game, row, col);
            
            // Check right
            if (col < cols - 1 && current == *get_cell(cur_game, row, col + 1)) {
                return 1;
            }
            
            // Check down
            if (row < rows - 1 && current == *get_cell(cur_game, row + 1, col)) {
                return 1;
            }
        }
    }

    return 0;
}

/*! code below is provided and should not be changed */

void rand_new_tile(game * cur_game)
/*! insert a new tile into a random empty cell. First call rand()%(rows*cols) to get a random value between 0 and (rows*cols)-1.
*/
{
    cell * cell_ptr;
    cell_ptr = cur_game->cells;
    
    if (cell_ptr == NULL){ 	
        printf("Bad Cell Pointer.\n");
        exit(0);
    }
    
    //check for an empty cell
    int emptycheck = 0;
    int i;
    
    for(i = 0; i < ((cur_game->rows)*(cur_game->cols)); i++){
        if ((*cell_ptr) == -1){
                emptycheck = 1;
                break;
        }		
        cell_ptr += 1;
    }
    if (emptycheck == 0){
        printf("Error: Trying to insert into no a board with no empty cell. The function rand_new_tile() should only be called after tiles have succesfully moved, meaning there should be at least 1 open spot.\n");
        exit(0);
    }
    
    int ind,row,col;
    int num;
    do{
        ind = rand()%((cur_game->rows)*(cur_game->cols));
        col = ind%(cur_game->cols);
        row = ind/cur_game->cols;
    } while ( *get_cell(cur_game, row, col) != -1);
        //*get_cell(cur_game, row, col) = 2;
    num = rand()%20;
    if(num <= 1){
        *get_cell(cur_game, row, col) = 4; // 1/10th chance
    }
    else{
        *get_cell(cur_game, row, col) = 2;// 9/10th chance
    }
}

int print_game(game * cur_game) 
{
    cell * cell_ptr;
    cell_ptr = 	cur_game->cells;

    int rows = cur_game->rows;
    int cols = cur_game->cols;
    int i,j;
    
    printf("\n\n\nscore:%d\n",cur_game->score); 
    
    
    printf("\u2554"); // topleft box char
    for(i = 0; i < cols*5;i++)
        printf("\u2550"); // top box char
    printf("\u2557\n"); //top right char 
    
    
    for(i = 0; i < rows; i++){
        printf("\u2551"); // side box char
        for(j = 0; j < cols; j++){
            if ((*cell_ptr) == -1 ) { //print asterisks
                printf(" **  "); 
            }
            else {
                switch( *cell_ptr ){ //print colored text
                    case 2:
                        printf("\x1b[1;31m%04d\x1b[0m ",(*cell_ptr));
                        break;
                    case 4:
                        printf("\x1b[1;32m%04d\x1b[0m ",(*cell_ptr));
                        break;
                    case 8:
                        printf("\x1b[1;33m%04d\x1b[0m ",(*cell_ptr));
                        break;
                    case 16:
                        printf("\x1b[1;34m%04d\x1b[0m ",(*cell_ptr));
                        break;
                    case 32:
                        printf("\x1b[1;35m%04d\x1b[0m ",(*cell_ptr));
                        break;
                    case 64:
                        printf("\x1b[1;36m%04d\x1b[0m ",(*cell_ptr));
                        break;
                    case 128:
                        printf("\x1b[31m%04d\x1b[0m ",(*cell_ptr));
                        break;
                    case 256:
                        printf("\x1b[32m%04d\x1b[0m ",(*cell_ptr));
                        break;
                    case 512:
                        printf("\x1b[33m%04d\x1b[0m ",(*cell_ptr));
                        break;
                    case 1024:
                        printf("\x1b[34m%04d\x1b[0m ",(*cell_ptr));
                        break;
                    case 2048:
                        printf("\x1b[35m%04d\x1b[0m ",(*cell_ptr));
                        break;
                    case 4096:
                        printf("\x1b[36m%04d\x1b[0m ",(*cell_ptr));
                        break;
                    case 8192:
                        printf("\x1b[31m%04d\x1b[0m ",(*cell_ptr));
                        break;
                    default:
                        printf("  X  ");
                }
            }
            cell_ptr++;
        }
        printf("\u2551\n"); //print right wall and newline
    }
    
    printf("\u255A"); // print bottom left char
    for(i = 0; i < cols*5;i++)
        printf("\u2550"); // bottom char
    printf("\u255D\n"); //bottom right char
    
    return 0;
}

int process_turn(const char input_char, game* cur_game) //returns 1 if legal move is possible after input is processed
{ 
    int rows,cols;
    char buf[200];
    char garbage[2];
    int move_success = 0;
    
    switch ( input_char ) {
    case 'w':
        move_success = move_w(cur_game);
        break;
    case 'a':
        move_success = move_a(cur_game);
        break;
    case 's':
        move_success = move_s(cur_game);
        break;
    case 'd':
        move_success = move_d(cur_game);
        break;
    case 'q':
        destroy_game(cur_game);
        printf("\nQuitting..\n");
        return 0;
        break;
    case 'n':
        //get row and col input for new game
        dim_prompt: printf("NEW GAME: Enter dimensions (rows columns):");
        while (NULL == fgets(buf,200,stdin)) {
            printf("\nProgram Terminated.\n");
            return 0;
        }
        
        if (2 != sscanf(buf,"%d%d%1s",&rows,&cols,garbage) ||
        rows < 0 || cols < 0){
            printf("Invalid dimensions.\n");
            goto dim_prompt;
        } 
        
        remake_game(&cur_game,rows,cols);
        
        move_success = 1;
        
    default: //any other input
        printf("Invalid Input. Valid inputs are: w, a, s, d, q, n.\n");
    }

    if(move_success == 1){ //if movement happened, insert new tile and print the game.
         rand_new_tile(cur_game); 
         print_game(cur_game);
    } 

    if( legal_move_check(cur_game) == 0){  //check if the newly spawned tile results in game over.
        printf("Game Over!\n");
        return 0;
    }
    return 1;
}

