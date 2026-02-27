#ifndef BOARD_H
#define BOARD_H

#include <stdbool.h>
#include <stdio.h>

/* Min/max with board */
#define MIN_BOARD_SIZE 2
#define MAX_BOARD_SIZE 10

/* Board discs */
typedef enum
{
  BLACK_DISC = 'X',
  WHITE_DISC = 'O',
  EMPTY_DISC = '_',
  HINT_DISC = '*'
} disc_t;

/* A move in the reversi game */
typedef struct
{
  size_t row;
  size_t column;
} move_t;

typedef struct
{
  unsigned short black;
  unsigned short white;
} score_t;

/* Reversi board (forward declaration to hide the implementation) */
typedef struct board_t board_t;
typedef unsigned __int128 bitboard_t;

/**
 * @brief Allocate the memory needed to create a board of size 'size'
 * @param size The size of the board, which can be 2, 4, 6, 8, or 10.
 * @param player current player
 * @return a pointer to the newly created board or NULL
 **/
board_t *board_alloc (const size_t size, const disc_t player);

/**
 * @brief Free the memory allocated to the hold the board
 * @param board A pointer to the initialized board.
 **/
void board_free (board_t *board);

/**
 * @brief Initialize all the squares of the board as a starting game
 *         (with the initial discs on the board)
 * @param size The size of the board, which can be 2, 4, 6, 8, or 10.
 * @return A pointer to the initialized board.
 **/
board_t *board_init (const size_t size);

/**
 * @brief Perform a deep copy of the board structure
 * @param board A pointer representing the game board.
 * @return A pointer to the newly created board that is a deep copy of the
 *original, or NULL if the copy operation fails
 **/
board_t *board_copy (const board_t *board);

/**
 * @brief Gets the size of the game board.
 * @param board A pointer representing the game board.
 * @return the size of the board (number of columns or rows)
 **/
size_t board_size (const board_t *board);

/**
 * @brief Get the current player of the given game
 * @param board A pointer representing the game board.
 * @return the current player
 **/
disc_t board_player (const board_t *board);

/**
 * @brief Set the current player of the given game
 * @param board A pointer representing the game board.
 **/
void board_set_player (board_t *board, disc_t player);

/**
 * @brief Retrieves the content of the square at the given coordinate
 * @param row The row index of the square.
 * @param column The column index of the square.
 * @return the empty square if the given coordinates are outside of the current
 *board
 **/
disc_t board_get (const board_t *board, const size_t row, const size_t column);

/**
 * @brief set the given disc at the given position
 * @param board pointer representing the game board.
 * @param disc The disc to be placed
 * @param row The row index of the square.
 * @param column The column index of the square.
 **/
void board_set (board_t *board, const disc_t disc, size_t row,
                const size_t column);

/**
 * @brief Calculates and returns the score of the given game board.
 *
 * @details This function computes the current score based on the state of the
 *  game board. It returns a score_t structure, which typically contains
 *  information such as the number of discs for each player (black and white)
 *
 * @param board A pointer representing the game board.
 * @return the score of the given board in a score_t struct
 **/
score_t board_score (const board_t *board);

/**
 * @brief Writes the content of the given game board to the specified file
 *descriptor.
 *  @details This function prints the current state of the game board to the
 *  provided file descriptor (a file or standard output).
 *  It returns the number of characters successfully written, or -1 in case
 *  of an error (such as an invalid file descriptor).
 *
 * @param  board A pointer representing the game board.
 * @param  fd  The file descriptor where the board content will be written.
 * @return The number of characters printed, or -1 if an error occurs.
 **/
int board_print (const board_t *board, FILE *fd);

/**
 * @brief Checks if a move is valid on the given game board.
 * @param board A pointer representing the game board.
 * @param move The move to check (row and column).
 * @return True if the move is valid, false otherwise.
 **/
bool board_is_move_valid (const board_t *board, const move_t move);

/**
 * @brief Count the number of possible moves
 * @param board A pointer representing the game board.
 * @return the number of possible moves
 **/
size_t board_count_player_moves (board_t *board);

/**
 * @brief Applies a move on the game board and updates the state for the next
 * turn.
 *
 * @details This function applies the move according to the game rules, flips
 *        the opponent's discs as necessary, and updates the board state. It
 *        sets the next player or ends the game if no moves are possible.
 * @param board A pointer representing the game board.
 * @param move The move to apply (row and column).
 * @return True if the move is successfully applied, false if the move is
 * invalid.
 **/
bool board_play (board_t *board, const move_t move);

/**
 * @brief Retrieves the next possible move from the list of valid moves.
 * @details This function iterates through the bitboard of possible moves.
 *          After each call, it clears the last bit corresponding to the most
 * recent move and prepares for the next call. When all possible moves are
 * exhausted, it resets the bitboard.
 * @param board A pointer representing the game board.
 * @return A move (row and column) representing the next valid move.
 **/
move_t board_next_move (board_t *board);

/**
 * @brief Reverts the last move made by the current player.
 * @param board A pointer to the game board structure.
 */
void board_undo (board_t *board);

/**
 * @brief Redoes the last undone move.
 * @param board A pointer to the game board structure.
 */
void board_redo (board_t *board);

/**
 * @brief Initializes the undo and redo stacks in the board_t structure.
 * @param board A pointer to the game board structure.
 */
void undo_redo_init (board_t *board);

/**
 * @brief Writes the content of the current state of the game board to the
 * specified file descriptor, including the recommended best move for the given
 * player.
 * @param board A pointer to the game board structure.
 * @param move The recommended best move for the current player.
 * @return The number of characters written to the file descriptor, or -1 in
 * case of an error.
 */
int board_print_help (const board_t *board, move_t move);

#endif /* BOARD_H */