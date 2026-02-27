#ifndef PLAYER_H
#define PLAYER_H

#include <time.h>

#include "board.h"

extern time_t timeout;
/* A player function 'move_t (*player_func) (board_t *)' returns a
 * chosen move depending on the given board. */

/**
 * @brief Interact with the user in order to ask him a move through stdin
 * @param board A pointer representing the game board.
 * @return the move if is a valid
 **/
move_t human_player (board_t *board);

/**
 * @brief Returns a random move among the possible ones
 * @param board A pointer representing the game board.
 * @return a random move.
 **/
move_t random_player (board_t *board);

/**
 * @brief Implements a minimax tree search algorithm to find the best possible
 *move.
 * @param board A pointer representing the game board.
 * @return The best move found by the minimax algorithm for the current player.
 **/
move_t minimax_player (board_t *board);

/**
 * @brief Implements a minimax tree search algorithm with alpha-beta pruning
 *        to find the best possible move.
 * @param board A pointer representing the game board.
 * @return The best move found by the minimax algorithm with alpha-beta pruning
 *for the current player.
 **/
move_t minimax_ab_player (board_t *board);

#endif /* PLAYER_H */