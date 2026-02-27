#include "board.h"

#include <stdlib.h>

#include <assert.h>
#include <stdint.h>

#define DIRECTIONS 8
#define CHECK_FPRINTF(fd, ...)                                                \
  {                                                                           \
    int cmp = fprintf (fd, ##__VA_ARGS__);                                    \
    if (cmp <= 0)                                                             \
      return -1;                                                              \
    sum += cmp;                                                               \
  }

struct queue_s
{
  struct element_s *head;
  struct element_s *tail;
  unsigned int length;
};

struct element_s
{
  void *data;
  struct element_s *next;
  struct element_s *prev;
};
typedef struct queue_s queue;
typedef struct element_s element_t;

struct board_t
{
  size_t size;
  disc_t player;
  bitboard_t black;
  bitboard_t white;
  bitboard_t moves;
  bitboard_t next_move;
  queue *undo_stack;
  queue *redo_stack;
};

static queue *
queue_new ()
{
  queue *queue_instance = malloc (sizeof (queue));
  assert (queue_instance);
  queue_instance->length = 0;
  queue_instance->tail = queue_instance->head = NULL;
  return queue_instance;
}

static void
queue_push_head (queue *queue_instance, void *value)
{
  assert (queue_instance);
  element_t *new_element = malloc (sizeof (element_t));
  assert (new_element);
  new_element->data = value;
  new_element->prev = NULL;
  new_element->next = queue_instance->head;

  if (queue_instance->head)
    queue_instance->head->prev = new_element;

  queue_instance->head = new_element;

  if (!queue_instance->tail)
    queue_instance->tail = new_element;

  queue_instance->length++;
}

static void *
queue_pop_head (queue *queue_instance)
{
  assert (queue_instance);
  assert (queue_instance->length > 0);

  if (!queue_instance->head)
    return NULL;

  void *value = queue_instance->head->data;
  element_t *next_element = queue_instance->head->next;

  if (next_element)
    next_element->prev = NULL;

  free (queue_instance->head);
  queue_instance->head = next_element;
  queue_instance->length--;

  if (!queue_instance->head)
    queue_instance->tail = NULL; 

  return value;
}

static int
queue_length (const queue *queue_instance)
{
  assert (queue_instance);
  return queue_instance->length;
}

static bool
queue_is_empty (const queue *queue_instance)
{
  assert (queue_instance);
  return (queue_instance->length == 0);
}

static void
queue_free_full (queue *queue_instance, void (*destroy_function) (void *))
{
  assert (queue_instance);

  element_t *current_element = queue_instance->head;

  while (current_element)
    {
      element_t *element_to_free = current_element;

      if (destroy_function)
        destroy_function (current_element->data);

      current_element = current_element->next;
      free (element_to_free);
    }

  queue_instance->head = queue_instance->tail = NULL;
  queue_instance->length = 0;
  free (queue_instance);
}

static bitboard_t
shift_north (const size_t size, const bitboard_t bitboard)
{
  return bitboard << size;
}

static bitboard_t
shift_south (const size_t size, const bitboard_t bitboard)
{
  return bitboard >> size;
}

static bitboard_t
shift_east (const size_t size, const bitboard_t bitboard)
{
  bitboard_t mask = 0;
  for (size_t i = size - 1; i < size * size; i += size)
    {
      mask |= (bitboard_t)1 << i;
    }

  return bitboard >> 1 & ~mask;
}

static bitboard_t
shift_west (const size_t size, const bitboard_t bitboard)
{
  bitboard_t mask = 0;
  for (size_t i = 0; i < size; i++)
    {
      mask |= (((bitboard_t)1) << (i * size));
    }

  return (bitboard << 1) & ~(mask);
}

static bitboard_t
shift_nw (const size_t size, const bitboard_t bitboard)
{
  return shift_west (size, shift_north (size, bitboard));
}

static bitboard_t
shift_sw (const size_t size, const bitboard_t bitboard)
{
  return shift_west (size, shift_south (size, bitboard));
}

static bitboard_t
shift_se (const size_t size, const bitboard_t bitboard)
{
  return shift_east (size, shift_south (size, bitboard));
}

static bitboard_t
shift_ne (const size_t size, const bitboard_t bitboard)
{
  return shift_east (size, shift_north (size, bitboard));
}

static bitboard_t (*shifts[DIRECTIONS]) (const size_t size, const bitboard_t)
    = { shift_north, shift_ne, shift_east, shift_se,
        shift_south, shift_sw, shift_west, shift_nw };

static bitboard_t
set_bitboard (const size_t size, const size_t row, const size_t column)
{
  if (size < MIN_BOARD_SIZE || size > MAX_BOARD_SIZE || row >= size
      || column >= size)
    return 0;

  bitboard_t bits = 1;
  return bits << (row * size + column);
}

static bitboard_t
compute_moves (const size_t size, const bitboard_t player,
               const bitboard_t opponent)
{
  if (!size || !player || !opponent)
    return 0;

  bitboard_t possible_bit;
  bitboard_t moves = 0;
  bitboard_t empty_square = ~(player | opponent);
  bitboard_t result;
  for (int d = 0; d < DIRECTIONS; d++)
    {
      possible_bit = shifts[d](size, player) & opponent;
      result = 0;
      while (possible_bit)
        {
          result |= possible_bit;
          possible_bit = shifts[d](size, possible_bit) & opponent;
        }

      moves |= shifts[d](size, result) & empty_square;
    }

  bitboard_t mask = 0;
  for (size_t i = 0; i < size * size; i++)
    mask |= (bitboard_t)1 << i;

  return moves & mask;
}

board_t *
board_alloc (const size_t size, const disc_t player)
{
  if (size < MIN_BOARD_SIZE || size > MAX_BOARD_SIZE
      || !(player == BLACK_DISC || player == WHITE_DISC))
    return NULL;

  board_t *ptr = malloc (sizeof (board_t));
  if (!ptr)
    return NULL;

  ptr->size = size;
  ptr->player = player;
  ptr->black = 0;
  ptr->white = 0;
  ptr->moves = 0;
  ptr->next_move = 0;
  ptr->undo_stack = queue_new ();
  ptr->redo_stack = queue_new ();
  return ptr;
}

void
board_free (board_t *board)
{
  if (!board)
    return;

  queue_free_full (board->undo_stack, free);
  queue_free_full (board->redo_stack, free);
  free (board);
}

static bitboard_t *
board_get_bitboard (const board_t *board)
{
  if (!board)
    return NULL;

  bitboard_t *bitboard = malloc (2 * sizeof (bitboard_t));
  if (!bitboard)
    return NULL;

  bitboard[0] = board->black;
  bitboard[1] = board->white;
  return bitboard;
}

void
undo_redo_init (board_t *board)
{
  if (!board)
    return;

  bitboard_t *initial_state = board_get_bitboard (board);
  queue_push_head (board->undo_stack, initial_state);
}

board_t *
board_init (const size_t size)
{
  if (size < MIN_BOARD_SIZE || size > MAX_BOARD_SIZE || size % 2 == 1)
    return NULL;

  size_t center = size / 2 - 1;
  board_t *board = board_alloc (size, BLACK_DISC);
  if (!board)
    return NULL;

  board_set (board, WHITE_DISC, center, center);
  board_set (board, BLACK_DISC, center, center + 1);
  board_set (board, BLACK_DISC, center + 1, center);
  board_set (board, WHITE_DISC, center + 1, center + 1);
  board->moves
      = compute_moves (board_size (board), board->black, board->white);
  board->next_move = board->moves;
  if (size == 2)
    {
      board->player = EMPTY_DISC;
      board->moves = 0;
    }
  undo_redo_init (board);
  return board;
}

board_t *
board_copy (const board_t *board)
{
  if (!board)
    return NULL;

  disc_t player = board_player (board);
  size_t size = board_size (board);

  board_t *board_copy = board_alloc (size, player);
  if (!board_copy)
    return NULL;

  board_copy->black = board->black;
  board_copy->white = board->white;
  board_copy->moves = board->moves;
  board_copy->next_move = board->next_move;

  return board_copy;
}

size_t
board_size (const board_t *board)
{
  if (!board)
    return 0;

  return board->size;
}

disc_t
board_player (const board_t *board)
{
  if (!board)
    return HINT_DISC;

  return board->player;
}

void
board_set_player (board_t *board, disc_t player)
{
  if (!board || !player)
    return;
  board->player = player;
}

disc_t
board_get (const board_t *board, const size_t row, const size_t column)
{
  if (!board || row >= board->size || column >= board->size)
    {
      return EMPTY_DISC;
    }

  bitboard_t bits = set_bitboard (board->size, row, column);

  if ((board->black & bits) != 0)
    {
      return BLACK_DISC;
    }

  if ((board->white & bits) != 0)
    {
      return WHITE_DISC;
    }

  return EMPTY_DISC;
}

static void
flip_discs (board_t *board, bitboard_t bitboard, disc_t disc)
{
  if (!bitboard || disc == EMPTY_DISC)
    return;

  bitboard_t player = 0;
  bitboard_t opponent = 0;
  bitboard_t result = 0;
  bitboard_t possible_case = 0;
  bitboard_t flipped_discs = 0;
  if (disc == BLACK_DISC)
    {
      player = board->black;
      opponent = board->white;
    }
  else
    {
      player = board->white;
      opponent = board->black;
    }

  for (int d = 0; d < DIRECTIONS; d++)
    {
      possible_case = shifts[d](board->size, bitboard);
      flipped_discs = 0;

      while (possible_case & opponent)
        {
          flipped_discs |= possible_case;
          possible_case = shifts[d](board->size, possible_case);
        }

      if (possible_case & player)
        {
          result |= flipped_discs;
        }
    }

  if (disc == WHITE_DISC)
    {
      board->white |= result;
      board->black &= ~result;
    }
  else
    {
      board->black |= result;
      board->white &= ~result;
    }
}

void
board_set (board_t *board, const disc_t disc, size_t row, const size_t column)
{
  if (!board || !disc || row > board->size || column > board->size)
    return;

  bitboard_t player = 0;
  bitboard_t opponent = 0;
  disc_t current_player = board_player (board);
  bitboard_t bits = set_bitboard (board->size, row, column);

  if (disc == BLACK_DISC)
    {
      board->black |= bits;
      board->white &= ~(bits);
    }
  else if (disc == WHITE_DISC)
    {
      board->white |= bits;
      board->black &= ~bits;
    }
  else if (disc == EMPTY_DISC)
    {
      board->black &= ~bits;
      board->white &= ~bits;
    }

  player = (current_player == BLACK_DISC) ? board->black : board->white;
  opponent = (current_player == BLACK_DISC) ? board->white : board->black;
  board->moves = compute_moves (board->size, player, opponent);
  board->next_move = board->moves;
}

/* https://www.playingwithpointers.com/blog/swar.html    Sanjoy Das*/
static int
pop_cnt_64 (uint64_t i)
{
  i = i - ((i >> 1) & 0x5555555555555555);
  i = (i & 0x3333333333333333) + ((i >> 2) & 0x3333333333333333);
  return (((i + (i >> 4)) & 0x0F0F0F0F0F0F0F0F) * 0x0101010101010101) >> 56;
}

static size_t
bitboard_popcount (bitboard_t bitboard)
{
  uint64_t a = (uint64_t)bitboard;
  uint64_t b = (uint64_t)(bitboard >> 64);
  size_t result = pop_cnt_64 (a) + pop_cnt_64 (b);
  return result;
}

size_t
board_count_player_moves (board_t *board)
{
  if (!board)
    return 0;
  bitboard_t moves = board->moves;
  return bitboard_popcount (moves);
}

int
board_print (const board_t *board, FILE *fd)
{
  if (!board || !fd)
    return -1;

  int sum = 0;
  size_t size = board_size (board);
  disc_t disc;
  bitboard_t moves = board->moves;
  size_t i = 0;
  disc_t player = board_player (board);
  if (player == BLACK_DISC || player == WHITE_DISC)
    {
      CHECK_FPRINTF (fd, "'%c' player's turn.\n", player);
    }
  CHECK_FPRINTF (fd, "\n   ");
  for (char c = 'A'; i < size; c++)
    {
      CHECK_FPRINTF (fd, "%c ", c);
      i++;
    }

  for (size_t i = 0; i < size; i++)
    {
      CHECK_FPRINTF (fd, "\n");
      CHECK_FPRINTF (fd, (i == 9) ? "%zu " : " %zu ", i + 1);

      for (size_t j = 0; j < size; j++)
        {
          disc = board_get (board, i, j);
          if ((moves >> ((i * size) + j) & 1) != 0)
            {
              CHECK_FPRINTF (fd, "%c ", HINT_DISC);
            }
          else
            {
              CHECK_FPRINTF (fd, "%c ", disc);
            }
        }
    }

  score_t score = board_score (board);
  CHECK_FPRINTF (fd, "\n\n");
  CHECK_FPRINTF (fd, "Score: '%c' = %u, '%c' = %u \n", BLACK_DISC, score.black,
                 WHITE_DISC, score.white);
  return sum;
}

int
board_print_help (const board_t *board, move_t move)
{
  if (!board)
    return -1;

  int sum = 0;
  size_t size = board_size (board);
  disc_t disc;
  bitboard_t moves = board->moves;
  size_t i = 0;
  disc_t player = board_player (board);
  if (player == BLACK_DISC || player == WHITE_DISC)
    printf ("'%c' player's turn.\n", player);

  printf ("\n   ");
  for (char c = 'A'; i < size; c++)
    {
      printf ("%c ", c);
      i++;
    }

  for (size_t i = 0; i < size; i++)
    {
      printf ("\n");
      printf ((i == 9) ? "%zu " : " %zu ", i + 1);
      for (size_t j = 0; j < size; j++)
        {
          disc = board_get (board, i, j);
          if (i == move.row && j == move.column)
            {
              printf ("%c ", '+');
              continue;
            }

          if ((moves >> ((i * size) + j) & 1) != 0)
            printf ("%c ", HINT_DISC);
          else
            printf ("%c ", disc);
        }
    }

  score_t score = board_score (board);
  printf ("\n\n");
  printf ("Score: '%c' = %u, '%c' = %u \n", BLACK_DISC, score.black,
          WHITE_DISC, score.white);
  return sum;
}

score_t
board_score (const board_t *board)
{
  if (!board)
    return (score_t){ .black = 0, .white = 0 };

  score_t score;
  score.black = bitboard_popcount (board->black);
  score.white = bitboard_popcount (board->white);
  return score;
}

bool
board_is_move_valid (const board_t *board, const move_t move)
{
  if (!board || move.row >= board->size || move.column >= board->size)
    return false;

  size_t size = board_size (board);
  bitboard_t bit_position = set_bitboard (size, move.row, move.column);

  return board->moves & bit_position;
}

static void
board_set_bitboard (board_t *board, bitboard_t *bitboard)
{
  if (!board || !bitboard)
    return;

  board->black = bitboard[0];
  board->white = bitboard[1];
  bitboard_t player
      = (board_player (board) == BLACK_DISC) ? board->black : board->white;
  bitboard_t opponent
      = (board_player (board) == BLACK_DISC) ? board->white : board->black;
  board->moves = compute_moves (board_size (board), player, opponent);
  board->next_move = board->moves;
}

void
board_undo (board_t *board)
{
  if (!board || queue_is_empty (board->undo_stack)
      || queue_length (board->undo_stack) <= 2)
    {
      printf ("Undo impossible: no move to undo!\n");
      return;
    }

  bitboard_t *opponent_state = queue_pop_head (board->undo_stack);
  queue_push_head (board->redo_stack, board_get_bitboard (board));
  board_set_bitboard (board, opponent_state);
  bitboard_t *player_state = queue_pop_head (board->undo_stack);
  queue_push_head (board->redo_stack, board_get_bitboard (board));
  board_set_bitboard (board, player_state);
}

void
board_redo (board_t *board)
{
  if (!board || queue_is_empty (board->redo_stack)
      || queue_length (board->redo_stack) < 2)
    {
      printf ("Redo impossible: no move to redo!\n");
      return;
    }

  bitboard_t *player_state = queue_pop_head (board->redo_stack);
  queue_push_head (board->undo_stack, board_get_bitboard (board));
  board_set_bitboard (board, player_state);
  bitboard_t *opponent_state = queue_pop_head (board->redo_stack);
  queue_push_head (board->undo_stack, board_get_bitboard (board));
  board_set_bitboard (board, opponent_state);
}

bool
board_play (board_t *board, const move_t move)
{
  if (!board || move.column > board->size || move.row > board->size)
    return false;

  bitboard_t next_move_player = 0;
  size_t size = board_size (board);
  disc_t current_player = board_player (board);
  bitboard_t opponent = 0;
  bitboard_t player = 0;
  move_t next_move = move;
  bool valid = board_is_move_valid (board, next_move);
  if (!valid)
    return false;

  bitboard_t *current_state = board_get_bitboard (board);
  queue_push_head (board->undo_stack, current_state);
  board_set (board, current_player, next_move.row, next_move.column);
  bitboard_t bits
      = set_bitboard (board->size, next_move.row, next_move.column);
  flip_discs (board, bits, current_player);
  if (current_player == BLACK_DISC)
    {
      player = board->black;
      opponent = board->white;
      board_set_player (board, WHITE_DISC);
      current_player = WHITE_DISC;
    }
  else
    {
      player = board->white;
      opponent = board->black;
      board_set_player (board, BLACK_DISC);
      current_player = BLACK_DISC;
    }

  board->moves = compute_moves (size, opponent, player);
  board->next_move = board->moves;
  next_move_player = compute_moves (size, player, opponent);
  if (board->moves == 0 && next_move_player == 0)
    {
      board_set_player (board, EMPTY_DISC);
      return true;
    }
  else if (board->moves == 0)
    {
      if (board_player (board) == BLACK_DISC)
        {
          board_set_player (board, WHITE_DISC);
          board->moves
              = compute_moves (board_size (board), board->white, board->black);
          board->next_move = board->moves;
        }
      else
        {
          board_set_player (board, BLACK_DISC);
          board->moves
              = compute_moves (board_size (board), board->black, board->white);
          board->next_move = board->moves;
        }
    }

  return true;
}

move_t
board_next_move (board_t *board)
{
  if (!board)
    return (move_t){ .row = board_size (board), .column = board_size (board) };

  move_t move = { .row = board_size (board), .column = board_size (board) };
  disc_t current_player = board_player (board);
  size_t size = board_size (board);
  bitboard_t player = 0;
  bitboard_t opponent = 0;
  size_t position = 0;

  player = (current_player == BLACK_DISC) ? board->black : board->white;
  opponent = (current_player == BLACK_DISC) ? board->white : board->black;

  if (board->next_move == 0)
    {
      board->next_move = compute_moves (board_size (board), player, opponent);
      if (board->next_move == 0)
        return move;
    }

  bitboard_t bits = board->next_move;
  bitboard_t bitboard = board->next_move;
  while (bits)
    {
      if (bits % 2 == 1)
        {
          break;
        }
      bits = bits >> 1;
      position++;
    }

  if (position > size * size)
    return (move_t){ .row = board_size (board), .column = board_size (board) };

  bitboard = 1;
  bitboard = bitboard << position;
  board->next_move = board->next_move & (~bitboard);
  move.row = (position / size);
  move.column = (position % size);
  return move;
}
