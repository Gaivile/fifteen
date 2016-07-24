/**
 * fifteen.c
 *
 * Computer Science 50
 * Problem Set 3
 *
 * Implements Game of Fifteen (generalized to d x d).
 *
 * Usage: fifteen d
 *
 * whereby the board's dimensions are to be d x d,
 * where d must be in [DIM_MIN,DIM_MAX]
 *
 * Note that usleep is obsolete, but it offers more granularity than
 * sleep and is simpler to use than nanosleep; `man usleep` for more.
 */
 
#define _XOPEN_SOURCE 500

#include <cs50.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// constants
#define DIM_MIN 3
#define DIM_MAX 9

// board
int board[DIM_MAX][DIM_MAX];

// dimensions
int d;

// variables to keep track of moves for move() function; "x" is to indicate the first move only
int x = 0;
int empty_row;
int empty_col;

// prototypes
void clear(void);
void greet(void);
void init(void);
void draw(void);
bool move(int tile);
bool won(void);

int main(int argc, string argv[])
{
    // ensure proper usage
    if (argc != 2)
    {
        printf("Usage: fifteen d\n");
        return 1;
    }

    // ensure valid dimensions
    d = atoi(argv[1]);
    if (d < DIM_MIN || d > DIM_MAX)
    {
        printf("Board must be between %i x %i and %i x %i, inclusive.\n",
            DIM_MIN, DIM_MIN, DIM_MAX, DIM_MAX);
        return 2;
    }

    // open log
    FILE* file = fopen("log.txt", "w");
    if (file == NULL)
    {
        return 3;
    }

    // greet user with instructions
    greet();

    // initialize the board
    init();

    // accept moves until game is won
    while (true)
    {
        // clear the screen
        clear();

        // draw the current state of the board
        draw();

        // log the current state of the board (for testing)
        for (int i = 0; i < d; i++)
        {
            for (int j = 0; j < d; j++)
            {
                fprintf(file, "%i", board[i][j]);
                if (j < d - 1)
                {
                    fprintf(file, "|");
                }
            }
            fprintf(file, "\n");
        }
        fflush(file);

        // check for win
        if (won())
        {
            printf("ftw!\n");
            break;
        }

        // prompt for move
        printf("Tile to move: ");
        int tile = GetInt();
        
        // quit if user inputs 0 (for testing)
        if (tile == 0)
        {
            break;
        }

        // log move (for testing)
        fprintf(file, "%i\n", tile);
        fflush(file);

        // move if possible, else report illegality
        if (!move(tile))
        {
            printf("\nIllegal move.\n");
            usleep(500000);
        }

        // sleep thread for animation's sake
        usleep(500000);
    }
    
    // close log
    fclose(file);

    // success
    return 0;
}

/**
 * Clears screen using ANSI escape sequences.
 */
void clear(void)
{
    printf("\033[2J");
    printf("\033[%d;%dH", 0, 0);
}

/**
 * Greets player.
 */
void greet(void)
{
    clear();
    printf("WELCOME TO GAME OF FIFTEEN\n");
    usleep(2000000);
}

/**
 * Initializes the game's board with tiles numbered 1 through d*d - 1
 * (i.e., fills 2D array with values but does not actually print them).  
 */
void init(void)
{
    // variable for the first integer of a 2D array, to be decremented inside loops
    int f = (d * d) - 1;
   
    // loop for rows
    for (int i = 0; i < d; i++)
    {
        // loop for columns
        for (int j = 0; j < d; j++)
        {
            // populate the array and decrement f
            board[i][j] = f;
            f--;
            
            // if/else-if statement to swap 2 and 1 if number of tiles is even
            if ((board[i][j] == 2) && ((d * d) % 2) == 0)
            {
                board[i][j] = 1;
            }
            else if ((board[i][j] == 1) && ((d * d) % 2) == 0)
            {
                board[i][j] = 2;
            }
        }
    }
}

/**
 * Prints the board in its current state.
 */
void draw(void)
{
    // loop for rows
    for(int i = 0; i < d; i++)
    {
        // loop for columns
        for(int j = 0; j < d; j++)
        {
            // check for 0 (empty tile) and print a char instead of an int
            if (board[i][j] == 0)
            {
                printf("%2c ", '_');
            }
            // print everything else
            else
            {
                printf("%2d ", board[i][j]);
            }
        }
        printf("\n");
    }
}

/**
 * If tile borders empty space, moves tile and returns true, else
 * returns false. 
 */
bool move(int tile)
{
    // declaration of variables for tile's indexes of 2D array
    int row_idx = 0;
    int col_idx = 0;
    
    // loops to check if tile exists on the board
    for(int i = 0; i < d; i++)
    {
        for (int j = 0; j < d; j++)
        {
            // get indexes of a tile
            if(board[i][j] == tile)
            {
                row_idx = i;
                col_idx = j;
                break;
            }
        }
    }
    // first move only
    if(x == 0)
    {
         empty_row = d - 1;
         empty_col = d - 1;
    }
    
    // check for row and make a move
    if ((((row_idx + 1) == empty_row) || ((row_idx -1) == empty_row)) && (col_idx == empty_col))
    {
       board[empty_row][empty_col] = tile;
       empty_row = row_idx;
       empty_col = col_idx;
       board[row_idx][col_idx] = 0;
       x++;
       return true;
    }
    
    //check for column and make a move
    else if ((((col_idx + 1) == empty_col) || ((col_idx -1) == empty_col)) && (row_idx == empty_row))
    {
       board[empty_row][empty_col] = tile;
       empty_row = row_idx;
       empty_col = col_idx;
       board[row_idx][col_idx] = 0;
       x++;
       return true;
    }
    
    // if given value is invalid
    return false;
}

/**
 * Returns true if game is won (i.e., board is in winning configuration), 
 * else false.
 */
bool won(void)
{
    // first value
    int n = 1;
    
    // loops to check each value
    for(int i = 0; i < d; i++)
    {
        for(int j = 0; j < d; j++)
        {
            // check for last empty tile (0)
            if((i == (d - 1)) && (j == (d - 1)))
            {
                n = 0;
            }
            
            // if anything is out of order - return false
            if(n != board[i][j])
            {
                return false;
            }
            // next value
            n++;
        }
    }
    // if everything is in places, return true 
    return true;
}
