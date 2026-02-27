#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int
run_test (const char *command, int expected_exit_code,
          const char *expected_stderr, const char *description)
{
  printf ("===== Check: %s\n", description);
  const char *stdout_tmp = "stdout.txt";
  const char *stderr_tmp = "stderr.txt";
  char commands[BUFFER_SIZE];
  snprintf (commands, sizeof (commands), "%s > %s 2> %s", command, stdout_tmp,
            stderr_tmp);
  int exit_status = system (commands);
  if (exit_status == -1)
    {
      perror ("system");
      return 0;
    }

  int exit_code = WEXITSTATUS (exit_status);
  if (exit_code != expected_exit_code)
    {
      printf ("----Fail----: expected code: %d, return code : %d\n",
              expected_exit_code, exit_code);
      return 0;
    }

  if (expected_stderr)
    {
      FILE *fd = fopen (stderr_tmp, "r");
      if (!fd)
        {
          perror ("fd");
          return 0;
        }

      fseek (fd, 0, SEEK_END);
      long size = ftell (fd);
      fclose (fd);
      if (size > 0)
        {
          printf ("Result : ** Passed **\n");
        }
      else
        {
          printf ("----Fail----\n");
          return 0;
        }
    }
  printf ("Result : ** Passed **\n");
  return 1;
}

int
main ()
{
  pid_t pid;
  struct TestCase
  {
    const char *command;
    int expected_exit_code;
    const char *expected_stderr;
    const char *description;

  } tests[] = {
    { "echo -e 'Q\n\n'| ../reversi board_parser/board-0x0.fail", 1, NULL,
      "./reversi board_parser/board-0x0.fail" },
    { "echo -e 'Q\n\n'| ../reversi board_parser/board-1x1.fail", 1, NULL,
      "./reversi board_parser/board-1x1.fail" },
    { "echo -e 'Q\n\n'| ../reversi board_parser/board-2x2.pass", 0, NULL,
      "./reversi board_parser/board-2x2.pass" },
    { "echo -e 'Q\n\n'| ../reversi board_parser/board-3x3.fail ", 1, NULL,
      "./reversi board_parser/board-3x3.fail " },
    { "echo -e 'Q\n\n'| ../reversi board_parser/board-4x4.pass", 0, NULL,
      "./reversi board_parser/board-4x4.pass" },
    { "echo -e 'Q\n\n'| ../reversi board_parser/board-5x5.fail", 1, NULL,
      "./reversi board_parser/board-5x5.fail" },
    { "echo -e 'Q\n\n'| ../reversi board_parser/board-6x6.pass", 0, NULL,
      "./reversi board_parser/board-6x6.pass" },
    { "echo -e 'Q\n\n'| ../reversi board_parser/board-7x7.fail", 1, NULL,
      "./reversi board_parser/board-7x7.fail" },
    { "echo -e 'Q\n\n'| ../reversi board_parser/board-8x8.pass", 0, NULL,
      "./reversi board_parser/board-8x8.pass" },
    { "echo -e 'Q\n\n'| ../reversi board_parser/board-9x9.fail", 1, NULL,
      "./reversi board_parser/board-9x9.fail" },
    { "echo -e 'Q\n\n'| ../reversi board_parser/board-10x10.pass", 0, NULL,
      "./reversi board_parser/board-10x10.pass" },
    { "echo -e 'Q\n\n'| ../reversi board_parser/board-11x11.fail", 1, NULL,
      "./reversi board_parser/board-11x11.fail" },
    { "echo -e 'Q\n\n'| ../reversi board_parser/board-12x12.fail", 1, NULL,
      "./reversi board_parser/board-12x12.fail" },
    { "echo -e 'Q\n\n'| ../reversi board_parser/ "
      "board-first_line_overflow.fail",
      1, NULL, "./reversi board_parser/board-first_line_overflow.fail" },
    { "echo -e 'Q\n\n'| ../reversi board_parser/board-full_board.pass", 0,
      NULL, "./reversi board_parser/board-full_board.pass" },
    { "echo -e 'Q\n\n'| ../reversi "
      "board_parser/board-impossible_board-01.pass",
      0, NULL, "./reversi board_parser/board-impossible_board-01.pass" },
    { "echo -e 'Q\n\n'| ../reversi "
      "board_parser/board-impossible_board-02.pass",
      0, NULL, "./reversi board_parser/board-impossible_board-02.pass" },
    { "echo -e 'Q\n\n'| ../reversi board_parser/board-line_too_long.fail", 1,
      NULL, "./reversi board_parser/board-line_too_long.fail" },
    { "echo -e 'Q\n\n'| ../reversi board_parser/board-line_too_short.fail", 1,
      NULL, "./reversi board_parser/board-line_too_short.fail" },
    { "echo -e 'Q\n\n'| ../reversi "
      "board_parser/board-line_too_short_with_comment.fail",
      1, NULL,
      "./reversi board_parser/board-line_too_short_with_comment.fail" },
    { "echo -e 'Q\n\n'| ../reversi "
      "board_parser/board-line_too_short_with_no_newline.fail",
      1, NULL,
      "./reversi board_parser/board-line_too_short_with_no_newline.fail" },
    { "echo -e 'Q\n\n'| ../reversi "
      "board_parser/board-long_line_filled_with_spaces.pass",
      0, NULL,
      "./reversi board_parser/board-long_line_filled_with_spaces.pass" },
    { "echo -e 'Q\n\n'| ../reversi board_parser/board-missing_board.fail", 1,
      NULL, "./reversi board_parser/board-missing_board.fail" },
    { "echo -e 'Q\n\n'| ../reversi "
      "board_parser/board-missing_current_player.fail",
      1, NULL, "./reversi board_parser/board-missing_current_player.fail" },
    { "echo -e 'Q\n\n'| ../reversi "
      "board_parser/board-missing_newline_after_current_player.pass",
      0, NULL,
      "./reversi "
      "board_parser/board-missing_newline_after_current_player.pass" },
    { "echo -e 'Q\n\n'| ../reversi board_parser/board-no_final_newline.pass",
      0, NULL, "./reversi board_parser/board-no_final_newline.pass" },
    { "echo -e 'Q\n\n'| ../reversi board_parser/board-almost_full_board.pass",
      0, NULL, "./reversi board_parser/board-almost_full_board.pass" },
    { "echo -e 'Q\n\n'| ../reversi "
      "board_parser/board-current_player_get_two_chars.fail",
      1, NULL,
      "./reversi board_parser/board-current_player_get_two_chars.fail" },
    { "echo -e 'Q\n\n'| ../reversi board_parser/board-empty_board.pass", 0,
      NULL, "./reversi board_parser/board-empty_board.pass" },
    { "echo -e 'Q\n\n'| ../reversi board_parser/board-empty_file.fail", 1,
      NULL, "./reversi board_parser/board-empty_file.fail" },
    { "echo -e 'Q\n\n'| ../reversi "
      "board_parser/board-empty_stone_as_current_player.fail",
      1, NULL,
      "./reversi board_parser/board-empty_stone_as_current_player.fail" },
    { "echo -e 'Q\n\n'| ../reversi "
      "board_parser/board-eof_after_current_player.fail",
      1, NULL, "./reversi board_parser/board-eof_after_current_player.fail" },
    { "echo -e 'Q\n\n'| ../reversi "
      "board_parser/board-eof_before_end_of_the_board.fail",
      1, NULL,
      "./reversi board_parser/board-eof_before_end_of_the_board.fail" },
    { "echo -e 'Q\n\n'| ../reversi board_parser/board-extra_empty_lines.pass",
      0, NULL, "./reversi board_parser/board-extra_empty_lines.pass" },
    { "echo -e 'Q\n\n'| ../reversi "
      "board_parser/board-extra_spaces_around_chars.pass",
      0, NULL, "./reversi board_parser/board-extra_spaces_around_chars.pass" },
    { "echo -e 'Q\n\n'| ../reversi board_parser/board-only_one_input.txt", 0,
      NULL, "./reversi board_parser/board-only_one_input.txt" },
    { "echo -e 'Q\n\n'| ../reversi "
      "board_parser/board-stop_at_first_line_without_newline.fail",
      1, NULL,
      "./reversi board_parser/board-stop_at_first_line_without_newline.fail" },
    { "echo -e 'Q\n\n'| ../reversi board_parser/board-too_few_lines.fail", 1,
      NULL, "./reversi board_parser/board-too_few_lines.fail" },
    { "echo -e 'Q\n\n'| ../reversi "
      "board_parser/board-too_few_lines_with_comment.fail",
      1, NULL,
      "./reversi board_parser/board-too_few_lines_with_comment.fail" },
    { "echo -e 'Q\n\n'| ../reversi board_parser/board-too_many_lines.fail", 1,
      NULL, "./reversi board_parser/board-too_many_lines.fail" },
    { "echo -e 'Q\n\n'| ../reversi board_parser/board-with_comments.pass", 0,
      NULL, "./reversi board_parser/board-with_comments.pass" },
    { "echo -e 'Q\n\n'| ../reversi board_parser/board-wrong_character.fail", 1,
      NULL, "./reversi board_parser/board-wrong_character.fail" },
    { "echo -e 'Q\n\n'| ../reversi "
      "board_parser/board-wrong_current_player_char.fail",
      1, NULL, "./reversi board_parser/board-wrong_current_player_char.fail" },
    { "../reversi", 0, NULL, "./reversi" },
    { "../reversi -x", 1, "Option invalide", "./reversi -x" },
    { "../reversi -v", 0, NULL, "./reversi -v" },
    { "../reversi --verbose", 0, NULL, "./reversi --verbose" },
    { "../reversi -V", 0, NULL, "./reversi -V" },
    { "../reversi --version", 0, NULL, "./reversi --version" },
    { "../reversi -h", 0, NULL, "./reversi -h" },
    { "../reversi --help", 0, NULL, "./reversi --help" },
    { "../reversi --he", 0, NULL, "./reversi --he" },
    { "echo -e 'Q\n\n'| ../reversi -b", 0, NULL, "./reversi -b" },
    { "echo -e 'Q\n\n'| ../reversi --black-ai", 0, NULL,
      "./reversi --black-ai" },
    { "echo -e 'Q\n\n'| ../reversi -b0", 0, NULL, "./reversi -b0 " },
    { "echo -e 'Q\n\n'| ../reversi --black-ai=0", 0, NULL,
      "./reversi --black-ai=0" },
    { "echo -e 'Q\n\n'| ../reversi -b1", 0, NULL, "./reversi  -b1" },
    { "echo -e 'Q\n\n'| ../reversi --black-ai=1", 0, NULL,
      "./reversi --black-ai=1 " },
    { "echo -e 'Q\n\n'| ../reversi -b10", 1, NULL, "./reversi with -b10 " },
    { "echo -e 'Q\n\n'| ../reversi --black-ai=10", 1, NULL,
      "./reversi --black-ai=10 " },
    { "echo -e 'Q\n\n'| ../reversi -w", 0, NULL, "./reversi -w" },
    { "echo -e 'Q\n\n'| ../reversi --white-ai", 0, NULL,
      "./reversi --white-ai " },
    { "echo -e 'Q\n\n'| ../reversi --white-ai=0", 0, NULL,
      "./reversi --white-ai=0" },
    { "echo -e 'Q\n\n'| ../reversi -w1", 0, NULL, "./reversi -w1 " },
    { "echo -e 'Q\n\n'| ../reversi --white-ai=1", 0, NULL,
      "./reversi --white-ai=1" },
    { "echo -e 'Q\n\n'| ../reversi -w10", 1, NULL, "./reversi -w10" },
    { "echo -e 'Q\n\n'| ../reversi --white-ai=10", 1, NULL,
      "./reversi --white-ai=10 " },
    { "echo -e 'Q\n\n'| ../reversi -s 2 -b -w", 0, NULL,
      "./reversi -s 2 -b -w " },
    { "echo -e 'Q\n\n'| ../reversi --size 2 --black-ai --white-ai", 0, NULL,
      "./reversi --size 2 --black-ai --white-ai " },
    { "echo -e 'Q\n\n'| ../reversi -b0 -w1", 0, NULL, "./reversi -b0 -w1" },
    { "echo -e 'Q\n\n'| ../reversi --black-ai=0 --white-ai=1", 0, NULL,
      "./reversi  --black-ai=0 --white-ai=1" },
    { "echo -e 'Q\n\n'| ../reversi --size", 1, NULL, "./reversi --size" },
    { "echo -e 'Q\n\n'| ../reversi -s 1", 0, NULL, "./reversi -s 1" },
    { "echo -e 'Q\n\n'| ../reversi -s 2", 0, NULL, "./reversi -s 2" },
    { "echo -e 'Q\n\n'| ../reversi -s 3", 0, NULL, "./reversi -s 3" },
    { "echo -e 'Q\n\n'| ../reversi -s 4", 0, NULL, "./reversi -s 4" },
    { "echo -e 'Q\n\n'| ../reversi -s 5", 0, NULL, "./reversi -s 5" },
    { "echo -e 'Q\n\n'| ../reversi -s 6", 1, NULL, "./reversi -s 6" },
    { "echo -e 'Q\n\n'| ../reversi -s 0", 1, NULL, "./reversi -s 0" },
    { "echo -e 'Q\n\n'| ../reversi --size 0", 1, NULL, "./reversi --size 0" },
    { "echo -e 'Q\n\n'| ../reversi --size 1", 0, NULL, "./reversi --size 1" },
    { "echo -e 'Q\n\n'| ../reversi --size 2", 0, NULL, "./reversi --size 2" },
    { "echo -e 'Q\n\n'| ../reversi --size 3", 0, NULL, "./reversi --size 3" },
    { "echo -e 'Q\n\n'| ../reversi --size 4", 0, NULL, "./reversi --size 4" },
    { "echo -e 'Q\n\n'| ../reversi --size 5", 0, NULL, "./reversi --size 5" },
    { "echo -e 'Q\n\n'| ../reversi --size 6", 1, NULL, "./reversi --size 6" },
    { "echo -e 'Q\n\n'| ../reversi -b0 -s 1", 0, NULL, "./reversi -b0 -s 1" },
    { "echo -e 'Q\n\n'| ../reversi --black-ai=0 --size 1", 0, NULL,
      "./reversi --black-ai=0 --size 1" },
    { "echo -e 'Q\n\n'| ../reversi -w0 -s 1", 0, NULL, "./reversi -w0 -s 1" },
    { "echo -e 'Q\n\n'| ../reversi --white-ai=0 --size 1", 0, NULL,
      "./reversi --white-ai=0 --size 1" },
    { "echo -e 'Q\n\n'| ../reversi board_parser/board-6x6.pass", 0, NULL,
      "./reversi board_parser/board-6x6.pass" },
    { "echo -e 'Q\n\n'| ../reversi -v board_parser/board-6x6.pass", 0, NULL,
      "./reversi -v board_parser/board-6x6.pass" },
    { "echo -e 'Q\n\n'| ../reversi --verbose board_parser/board-6x6.pass", 0,
      NULL, "./reversi --verbose board_parser/board-6x6.pass" },
    { "echo -e 'Q\n\n'| ../reversi -c board_parser/board-6x6.pass", 0, NULL,
      "./reversi -c board_parser/board-6x6.pass" },
    { "echo -e 'Q\n\n'| ../reversi --contest board_parser/board-6x6.pass", 0,
      NULL, "./reversi --contest board_parser/board-6x6.pass" },
    { "echo -e 'Q\n\n'| ../reversi -c missing.txt", 1, NULL,
      "./reversi -c missing.txt" },
    { "echo -e 'Q\n\n'| ../reversi --contest missing.txt", 1, NULL,
      "./reversi --contest missing.txt" },
    { "echo -e 'Q\n\n'| ../reversi -c -v board_parser/board-6x6.pass", 0, NULL,
      "./reversi -c -v board_parser/board-6x6.pass" },
    { "echo -e 'Q\n\n'| ../reversi --contest --verbose "
      "board_parser/board-6x6.pass",
      0, NULL, "./reversi --contest --verbose board_parser/board-6x6.pass" },
    { "echo -e 'q\n\n'| ../reversi", 0, NULL, "./reversi q\\n\\n" },
    { "echo -e 'Q\n\n'| ../reversi", 0, NULL, "./reversi Q\\n\\n" },
    { "echo -e 'q\n\n'| ../reversi", 0, NULL, "./reversi q\\n\\n" },
    { "echo -e 'Q\ny\n\n'| ../reversi", 0, NULL, "./reversi Q\\ny\\n\\n" },
    { "echo -e 'Q\ny\n\n board_parser/board-6x6.pass'| ../reversi", 0, NULL,
      "./reversi Q\\ny\\n\\n board_parser/board-6x6.pass" },
    { "echo -e 'Q\nY\n\n'| ../reversi", 0, NULL, "./reversi Q\\nY\\n\\n-" },
    { "echo -e 'Q\nY\n\n'| ../reversi -s 1", 0, NULL,
      "./reversi -s1 Q\\nY\\n\\n" },
    { "echo -e 'Q\nY\n\n'| ../reversi -s 2", 0, NULL,
      "./reversi -s2 Q\\nY\\n\\n" },
    { "echo -e 'Q\nY\n\n'| ../reversi -s 3", 0, NULL,
      "./reversi -s3 Q\\nY\\n\\n" },
    { "echo -e 'Q\nY\n\n'| ../reversi -s 4", 0, NULL,
      "./reversi -s4 Q\\nY\\n\\n" },
    { "echo -e 'Q\nY\n\n'| ../reversi -s 5", 0, NULL,
      "./reversi -s5 Q\\nY\\n\\n" },
    { "echo -e 'd0\nq\ny\naaaa\n'| ../reversi", 0, NULL,
      "./reversi d0\\nq\\ny\\naaaa\\n" },
    { "echo -e 'd0\nq\ny\n\n'| ../reversi", 0, NULL,
      "../reversi d0\\nq\\ny\\n\\n" },
    { "echo -e 'aaaaaaaaaaaaaaaaaaaaa\nq\ny\n\n'| ../reversi", 0, NULL,
      "../reversi aaaaaaaaaaaaaaaaaaaaa\\nq\\ny\\n\\n" },
    { "echo -e '    a    a   \nq\ny\n\n'| ../reversi", 0, NULL,
      "./reversi     a    a   \\nq\\ny\\n\\n" },
    { "echo -e '0d\nq\ny\n\n'| ../reversi", 0, NULL,
      "./reversi 0d\\nq\\ny\\n\\n" },
    { "echo -e '00\nq\ny\n\n'| ../reversi", 0, NULL,
      "./reversi 00\\nq\\ny\\n\\n" },
    { "echo -e '  0   0    \nq\ny\n\n'| ../reversi", 0, NULL,
      "./reversi   0   0    \\nq\\ny\\n\\n" },
    { "echo -e 'Q\n\n'| ../reversi -b1", 0, NULL, "./reversi -b1" },
    { "echo -e 'Q\n\n'| ../reversi -w1", 0, NULL, "./reversi -w1" },
    { "echo -e 'Q\n\n'| ../reversi -b2 ", 0, NULL, "./reversi -b2" },
    { "echo -e 'Q\n\n'| ../reversi -w2 ", 0, NULL, "./reversi -w2" },
    { "echo -e '      d      2\ne  2  nq\ny\n\n'| ../reversi", 0, NULL,
      "./reversi      d      2\\ne  2  nq\\ny\\n\\n" },
    { "echo -e 'c2\b2\\u\\r\\nq\\nY\\n\\n'| ../reversi -s 3", 0, NULL,
      "./reversi -s3  undo_redo tests" },
    { "echo -e 'f5\nf4\nu\nQ\nY\n\n'| ../reversi", 0, NULL,
      " ./reversi test undo " },
    { "echo -e 'f5\nf4\nu\nr\nQ\nY\n\n' | ../reversi", 0, NULL,
      " ./reversi test redo " },
    { "echo -e 'f5\nf4\nh\nQ\nY\n\n' | ../reversi", 0, NULL,
      " ./reversi test help player" },
    { "valgrind echo -e 'f5\nf4\nh\nQ\nY\n\n' | ../reversi", 0, NULL,
      "valgrind ./reversi test help player" },
    { "valgrind echo -e 'f5\nf4\nu\nQ\nY\n\n'| ../reversi", 0, NULL,
      "valgrind ./reversi test undo " },
    { "valgrind echo -e 'f5\nf4\nu\nr\nQ\nY\n\n' | ../reversi", 0, NULL,
      "valgrind ./reversi test redo " },
    { "valgrind echo -e 'c2\b2\\u\\r\nq\nY\n\n'| ../reversi -s 3", 0, NULL,
      "./reversi -s3  undo_redo tests" },
    { "valgrind ../reversi -h", 0, NULL, "valgrind ./reversi -h" },
    { "valgrind ../reversi --help", 0, NULL, "valgrind ./reversi --help" },
    { "valgrind ../reversi -V", 0, NULL, "valgrind ./reversi -V" },
    { "valgrind ../reversi --version", 0, NULL,
      "valgrind ./reversi --version" },
    { "valgrind ../reversi", 0, NULL,
      "valgrind ./reversi board-with_comments.pass" },
    { "valgrind ../reversi -c board_parser/board-8x8.pass", 0, NULL,
      "./reversi -c board-8x8.pass" },
    { "valgrind ../reversi -c board_parser/board-8x8.pass", 0, NULL,
      "./reversi -c board-8x8.pass" },
    { "valgrind ../reversi -c board_parser/board-only_one_input.txt", 0, NULL,
      "./reversi -c board-only_one_input.txt" },
    { "valgrind echo -e 'Q\n\n'| ../reversi -b1 -w1", 0, NULL,
      "valgrind ./reversi -b1 -w1" },
    { "valgrind echo -e 'Q\n\n'| ../reversi -b2 -w2", 0, NULL,
      "valgrind ./reversi -b2 -w2" },
    { "valgrind echo -e 'Q\n\n'| ../reversi -b1 -w2", 0, NULL,
      "valgrind ./reversi -b1 -w2" },
    { "valgrind echo -e 'Q\n\n'| ../reversi -t 1", 0, NULL,
      "valgrind ./reversi -t 1" },
  };

  int num_tests = sizeof (tests) / sizeof (tests[0]);
  int passed = 0;
  pid = fork ();
  if (!pid)
    {
      for (int i = 0; i < num_tests; i++)
        {
          passed += run_test (tests[i].command, tests[i].expected_exit_code,
                              tests[i].expected_stderr, tests[i].description);
        }
    }

  remove ("stdout.txt");
  remove ("stderr.txt");
  printf ("\nTests success : %d/%d\n", passed, num_tests);
  return EXIT_SUCCESS;
}
