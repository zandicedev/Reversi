#define _GNU_SOURCE
#include "player.h"

#include <stdio.h>
#include <stdlib.h>

#include <ctype.h>
#include <limits.h>
#include <string.h>
#include <strings.h>
#include <time.h>
#include <unistd.h>

#include "board.h"

#define DEFAULT_FILENAME "board.txt"
#define CORNER_SCORE 10
#define STABLE_SCORE 5

static time_t begin_time;

static void
prng_init (unsigned int seed)
{
  static bool seed_initialized = false;
  if (!seed_initialized)
    {
      srand (seed);
      seed_initialized = true;
    }
}

move_t
random_player (board_t *board)
{
  if (!board)
    return (move_t){ .row = board_size (board), .column = board_size (board) };

  move_t move;
  size_t count = 0;
  count = board_count_player_moves (board);
  if (!count)
    return (move_t){ .row = board_size (board), .column = board_size (board) };

  prng_init ((time (NULL) * getpid ()) % INT_MAX);
  size_t random = (rand () % count) + 1;
  for (size_t i = 0; i < random; i++)
    {
      move = board_next_move (board);
    }

  return move;
}

static void
remove_spaces (char *move)
{
  char *valid_character = move;
  char *ptr = move;
  while (*ptr != '\0')
    {
      if (!isspace ((unsigned char)*ptr))
        {
          *valid_character = *ptr;
          valid_character++;
        }
      ptr++;
    }
  *valid_character = '\0';
}

static void
save_game (board_t *board)
{
  if (!board)
    return;
  char *filename = NULL;
  size_t len = 0;

  printf ("Give a filename to save the game (default: '%s'): ",
          DEFAULT_FILENAME);
  if (getline (&filename, &len, stdin) == -1)
    {
      fprintf (stderr, "Error reading filename.\n");
      free (filename);
      return;
    }

  if (filename[0] == '\n')
    strcpy (filename, DEFAULT_FILENAME);

  remove_spaces (filename);
  if (strlen (filename) == 0)
    {
      printf ("Missing filename, using default: '%s'.\n", DEFAULT_FILENAME);
      strcpy (filename, DEFAULT_FILENAME);
    }

  FILE *fd = fopen (filename, "w");
  if (!fd)
    {
      fprintf (stderr, "Error: Could not open file '%s' for writing.\n",
               filename);
      free (filename);
      return;
    }

  if (fprintf (fd, "%c\n", board_player (board)) <= 0)
    {
      fprintf (stderr, "Error: Failed to write to file '%s'.\n", filename);
      fclose (fd);
      free (filename);
      return;
    }

  size_t size = board_size (board);
  disc_t disc;
  for (size_t i = 0; i < size; i++)
    {
      for (size_t j = 0; j < size; j++)
        {
          disc = board_get (board, i, j);
          if (fprintf (fd, "%c ", disc) <= 0)
            {
              fprintf (stderr, "Error: Failed to write to file '%s'.\n",
                       filename);
              fclose (fd);
              free (filename);
              return;
            }
        }
      fprintf (fd, "\n");
    }

  free (filename);
  fclose (fd);
}

move_t
human_player (board_t *board)
{
  if (!board)
    return (move_t){ .row = board_size (board), .column = board_size (board) };

  if (board_count_player_moves (board) == 1)
    return board_next_move (board);

  move_t move = { .row = board_size (board), .column = board_size (board) };
  size_t size = board_size (board);
  char *player_move = NULL;
  size_t len = 0;
  size_t row = 0;
  bool exit = false;
  board_print (board, stdout);
  printf ("\n");
  while (true)
    {
      if (!exit)
        printf ("Give your move (e.g. 'A5' or 'a5'), 'q' or 'Q' to quit: ");

      if (getline (&player_move, &len, stdin) == -1)
        {
          free (player_move);
          return move;
        }

      remove_spaces (player_move);
      if (toupper (player_move[0]) == 'Q')
        {
          printf ("Quitting, do you want to save this game (y/N)? ");
          exit = true;
          continue;
        }

      if (exit)
        {
          if (toupper (player_move[0]) == 'Y')
            save_game (board);
          board_set_player (board, EMPTY_DISC);
          free (player_move);
          return move;
        }
      if (toupper (player_move[0]) == 'H')
        {
          move_t move = minimax_ab_player (board);
          board_print_help (board, move);
          continue;
        }

      if (toupper (player_move[0]) == 'U')
        {
          board_undo (board);
          board_print (board, stdout);
          continue;
        }

      if (toupper (player_move[0]) == 'R')
        {
          board_redo (board);
          board_print (board, stdout);
          continue;
        }

      if (strlen (player_move) < 2 || strlen (player_move) > 3
          || !isalpha (player_move[0]))
        {
          printf ("Wrong input, try again!\n\n");
          continue;
        }

      if (toupper (player_move[0]) < 'A'
          || toupper (player_move[0]) >= 'A' + (int)size)
        {
          printf ("Column out of bounds. Try again.\n");
          continue;
        }

      row = 0;
      for (size_t i = 1; i < strlen (player_move); ++i)
        {
          if (!isdigit (player_move[i]))
            {
              printf ("Invalid row input. Try again.\n");
              row = 0;
              break;
            }

          row = row * 10 + (player_move[i] - '0');
        }

      if (row < 1 || row > size)
        {
          printf ("Row out of bounds. Try again.\n");
          continue;
        }

      move.column = toupper (player_move[0]) - 'A';
      move.row = row - 1;
      if (board_is_move_valid (board, move))
        {
          printf ("\nPlaying %c%zu.\n\n", toupper (player_move[0]), row);
          free (player_move);
          return move;
        }

      printf ("This move is invalid. Try again.\n");
    }

  free (player_move);
  return move;
}
static int
stable_score (board_t *board, disc_t player, disc_t opponent, disc_t disc,
              size_t i, size_t j)
{
  int score = 0;
  /* Add score if player disc in the corner and adjacent corner */
  if (board_get (board, i, j) == player)
    {
      if (disc == player)
        score = STABLE_SCORE;
      else if (disc == opponent)
        score = -STABLE_SCORE;
    }
  else
    {
      /* subtract score if opponent disc in the corner */
      if (disc == player)
        score = -STABLE_SCORE;

      else if (disc == opponent)
        score = STABLE_SCORE;
    }
  return score;
}

static int
board_weight_score (board_t *board, disc_t player)
{
  int weight = 0;
  disc_t opponent = (player == BLACK_DISC) ? WHITE_DISC : BLACK_DISC;
  size_t size = board_size (board);
  size_t center = size / 2;
  int distance;
  for (size_t i = 0; i < size; i++)
    {
      for (size_t j = 0; j < size; j++)
        {
          disc_t disc = board_get (board, i, j);

          // Add score for corner positions
          if ((i == 0 && j == 0) || (i == 0 && j == size - 1)
              || (i == size - 1 && j == 0) || (i == size - 1 && j == size - 1))
            {
              if (disc == player)
                weight += CORNER_SCORE;
              else if (disc == opponent)
                weight -= CORNER_SCORE;
              continue;
            }
          // Add or subtract score for positions adjacent corner
          else if ((i == 1 && j == 0) || (i == 0 && j == 1)
                   || (i == 1 && j == 1))
            {
              weight += stable_score (board, player, opponent, disc, 0, 0);
              continue;
            }
          else if ((i == 1 && j == size - 1) || (i == 0 && j == size - 2)
                   || (i == 1 && j == size - 2))
            {
              weight
                  += stable_score (board, player, opponent, disc, 0, size - 1);
              continue;
            }
          else if ((i == size - 2 && j == 0) || (i == size - 2 && j == 1)
                   || (i == size - 1 && j == 1))
            {
              weight
                  += stable_score (board, player, opponent, disc, size - 1, 0);
              continue;
            }
          else if ((i == size - 2 && j == size - 1)
                   || (i == size - 1 && j == size - 2)
                   || (i == size - 2 && j == size - 2))
            {
              weight += stable_score (board, player, opponent, disc, size - 1,
                                      size - 1);
              continue;
            }
          // Add score for edge positions
          else if (i == 0 || i == size - 1 || j == 0 || j == size - 1)
            {
              if (disc == player)
                weight += STABLE_SCORE;
              else if (disc == opponent)
                weight -= STABLE_SCORE;
            }
          // more distance of center more score
          int rows = abs ((int)i - (int)center);
          int columns = abs ((int)j - (int)center);
          distance = size - (rows + columns);
          if (disc == player)
            weight += distance * 2;
          else if (disc == opponent)
            weight -= distance * 2;
        }
    }
  return weight;
}

static int
reduce_possible_moves (board_t *board, disc_t player)
{
  disc_t opponent = (player == BLACK_DISC) ? WHITE_DISC : BLACK_DISC;
  int player_move_number = board_count_player_moves (board);
  board_set_player (board, opponent);
  int opponent_move_number = board_count_player_moves (board);
  board_set_player (board, player);
  int score = 5 * (player_move_number - opponent_move_number);
  return score;
}

static int
score_heuristic (board_t *board, disc_t player)
{
  if (!board || player == EMPTY_DISC)
    return 0;

  disc_t current_player = board_player (board);
  score_t score = board_score (board);
  int final_score = (current_player == BLACK_DISC)
                        ? (int)score.black - score.white
                        : (int)score.white - score.black;
  final_score += board_weight_score (board, player);
  final_score += reduce_possible_moves (board, player);
  return final_score;
}

static int
minimax_search (board_t *board, int depth, bool maximizing_player)
{

  if (depth == 0 || !board_count_player_moves (board)
      || difftime (time (NULL), begin_time) >= timeout)
    return score_heuristic (board, board_player (board));

  int best_score = maximizing_player ? INT_MIN : INT_MAX;
  size_t count = board_count_player_moves (board);
  int current_score;
  board_t *next_board;
  disc_t player = board_player (board);
  bool next_maximizing_player;

  for (size_t i = 0; i < count; i++)
    {
      next_board = board_copy (board);
      if (!next_board)
        return 0;

      move_t move = board_next_move (board);
      board_play (next_board, move);
      next_maximizing_player = (player == board_player (next_board));
      current_score
          = minimax_search (next_board, depth - 1, next_maximizing_player);

      if (maximizing_player)
        best_score = (current_score > best_score) ? current_score : best_score;
      else
        best_score = (current_score < best_score) ? current_score : best_score;

      board_free (next_board);
      if (difftime (time (NULL), begin_time) >= timeout)
        return best_score;
    }
  return best_score;
}

move_t
minimax_player (board_t *board)
{
  if (!board || !board_count_player_moves (board))
    return (move_t){ .row = board_size (board), .column = board_size (board) };

  begin_time = time (NULL);
  move_t move;
  int score;
  move_t best_move;
  int best_score = INT_MIN;
  int depth = 5;
  if (timeout == 1 || timeout == 2)
    depth = 3;
  if (timeout == 3)
    depth = 4;
  board_t *test_board;
  disc_t player = board_player (board);
  size_t count = board_count_player_moves (board);
  bool maximizing_player;
  if (count == 1)
    return board_next_move (board);

  for (size_t i = 0; i < count; i++)
    {
      move = board_next_move (board);
      test_board = board_copy (board);
      if (!test_board)
        return (move_t){ .row = board_size (board),
                         .column = board_size (board) };
      board_play (test_board, move);
      maximizing_player = (player == board_player (test_board));
      score = minimax_search (test_board, depth, maximizing_player);
      if (score > best_score)
        {
          best_score = score;
          best_move = move;
        }

      board_free (test_board);
      if (difftime (time (NULL), begin_time) >= timeout)
        return best_move;
    }
  return best_move;
}

static int
minimax_ab_search (board_t *board, int depth, bool maximizing_player,
                   int alpha, int beta)
{
  if (depth == 0 || !board_count_player_moves (board)
      || (difftime (time (NULL), begin_time) >= timeout))
    return score_heuristic (board, board_player (board));

  int best_score = maximizing_player ? INT_MIN : INT_MAX;
  size_t count = board_count_player_moves (board);
  disc_t player = board_player (board);
  int current_score;
  board_t *next_board;
  bool next_maximizing_player;
  for (size_t i = 0; i < count; i++)
    {
      next_board = board_copy (board);
      if (!next_board)
        return 0;

      move_t move = board_next_move (board);
      if (!board_play (next_board, move))
        return 0;

      next_maximizing_player = (player == board_player (next_board));
      current_score = minimax_ab_search (next_board, depth - 1,
                                         next_maximizing_player, alpha, beta);
      if (maximizing_player)
        {
          best_score
              = (current_score > best_score) ? current_score : best_score;
          alpha = (alpha > best_score) ? alpha : best_score;
        }
      else
        {
          best_score
              = (current_score < best_score) ? current_score : best_score;
          beta = (beta < best_score) ? beta : best_score;
        }

      board_free (next_board);
      if (beta <= alpha)
        return best_score;

      if (difftime (time (NULL), begin_time) >= timeout)
        return best_score;
    }
  
  return best_score;
}

move_t
minimax_ab_player (board_t *board)
{
  if (!board || !board_count_player_moves (board))
    return (move_t){ .row = board_size (board), .column = board_size (board) };

  begin_time = time (NULL);
  move_t move;
  int score = 0;
  move_t best_move;
  int best_score = INT_MIN;
  int depth = 5;
  if (timeout == 1 || timeout == 2)
    depth = 3;
  if (timeout == 3)
    depth = 4;
  board_t *board_test;
  disc_t player = board_player (board);
  bool maximizing_player;
  size_t count = board_count_player_moves (board);
  if (count == 1)
    return board_next_move (board);

  for (size_t i = 0; i < count; i++)
    {
      move = board_next_move (board);
      board_test = board_copy (board);
      if (!board_test)
        return (move_t){ .row = board_size (board),
                         .column = board_size (board) };
      board_play (board_test, move);
      maximizing_player = (player == board_player (board_test));
      score = minimax_ab_search (board_test, depth, maximizing_player, INT_MIN,
                                 INT_MAX);
      if (score > best_score)
        {
          best_score = score;
          best_move = move;
        }

      board_free (board_test);
      if (difftime (time (NULL), begin_time) >= timeout)
        return best_move;
    }
  return best_move;
}
