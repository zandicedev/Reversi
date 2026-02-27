#include "reversi.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include <ctype.h>
#include <err.h>
#include <getopt.h>
#include <string.h>
#include <time.h>

#include "board.h"
#include "player.h"

#define BOARD_DEFAULT_SIZE 4
#define BLACK_WIN 1
#define WHITE_WIN 2
#define BLACK_RESIGNED -1
#define WHITE_RESIGNED -2
#define DRAW_GAME 0
#define ERROR_CODE -3
#define BUFFER_SIZE 2048

#define ERROR(fd, ...)                                                        \
  {                                                                           \
    fclose (fd);                                                              \
    fprintf (stderr, ##__VA_ARGS__);                                          \
    exit (EXIT_FAILURE);                                                      \
  }

#define FIND_CHAR_COLUMN(column)                                              \
  char c = 'A';                                                               \
  for (size_t i = 0; i < column; c++, i++)

static bool verbose = false;
time_t timeout = 5;

static void
usage (void)
{
  printf ("Usage: reversi ([-s SIZE|-b[N]|-w[N]|-t TIME|-v|-V|-h|-c] [FILE])\n"
          "Play a reversi game with human or program players\n"
          "  -s, --size SIZE        board size (min=1, max=5 (default=4))\n"
          "  -b[N], --black-ai[=N]  set tactic of black player (default: 0)\n"
          "  -w[N], --white-ai[=N]  set tactic of white player (default: 0)\n"
          "  -t, --timeout SECONDS  set AI timeout (default:5s)\n"
          "  -c, --contest          enable 'contest' mode\n"
          "  -v, --verbose          verbose output\n"
          "  -V, --version          display version and exit\n"
          "  -h, --help             display this help and exit\n"
          "\n"
          "Tactics list: human (0) random (1), minimax (2)\n");
}

static void
remove_spaces_and_comment (char *file_content)
{
  bool exist_comment = false;
  char *valid_character = file_content;
  char *ptr = file_content;
  while (*ptr != '\0')
    {
      if (*ptr == ' ' || *ptr == '\t')
        {
          ptr++;
          continue;
        }

      if (*ptr == '#')
        {
          exist_comment = true;
          ptr++;
          continue;
        }

      if (exist_comment)
        {
          if (*ptr == '\n')
            exist_comment = false;
          ptr++;
          continue;
        }

      if (*ptr == '\n')
        {
          ptr++;
          continue;
        }

      *valid_character = *ptr;
      valid_character++;
      ptr++;
    }
  *valid_character = '\0';
}

static board_t *
file_parser (const char *filename)
{
  FILE *fd = fopen (filename, "r");
  if (!fd)
    err (EXIT_FAILURE, "error: the file could not be opened correctly!\n");

  char file_content[BUFFER_SIZE];
  while (!feof (fd))
    {
      if (!fgets (file_content, sizeof (file_content), fd))
        ERROR (fd, "reversi: error: file parser\n");

      remove_spaces_and_comment (file_content);
      if (strlen (file_content) >= 1)
        break;
    }

  if (strlen (file_content) < 1)
    ERROR (fd, "reversi: error: the file is empty.\n");

  bool player_first_row = false;
  disc_t player = file_content[0];
  if (player != BLACK_DISC && player != WHITE_DISC)
    ERROR (fd, "reversi: error: player is incorrect or missing.\n");

  /* Check if the player is in the first row of the file */
  if (file_content[1] != '\0')
    {
      player_first_row = true;
      goto player_in_first_row;
    }

  while (!feof (fd))
    {
      if (!fgets (file_content, sizeof (file_content), fd))
        ERROR (fd, "reversi: error: file parser\n");

      // Skip empty lines with only comments
      remove_spaces_and_comment (file_content);
      if (strlen (file_content) > 1)
        break;
    }

  int i = 0;
  disc_t save_discs[MAX_BOARD_SIZE];
player_in_first_row:
  while (file_content[i] != '\0')
    {
      save_discs[i]
          = (player_first_row) ? file_content[i + 1] : file_content[i];
      i++;
    }

  size_t size_of_board = i;
  if (size_of_board > MAX_BOARD_SIZE || size_of_board < MIN_BOARD_SIZE
      || (!player_first_row && size_of_board % 2 != 0))
    ERROR (fd,
           "reversi: error: row 1 is malformed! (wrong number of columns)\n");
  if (player_first_row)
    size_of_board = size_of_board - 1;
  board_t *board = board_alloc (size_of_board, player);
  if (!board)
    ERROR (fd, "reversi: error: board is null.\n");

  board_set_player (board, player);
  for (size_t i = 0; i < size_of_board; i++)
    board_set (board, save_discs[i], 0, i);

  size_t rows = 1;
  size_t columns = 0;
  while (!feof (fd))
    {
      if (!fgets (file_content, sizeof (file_content), fd))
        {
          if (feof (fd))
            break;
          else
            {
              board_free (board);
              ERROR (fd, "reversi: error: file parser\n");
            }
        }

      remove_spaces_and_comment (file_content);
      columns = 0;
      for (int i = 0; file_content[i] != '\0'; i++)
        {
          switch (file_content[i])
            {
            case BLACK_DISC:
              board_set (board, BLACK_DISC, rows, i);
              break;

            case WHITE_DISC:
              board_set (board, WHITE_DISC, rows, i);
              break;

            case EMPTY_DISC:
              board_set (board, EMPTY_DISC, rows, i);
              break;

            default:
              board_free (board);
              ERROR (fd, "reversi: error: wrong character '%c' at row %zu\n",
                     file_content[i], rows + 1)
            }

          if (i > 0)
            columns = i + 1;
        }

      /* empty row, skip this row */
      if (columns == 0)
        continue;
      rows++;
      /* Ensure the number of columns matches the expected size */
      if (columns != size_of_board)
        {
          board_free (board);
          ERROR (fd,
                 "reversi: error: row %zu is malformed! (wrong number of "
                 "columns)\n",
                 rows);
        }
    }

  /* Check if the number of rows matches the board size */
  if (rows > size_of_board)
    {
      board_free (board);
      ERROR (fd, "reversi: error: board has %zu extra row(s).\n",
             (rows - size_of_board));
    }

  if (rows < size_of_board)
    {
      board_free (board);
      ERROR (fd, "reversi: error: board has %zu missing row(s).\n",
             (size_of_board - rows));
    }

  undo_redo_init (board);
  fclose (fd);
  return board;
}

static int
game (move_t (*black) (board_t *), move_t (*white) (board_t *), board_t *board)
{
  if (!board || !black || !white)
    return ERROR_CODE;

  printf ("Welcome to this reversi game!\n"
          "Black player is '%c' and white player is '%c'.\n\n",
          BLACK_DISC, WHITE_DISC);

  int result;
  bool game_is_over = false;
  move_t player_move;
  score_t score;
  size_t game_size = board_size (board);
  disc_t player = board_player (board);
  disc_t opponent = (player == BLACK_DISC) ? WHITE_DISC : BLACK_DISC;
  if (!board_count_player_moves (board))
    {
      board_set_player (board, opponent);
      if (!board_count_player_moves (board))
        {
          board_set_player (board, EMPTY_DISC);
          goto finish_game;
        }
    }

  while (!game_is_over)
    {
      player_move = (player == BLACK_DISC) ? black (board) : white (board);
      if (verbose)
        {
          if (board_count_player_moves (board) == 1)
            {
              FIND_CHAR_COLUMN (player_move.column);
              printf ("\n");
              printf ("Player '%c' has only one choice %c%zu\n\n",
                      board_player (board), c, player_move.row + 1);
              board_print (board, stdout);
              printf ("\n");
            }
          else if (((black == random_player || black == minimax_ab_player)
                    && board_player (board) == BLACK_DISC)
                   || ((white == random_player || white == minimax_ab_player)
                       && board_player (board) == WHITE_DISC))
            {
              printf ("\n");
              board_print (board, stdout);
              printf ("\n");
              FIND_CHAR_COLUMN (player_move.column);
              printf ("Playing %c%zu\n\n", c, player_move.row + 1);
            }
        }

      if ((player_move.row == game_size) && (player_move.column == game_size))
        {
          result = (player == BLACK_DISC) ? BLACK_RESIGNED : WHITE_RESIGNED;
          game_is_over = true;
          continue;
        }

      board_play (board, player_move);
      player = board_player (board);
      if (player == EMPTY_DISC)
        {
        finish_game:
          score = board_score (board);
          game_is_over = true;
          result = (score.black > score.white)   ? BLACK_WIN
                   : (score.white > score.black) ? WHITE_WIN
                                                 : DRAW_GAME;
          continue;
        }
    }

  board_print (board, stdout);
  printf ("\n");
  switch (result)
    {
    case BLACK_WIN:
      printf ("Player '%c' wins the game.\n", BLACK_DISC);
      break;

    case WHITE_WIN:
      printf ("Player '%c' wins the game.\n", WHITE_DISC);
      break;

    case BLACK_RESIGNED:
      printf ("Player '%c' resigned. Player '%c' wins the game.\n", BLACK_DISC,
              WHITE_DISC);
      break;

    case WHITE_RESIGNED:
      printf ("Player '%c' resigned. Player '%c' wins the game.\n", WHITE_DISC,
              BLACK_DISC);
      break;

    default:
      printf ("Draw game, no winner.\n");
      break;
    }

  score = board_score (board);
  unsigned short empty_disc
      = (game_size * game_size) - (score.black + score.white);
  if (result == BLACK_RESIGNED || result == WHITE_WIN)
    printf ("Final score: '%c' = %u, '%c' = %u \n\n", BLACK_DISC, score.black,
            WHITE_DISC, score.white + empty_disc);
  else if (result == WHITE_RESIGNED || result == BLACK_WIN)
    printf ("Final score: '%c' = %u, '%c' = %u \n\n", BLACK_DISC,
            score.black + empty_disc, WHITE_DISC, score.white);
  printf ("Thanks for playing, see you soon!\n");
  return result;
}

static void
error_reversi (char *s)
{
  fprintf (stderr,
           "%s\n"
           "Try `./reversi --help' for more informations.\n",
           s);
  exit (EXIT_FAILURE);
}

int
main (int argc, char *argv[])
{
  int optc;
  int black_tactic = 0;
  board_t *board;
  int white_tactic = 0;
  bool contest_mode = false;
  size_t size = 8;
  move_t (*black_player) (board_t *);
  move_t (*white_player) (board_t *);
  struct option const long_opts[]
      = { { "size", required_argument, NULL, 's' },
          { "timeout", required_argument, NULL, 't' },
          { "black-ai", optional_argument, NULL, 'b' },
          { "white-ai", optional_argument, NULL, 'w' },
          { "contest", no_argument, NULL, 'c' },
          { "verbose", no_argument, NULL, 'v' },
          { "version", no_argument, NULL, 'V' },
          { "help", no_argument, NULL, 'h' },
          { NULL, 0, NULL, 0 } };

  while ((optc = getopt_long (argc, argv, "s:t:b::w::cvVh", long_opts, NULL))
         != -1)
    switch (optc)
      {
      case 's':
        size = atoi (optarg);
        if (size < MIN_PARSER_SIZE || size > MAX_PARSER_SIZE)
          error_reversi ("size is out of range");
        size = size * 2;
        break;

      case 't':
        timeout = atoi (optarg);
        if (timeout < 0)
          error_reversi ("time is out of range");
        break;

      case 'b':
        if (optarg)
          {
            if (!isdigit (optarg[0]))
              error_reversi ("Black tactic is not number");
            black_tactic = atoi (optarg);
          }

        if (black_tactic < HUMAN_TACTIC || black_tactic > MINMAX_TACTIC)
          error_reversi ("Black tactic is out of range");
        break;

      case 'w':
        if (optarg)
          {
            if (!isdigit (optarg[0]))
              error_reversi ("White tactic is not number");
            white_tactic = atoi (optarg);
          }

        if (white_tactic < HUMAN_TACTIC || white_tactic > MINMAX_TACTIC)
          error_reversi ("White tactic is out of range");
        break;

      case 'c':
        contest_mode = true;
        break;

      case 'v':
        verbose = true;
        break;

      case 'V':
        printf ("reversi %d.%d.%d\n", VERSION, SUBVERSION, REVISION);
        printf ("This software allows to play to reversi game.\n");
        return EXIT_SUCCESS;

      case 'h':
        usage ();
        return EXIT_SUCCESS;

      default:
        error_reversi ("Incorrect input");
      }

  black_player = (black_tactic == HUMAN_TACTIC)    ? human_player
                 : (black_tactic == RANDOM_TACTIC) ? random_player
                                                   : minimax_ab_player;

  white_player = (white_tactic == HUMAN_TACTIC)    ? human_player
                 : (white_tactic == RANDOM_TACTIC) ? random_player
                                                   : minimax_ab_player;
  board = (optind < argc) ? file_parser (argv[optind]) : board_init (size);
  if (!board)
    err (EXIT_FAILURE, "the game could not start!\n");

  if (contest_mode)
    {
      move_t move = random_player (board);
      size_t column = move.column;
      size_t row = move.row;
      if (board_size (board) == MIN_BOARD_SIZE)
        {
          fprintf (stderr, "error: random player: no valid move found!\n");
          exit (EXIT_FAILURE);
        }

      FIND_CHAR_COLUMN (column);
      printf ("%c%zu\n", c, row + 1);
      board_free (board);
      return EXIT_SUCCESS;
    }

  if (game (black_player, white_player, board) == ERROR_CODE)
    err (EXIT_FAILURE, "the game could not start!\n");

  board_free (board);
  return EXIT_SUCCESS;
}
