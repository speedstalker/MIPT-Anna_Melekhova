#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include <errno.h>


//------------------------------------------------------------------------------
// Error handling macroses
//------------------------------------------------------------------------------
#define IS_DEBUG 1

#if (IS_DEBUG == 1)
//{
   #define HANDLE_ERROR HANDLE_ERROR_wL
   #define HANDLE_ERROR_wL(msg)                                                    \
                   do                                                              \
                   {                                                               \
                   char err_msg[256] = {0};                                        \
                                                                                   \
                   snprintf (err_msg, 255, "%d. " msg "%c", __LINE__, '\0');       \
                   perror (err_msg);                                               \
                   exit   (EXIT_FAILURE);                                          \
                   }                                                               \
                   while (0)
//}
#else
//{
   #define HANDLE_ERROR_wL HANDLE_ERROR
   #define HANDLE_ERROR(msg) \
                  do { perror(msg); exit(EXIT_FAILURE); } while (0)
//}
#endif
//------------------------------------------------------------------------------

#define SIZE_OF_BUF 1024


int main (int argc, char* argv[])
{
size_t i = 0;
int is_prev_char_digit = 0, is_printed = 0;
size_t idx_of_first_digit = 0, numb_of_digits = 0;
int sum = 0;

char prev_char = 0, cur_char = 0, saved_char = 0;
char* input_str = NULL;

int fd = 0;
int numb_of_read_bytes = 0;
char buffer[SIZE_OF_BUF] = {0};


//------------------------------------------------------------------------------
// Help info + getting input string
//------------------------------------------------------------------------------
if (argc != 2)
        {
        printf ("\n");
        printf ("This program scans an input file and replaces all numbers in it "
                                "with the strings, according to the following rule:\n");

        printf ("\t* if a number is divisible by 3       -> \"bizz\"\n");
        printf ("\t* if a number is divisible by 5       -> \"buzz\"\n");
        printf ("\t* if a number is divisible by 3 and 5 -> \"bizzbuzz\"\n");
        printf ("\t* if a number is neither divisible by 3 nor 5, it is not changed\n");

        printf ("All the remaining content of the file is not changed.\n");

        printf ("\n");
        printf ("Usage: %s FILE_NAME\n", argv[0]);
        printf ("\n");
        exit (EXIT_FAILURE);
        }
//------------------------------------------------------------------------------
if ((fd = open (argv[1], O_RDONLY)) == -1)
        HANDLE_ERROR("file open");
//------------------------------------------------------------------------------

numb_of_read_bytes = 1;
while (numb_of_read_bytes > 0)
{
        if ((numb_of_read_bytes = read (fd, buffer, SIZE_OF_BUF - 1)) == -1)
                HANDLE_ERROR("read");
        buffer[numb_of_read_bytes] = '\0';
        //------------------------------------------------------------------------------
        input_str = buffer;
        //------------------------------------------------------------------------------
        // Processing of input string
        //------------------------------------------------------------------------------
        for (i = 0; i < numb_of_read_bytes; i++)
                {
                cur_char = input_str[i];

                if (isdigit(cur_char) == 0)
                        {
                        if (is_prev_char_digit == 1)
                                {
                                if (sum == 0)
                                        {
                                        printf ("bizz");
                                        is_printed = 1;
                                        }
                                if ((prev_char == '0') || (prev_char == '5'))
                                        {
                                        printf ("buzz");
                                        is_printed = 1;
                                        }

                                if (is_printed == 0)
                                        {
                                        // makes warning about '.*' requires int, but 'numb_of_digits' is size_t
                                        // printf ("%.*s", numb_of_digits, &input_str[idx_of_first_digit]);
                                        saved_char = input_str[idx_of_first_digit + numb_of_digits];
                                        input_str[idx_of_first_digit + numb_of_digits] = '\0';

                                        printf ("%s", &input_str[idx_of_first_digit]);

                                        input_str[idx_of_first_digit + numb_of_digits] = saved_char;
                                        }

                                numb_of_digits = 0;
                                idx_of_first_digit = 0;
                                is_prev_char_digit = 0;
                                is_printed = 0;
                                sum = 0;
                                }

                        printf ("%c", cur_char);
                        }
                else // the 'cur_char' is a digit
                        {
                        numb_of_digits++;

                        if (is_prev_char_digit == 0)
                                {
                                idx_of_first_digit = i;
                                is_prev_char_digit = 1;
                                }

                        sum += cur_char;
                        while (sum >= 3)
                                sum -= 3;
                        }

                prev_char = cur_char;
                }
}
//------------------------------------------------------------------------------

if (close (fd))
        HANDLE_ERROR("file close");

return 0;
}
