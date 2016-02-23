#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

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


int main (int argc, char* argv[])
{
size_t i = 0;
int is_prev_char_digit = 0, is_printed = 0;
size_t idx_of_first_digit = 0, numb_of_digits = 0;
int sum = 0;

char prev_char = 0, cur_char = 0, saved_char = 0;
char* input_str = NULL;


//------------------------------------------------------------------------------
// Help info + getting input string
//------------------------------------------------------------------------------
if (argc != 2)
        {
        printf ("\n");
        printf ("This program scans an input string and replaces all numbers "
                                "with the strings, according to the following rule:\n");

        printf ("\t* if a number is divisible by 3       -> \"bizz\"\n");
        printf ("\t* if a number is divisible by 5       -> \"buzz\"\n");
        printf ("\t* if a number is divisible by 3 and 5 -> \"bizzbuzz\"\n");
        printf ("\t* if a number is neither divisible by 3 nor 5, it is not changed\n");

        printf ("All the remaining string is not changed.\n");

        printf ("\n");
        printf ("Usage: %s INPUT_STRING\n", argv[0]);
        printf ("\n");
        exit (EXIT_FAILURE);
        }

input_str = argv[1];
//------------------------------------------------------------------------------
// Processing of input string
//------------------------------------------------------------------------------
// in order to correctly process digit if it is the last symbol in a string
i = -1; // http://stackoverflow.com/questions/15710072/how-to-detect-negative-number-assigned-to-size-t
do
        {
        i++;
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

                sum += (cur_char - '0');
                while (sum >= 3)
                        sum -= 3;
                }

        prev_char = cur_char;
        }
        while (input_str[i] != '\0');
//------------------------------------------------------------------------------


printf ("\n");

return 0;
}
