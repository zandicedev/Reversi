#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#include <stdarg.h>
#include <string.h>

#include <board.h>

/* gcc -std=c11 -g -I../include -c board_tests.c       */
/* gcc -std=c11 -g -o board_tests board_tests.o board.o */

void
EXPECT(bool test, char *fmt, ...)
{
  fprintf(stdout, "Checking '");

  va_list vargs;
  va_start(vargs, fmt);
  vprintf(fmt, vargs);
  va_end(vargs);

  fprintf (stdout, "': (%s)\n", (test) ? "passed" : "failed");
}

void
ASSERT(bool test, char *fmt, ...)
{
  fprintf(stdout, "Checking '");

  va_list vargs;
  va_start(vargs, fmt);
  vprintf(fmt, vargs);
  va_end(vargs);

  if (test)
    fprintf(stdout, "': (passed)\n");
  else
    {
      fprintf(stdout, "': (critical fail!) aborting...\n");
      exit(EXIT_FAILURE);
    }
}

int
main(void)
{
  /* Free a NULL pointer */
  board_free(NULL);
  EXPECT(true, "board_free(NULL)");

  /* Allocate and free memory, get_size() and get_player() */
  board_t *board_0 = board_init(0);
  EXPECT(board_0 == NULL, "board_init(0) == NULL");

  board_0 = board_init(1);
  EXPECT(board_0 == NULL, "board_init(1) == NULL");

  board_t *board_1 = board_init(2);
  ASSERT(board_1 != NULL, "board_init(2) != NULL");
  EXPECT(board_size(board_1) == 2, "board_size() == 2");
  /* No move is possible, therefore we need to get EMPTY_DISC */
  EXPECT(board_player(board_1) == EMPTY_DISC,
	 "board_player() == EMPTY_DISC");
  board_free(board_1);

  board_0 = board_init(3);
  EXPECT(board_0 == NULL, "board_init(3) == NULL");

  board_t *board_2 = board_init(4);
  ASSERT(board_2 != NULL, "board_init(4) != NULL");
  EXPECT(board_size(board_2) == 4, "board_size() == 4");
  EXPECT(board_player(board_2) == BLACK_DISC,
	 "board_player() == BLACK_DISC");
  board_free(board_2);

  board_0 = board_init(5);
  EXPECT(board_0 == NULL, "board_init(5) == NULL");

  board_t *board_3 = board_init(6);
  ASSERT(board_3 != NULL, "board_init(6) != NULL");
  EXPECT(board_size(board_3) == 6, "board_size() == 6");
  EXPECT(board_player(board_3) == BLACK_DISC,
	 "board_player() == BLACK_DISC");
  board_free(board_3);

  board_0 = board_init(7);
  EXPECT(board_0 == NULL, "board_init(7) == NULL");

  board_t *board_4 = board_init(8);
  ASSERT(board_4 != NULL, "board_init(8) != NULL");
  EXPECT(board_size(board_4) == 8, "board_size() == 8");
  EXPECT(board_player(board_4) == BLACK_DISC,
	 "board_player() == BLACK_DISC");
  board_free(board_4);

  board_0 = board_init(9);
  EXPECT(board_0 == NULL, "board_init(9) == NULL");

  board_t *board_5 = board_init(10);
  ASSERT(board_5 != NULL, "board_init(10) != NULL");
  EXPECT(board_size(board_5) == 10, "board_size() == 10");
  EXPECT(board_player(board_5) == BLACK_DISC,
	 "board_player() == BLACK_DISC");
  board_free(board_5);

  board_0 = board_init(11);
  EXPECT(board_0 == NULL, "board_init(11) == NULL");

  /* Get and set a cell */
  board_2 =  board_init(4);

  EXPECT(board_get(board_2, 0, 0) == EMPTY_DISC,
	 "board_get(board_init(4), 0, 0) == EMPTY_DISC");

  EXPECT(board_get(board_2, 1, 1) == WHITE_DISC,
	 "board_get(board_init(4), 1, 1) == WHITE_DISC");

  EXPECT(board_get(board_2, 1, 2) == BLACK_DISC,
	 "board_get(board_init(4), 1, 2) == BLACK_DISC");

  EXPECT(board_get(board_2, 4, 2) == EMPTY_DISC,
	 "board_get(board_init(4), 4, 2) == EMPTY_DISC");

  EXPECT(board_get(board_2, 42, 42) == EMPTY_DISC,
	 "board_get(board_init(4), 42, 42) == EMPTY_DISC");

  board_set(board_2, BLACK_DISC, 0, 0);
  EXPECT(board_get(board_2, 0, 0) == BLACK_DISC,
	 "board_set(board_init(4), BLACK_DISC, 0, 0)");

  board_set(board_2, WHITE_DISC, 3, 3);
  EXPECT(board_get(board_2, 3, 3) == WHITE_DISC,
	 "board_set(board_init(4), WHITE_DISC, 3, 3)");

  board_set(board_2, EMPTY_DISC, 1, 1);
  EXPECT(board_get(board_2, 1, 1) == EMPTY_DISC,
	 "board_set(board_init(4), EMPTY_DISC, 1, 1)");

  board_set(board_2, EMPTY_DISC, 42, 42);
  EXPECT(true, "board_set(board_init(4), EMPTY_DISC, 42, 42)");

  board_set(board_2, EMPTY_DISC, -42, 42);
  EXPECT(true, "board_set(board_init(4), EMPTY_DISC, -42, 42)");

  board_free(board_2);

  EXPECT(board_get(NULL, 0, 0) == EMPTY_DISC,
	 "board_get(NULL, 0, 0) == EMPTY_DISC");

  board_set(NULL, EMPTY_DISC, 0, 0);
  EXPECT(true, "board_set(NULL, EMPTY_DISC, 0, 0)");

  /* Get and set a cell > 8 */
  board_5 =  board_init(10);

  EXPECT(board_get(board_5, 0, 0) == EMPTY_DISC,
	 "board_get(board_init(10), 0, 0) == EMPTY_DISC");

  EXPECT(board_get(board_5, 4, 4) == WHITE_DISC,
	 "board_get(board_init(10), 4, 4) == WHITE_DISC");

  EXPECT(board_get(board_5, 4, 5) == BLACK_DISC,
	 "board_get(board_init(10), 4, 5) == BLACK_DISC");

  EXPECT(board_get(board_5, 4, 2) == EMPTY_DISC,
	 "board_get(board_init(10), 4, 2) == EMPTY_DISC");

  EXPECT(board_get(board_5, 42, 42) == EMPTY_DISC,
	 "board_get(board_init(10), 42, 42) == EMPTY_DISC");

  board_set(board_5, BLACK_DISC, 0, 0);
  EXPECT(board_get(board_5, 0, 0) == BLACK_DISC,
	 "board_set(board_init(10), BLACK_DISC, 0, 0)");

  board_set(board_5, WHITE_DISC, 3, 3);
  EXPECT(board_get(board_5, 3, 3) == WHITE_DISC,
	 "board_set(board_init(10), WHITE_DISC, 3, 3)");

  board_set(board_5, EMPTY_DISC, 1, 1);
  EXPECT(board_get(board_5, 1, 1) == EMPTY_DISC,
	 "board_set(board_init(10), EMPTY_DISC, 1, 1)");

  board_set(board_5, WHITE_DISC, 9, 9);
  EXPECT(board_get(board_5, 9, 9) == WHITE_DISC,
	 "board_set(board_init(10), WHITE_DISC, 9, 9)");

  board_set(board_5, BLACK_DISC, 0, 9);
  EXPECT(board_get(board_5, 0, 9) == BLACK_DISC,
	 "board_set(board_init(10), BLACK_DISC, 0, 9)");

  board_set(board_5, BLACK_DISC, 9, 0);
  EXPECT(board_get(board_5, 9, 0) == BLACK_DISC,
	 "board_set(board_init(10), BLACK_DISC, 9, 0)");

  board_set(board_5, EMPTY_DISC, 42, 42);
  EXPECT(true, "board_set(board_init(10), EMPTY_DISC, 42, 42)");

  board_set(board_5, EMPTY_DISC, -42, 42);
  EXPECT(true, "board_set(board_init(10), EMPTY_DISC, -42, 42)");

  board_free(board_5);

  /* Copy a board */
  board_2 = board_init(4);
  board_set(board_2, WHITE_DISC, 0, 0);
  board_set(board_2, BLACK_DISC, 3, 3);
  board_set(board_2, BLACK_DISC, 2, 1); /* Try non-symetric cell */

  board_0 = board_copy(board_2);

  bool is_same = true;
  for (size_t i = 0; i < board_size(board_2); ++i)
    for (size_t j = 0; j < board_size(board_2); ++j)
      is_same &= (board_get(board_0, i, j) == board_get(board_2, i, j));
  EXPECT(is_same, "board == board_copy(board)");

  board_free(board_0);
  board_free(board_2);

  /* Copy a NULL board */
  EXPECT(board_copy(NULL) == NULL, "board_copy(NULL) == NULL");

  /* Score a board */
  board_2 = board_init(4);
  score_t score = board_score(board_2);
  EXPECT(score.black == 2 && score.white == 2,
	 "board_score(board_init(4)) == { 2, 2 }");

  /* Score empty board */
  board_set(board_2, EMPTY_DISC, 1, 1);
  board_set(board_2, EMPTY_DISC, 1, 2);
  board_set(board_2, EMPTY_DISC, 2, 1);
  board_set(board_2, EMPTY_DISC, 2, 2);
  score = board_score(board_2);
  EXPECT(score.black == 0 && score.white == 0,
	 "board_score(board_empty) == { 0, 0 }");

  /* Score full boards (black and white) of size 4 */
  for (size_t i = 0; i < board_size(board_2); ++i)
    for (size_t j = 0; j < board_size(board_2); ++j)
      board_set(board_2, BLACK_DISC, i, j);

  score = board_score(board_2);
  EXPECT(score.black == 16 && score.white == 0,
	 "board_score(board_full_black) == { 16, 0 }");

  for (size_t i = 0; i < board_size(board_2); ++i)
    for (size_t j = 0; j < board_size(board_2); ++j)
      board_set(board_2, WHITE_DISC, i, j);

  score = board_score(board_2);
  EXPECT(score.black == 0 && score.white == 16,
	 "board_score(board_full_white) == { 0, 16 }");

  board_free(board_2);

  /* Score full boards (black and white) of size 8 */
  board_4 = board_init(8);
  for (size_t i = 0; i < board_size(board_4); ++i)
    for (size_t j = 0; j < board_size(board_4); ++j)
      board_set(board_4, WHITE_DISC, i, j);

  score = board_score(board_4);
  EXPECT(score.black == 0 && score.white == 64,
	 "board_score(board_full_white) == { 0, 64 }");

  board_free(board_4);

  /* Score full boards (black and white) of size 10 */
  board_5 = board_init(10);
  for (size_t i = 0; i < board_size(board_5); ++i)
    for (size_t j = 0; j < board_size(board_5); ++j)
      board_set(board_5, WHITE_DISC, i, j);

  score = board_score(board_5);
  EXPECT(score.black == 0 && score.white == 100,
	 "board_score(board_full_white) == { 0, 100 }");

  for (size_t i = 0; i < board_size(board_5); ++i)
    for (size_t j = 0; j < board_size(board_5); ++j)
      board_set(board_5, BLACK_DISC, i, j);

  score = board_score(board_5);
  EXPECT(score.black == 100 && score.white == 0,
	 "board_score(board_full_black) == { 100, 0 }");

  board_free(board_5);

  /* Check board_print() */
  board_print(NULL, stdout);
  EXPECT(true, "board_print(NULL, stdout)");

  const char *filename = "test.txt";
  FILE *fd = fopen(filename, "w");
  board_2 = board_init(4);

  board_print(board_2, fd);

  fclose(fd);
  EXPECT(!remove(filename), "board_print(board_init(4))");
  board_free(board_2);

  /* Check board of size 8x8 */
  board_4 = board_init(8);
  board_set(board_4, BLACK_DISC, 2, 3);
  board_set(board_4, BLACK_DISC, 3, 3);
  board_set(board_4, BLACK_DISC, 3, 4);
  board_set(board_4, WHITE_DISC, 4, 2);
  board_set(board_4, WHITE_DISC, 4, 3);
  board_set(board_4, WHITE_DISC, 4, 4);

  fputs("\n", stdout);
  board_print(board_4, stdout);

  /* Check board_count_player_moves() */
  EXPECT(board_count_player_moves(board_4) == 5,
	 "board_count_player_moves(board) == 5");

  /* Check board_next_move() */
  move_t tmp_move;
  for (size_t i = 0; i <= board_count_player_moves(board_4); i++)
    {
      tmp_move = board_next_move(board_4);
      EXPECT(tmp_move.row == 5 &&
	     (tmp_move.column == 1 ||
	      tmp_move.column == 2 ||
	      tmp_move.column == 3 ||
	      tmp_move.column == 4 ||
	      tmp_move.column == 5),
	     "board_next_move() == { 5, %zu }", tmp_move.column);
    }

  /* Check board_is_move_valid() */
  EXPECT(!board_is_move_valid(board_4, (move_t) { 5, 0 }),
	 "board_is_move_valid(5,0) == false");

  for (size_t i = 0; i < 5; i++)
    {
      EXPECT(board_is_move_valid(board_4, (move_t) { 5, i + 1 }),
	     "board_is_move_valid(5, %zu) == true", i + 1);
    }

  EXPECT(!board_is_move_valid(board_4, (move_t) { 5, 6 }),
	 "board_is_move_valid(5,6) == false");

  EXPECT(!board_is_move_valid(board_4, (move_t) { 5, 7 }),
	 "board_is_move_valid(5,7) == false");

  EXPECT(!board_is_move_valid(board_4, (move_t) { 42, 42 }),
	 "board_is_move_valid(42,42) == false");

  EXPECT(!board_is_move_valid(board_4, (move_t) { -42, 42 }),
	 "board_is_move_valid(-42,42) == false");

  board_free(board_4);

  /* Check board of size 10x10 */
  board_5 = board_init(10);
  board_set(board_5, BLACK_DISC, 4, 4);
  board_set(board_5, BLACK_DISC, 3, 5);
  board_set(board_5, WHITE_DISC, 3, 4);
  board_set(board_5, WHITE_DISC, 5, 3);
  board_set(board_5, WHITE_DISC, 5, 4);
  board_set(board_5, WHITE_DISC, 5, 5);

  fputs("\n", stdout);
  board_print(board_5, stdout);

  /* Check board_count_player_moves() */
  EXPECT(board_count_player_moves(board_5) == 8,
	 "board_count_player_moves(board) == 8");

  /* Check board_next_move() */
  for (size_t i = 0; i <= board_count_player_moves(board_5); i++)
    {
      tmp_move = board_next_move(board_5);
      EXPECT(((tmp_move.row == 2 && tmp_move.column == 3) ||
	      (tmp_move.row == 2 && tmp_move.column == 4) ||
	      (tmp_move.row == 3 && tmp_move.column == 3) ||
	      (tmp_move.row == 6 && tmp_move.column == 2) ||
	      (tmp_move.row == 6 && tmp_move.column == 3) ||
	      (tmp_move.row == 6 && tmp_move.column == 4) ||
	      (tmp_move.row == 6 && tmp_move.column == 5) ||
	      (tmp_move.row == 6 && tmp_move.column == 6)),
	     "board_next_move() == { %zu, %zu }",
	     tmp_move.row, tmp_move.column);
    }

  /* Check board_is_move_valid() */
  EXPECT(!board_is_move_valid(board_5, (move_t) { 2, 2 }),
	 "board_is_move_valid(2,2) == false");

  EXPECT(board_is_move_valid(board_5, (move_t) { 2, 3 }),
	 "board_is_move_valid(2,3) == true");

  EXPECT(board_is_move_valid(board_5, (move_t) { 2, 4 }),
	 "board_is_move_valid(2,4) == true");

  EXPECT(board_is_move_valid(board_5, (move_t) { 3, 3 }),
	 "board_is_move_valid(3,3) == true");

  EXPECT(board_is_move_valid(board_5, (move_t) { 6, 2 }),
	 "board_is_move_valid(6,2) == true");

  EXPECT(board_is_move_valid(board_5, (move_t) { 6, 3 }),
	 "board_is_move_valid(6,3) == true");

  EXPECT(board_is_move_valid(board_5, (move_t) { 6, 4 }),
	 "board_is_move_valid(6,4) == true");

  EXPECT(board_is_move_valid(board_5, (move_t) { 6, 5 }),
	 "board_is_move_valid(6,5) == true");

  EXPECT(board_is_move_valid(board_5, (move_t) { 6, 6 }),
	 "board_is_move_valid(6,6) == true");

  EXPECT(!board_is_move_valid(board_5, (move_t) { 6, 7 }),
	 "board_is_move_valid(6,7) == false");

  EXPECT(!board_is_move_valid(board_5, (move_t) { 42, 42 }),
	 "board_is_move_valid(42,42) == false");

  EXPECT(!board_is_move_valid(board_5, (move_t) { -42, 42 }),
	 "board_is_move_valid(-42,42) == false");

  board_free(board_5);

  /* Check board_play() */
  board_4 = board_init(8);
  board_set(board_4, WHITE_DISC, 4, 2);
  board_set(board_4, WHITE_DISC, 4, 3);
  board_set(board_4, WHITE_DISC, 4, 4);
  board_set(board_4, BLACK_DISC, 2, 3);
  board_set(board_4, BLACK_DISC, 3, 3);
  board_set(board_4, BLACK_DISC, 3, 4);

  fputs("\n", stdout);
  board_print(board_4, stdout);

  EXPECT(board_play(board_4, (move_t) { 5, 5 }),
	 "board_play(5,5) == true");

  EXPECT(board_get(board_4, 5, 5) == BLACK_DISC,
	 "board_get(5,5) == BLACK_DISC");

  EXPECT(board_get(board_4, 4, 4) == BLACK_DISC,
	 "board_get(4,4) == BLACK_DISC");

  fputs("\n", stdout);
  board_print(board_4, stdout);

  EXPECT(board_play(board_4, (move_t) { 1, 3 }),
	 "board_play(1,3) == true");

  EXPECT(board_get(board_4, 1, 3) == WHITE_DISC,
	 "board_get(1,3) == WHITE_DISC");

  EXPECT(board_get(board_4, 2, 3) == WHITE_DISC,
	 "board_get(2,3) == WHITE_DISC");

  EXPECT(board_get(board_4, 3, 3) == WHITE_DISC,
	 "board_get(3,3) == WHITE_DISC");

  fputs("\n", stdout);
  board_print(board_4, stdout);

  EXPECT(board_play(board_4, (move_t) { 3, 2 }),
	 "board_play(3,2) == true");

  EXPECT(board_get(board_4, 3, 2) == BLACK_DISC,
	 "board_get(3,2) == BLACK_DISC");

  EXPECT(board_get(board_4, 3, 3) == BLACK_DISC,
	 "board_get(3,3) == BLACK_DISC");

  fputs("\n", stdout);
  board_print(board_4, stdout);

  EXPECT(board_play(board_4, (move_t) { 4, 5 }),
	 "board_play(4,5) == true");

  EXPECT(board_get(board_4, 4, 5) == WHITE_DISC,
	 "board_get(4,5) == WHITE_DISC");

  EXPECT(board_get(board_4, 4, 5) == WHITE_DISC,
	 "board_get(4,5) == WHITE_DISC");

  EXPECT(board_get(board_4, 3, 4) == WHITE_DISC,
	 "board_get(3,4) == WHITE_DISC");

  EXPECT(board_get(board_4, 4, 4) == WHITE_DISC,
	 "board_get(4,4) == WHITE_DISC");

  fputs("\n", stdout);
  board_print(board_4, stdout);

  EXPECT(!board_play(board_4, (move_t) { 0, 0 }),
	 "board_play(0,0) == false");

  board_free(board_4);

  board_2 = board_init(4);
  board_set(board_2, BLACK_DISC, 0, 0);
  board_set(board_2, WHITE_DISC, 1, 1);
  board_set(board_2, WHITE_DISC, 2, 2);
  board_set(board_2, EMPTY_DISC, 2, 1);
  board_set(board_2, EMPTY_DISC, 1, 2);

  fputs("\n", stdout);
  board_print(board_2, stdout);

  EXPECT(board_play(board_2, (move_t) { 3, 3 }),
	 "board_play(3,3) == true");
  EXPECT(board_player(board_2) == EMPTY_DISC,
	 "board_player() == EMPTY_DISC");

  board_free(board_2);

  /* Check board border side-effects */
  /* North */
  board_0 = board_alloc(6, BLACK_DISC);
  board_set(board_0, BLACK_DISC, 0, 0);
  board_set(board_0, WHITE_DISC, 0, 1);
  board_set(board_0, WHITE_DISC, 0, 2);
  board_set(board_0, WHITE_DISC, 0, 3);
  board_set(board_0, WHITE_DISC, 0, 4);

  fputs("\n", stdout);
  board_print(board_0, stdout);

  EXPECT(board_play(board_0, (move_t) { 0, 5 }),
	 "board_play(0,5) == true");

  fputs("\n", stdout);
  board_print(board_0, stdout);

  for (size_t i = 0; i < 6; i++)
    EXPECT(board_get(board_0, 0, i) == BLACK_DISC,
	   "board_get(0,%zu) == BLACK_DISC", i);

  for (size_t i = 0; i < 6; i++)
    for (size_t j = 1; j < 6; j++)
      EXPECT(board_get(board_0, j, i) == EMPTY_DISC,
	     "board_get(%zu,%zu) == EMPTY_DISC", i, j);

  board_free(board_0);

  /* South */
  board_0 = board_alloc(6, BLACK_DISC);
  board_set(board_0, WHITE_DISC, 5, 1);
  board_set(board_0, WHITE_DISC, 5, 2);
  board_set(board_0, WHITE_DISC, 5, 3);
  board_set(board_0, WHITE_DISC, 5, 4);
  board_set(board_0, BLACK_DISC, 5, 5);

  fputs("\n", stdout);
  board_print(board_0, stdout);

  EXPECT(board_play(board_0, (move_t) { 5, 0 }),
	 "board_play(0,5) == true");

  fputs("\n", stdout);
  board_print(board_0, stdout);

  for (size_t i = 0; i < 6; i++)
    EXPECT(board_get(board_0, 5, i) == BLACK_DISC,
	   "board_get(0,%zu) == BLACK_DISC", i);

  for (size_t i = 0; i < 6; i++)
    for (size_t j = 0; j < 5; j++)
      EXPECT(board_get(board_0, j, i) == EMPTY_DISC,
	     "board_get(%zu,%zu) == EMPTY_DISC", i, j);

  board_free(board_0);

  /* East */
  board_0 = board_alloc(6, BLACK_DISC);
  board_set(board_0, WHITE_DISC, 1, 5);
  board_set(board_0, WHITE_DISC, 2, 5);
  board_set(board_0, WHITE_DISC, 3, 5);
  board_set(board_0, WHITE_DISC, 4, 5);
  board_set(board_0, BLACK_DISC, 5, 5);

  fputs("\n", stdout);
  board_print(board_0, stdout);

  EXPECT(board_play(board_0, (move_t) { 0, 5 }),
	 "board_play(0,5) == true");

  fputs("\n", stdout);
  board_print(board_0, stdout);

  for (size_t i = 0; i < 6; i++)
    EXPECT(board_get(board_0, i, 5) == BLACK_DISC,
	   "board_get(0,%zu) == BLACK_DISC", i);

  for (size_t i = 0; i < 5; i++)
    for (size_t j = 0; j < 6; j++)
      EXPECT(board_get(board_0, j, i) == EMPTY_DISC,
	     "board_get(%zu,%zu) == EMPTY_DISC", i, j);

  board_free(board_0);


  /* West */
  board_0 = board_alloc(6, BLACK_DISC);
  board_set(board_0, BLACK_DISC, 0, 0);
  board_set(board_0, WHITE_DISC, 1, 0);
  board_set(board_0, WHITE_DISC, 2, 0);
  board_set(board_0, WHITE_DISC, 3, 0);
  board_set(board_0, WHITE_DISC, 4, 0);

  fputs("\n", stdout);
  board_print(board_0, stdout);

  EXPECT(board_play(board_0, (move_t) { 5, 0 }),
	 "board_play(0,5) == true");

  fputs("\n", stdout);
  board_print(board_0, stdout);

  for (size_t i = 0; i < 6; i++)
    EXPECT(board_get(board_0, i, 0) == BLACK_DISC,
	   "board_get(0,%zu) == BLACK_DISC", i);

  for (size_t i = 1; i < 6; i++)
    for (size_t j = 0; j < 6; j++)
      EXPECT(board_get(board_0, j, i) == EMPTY_DISC,
	     "board_get(%zu,%zu) == EMPTY_DISC", i, j);

  board_free(board_0);

  return EXIT_SUCCESS;
}
