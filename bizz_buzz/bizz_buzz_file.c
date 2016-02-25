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


int printf_substring (char* string, size_t idx_of_first_char, size_t length_of_substring);
int printf_numb_on_edge (int fd_for_numb_on_edge, char* buffer_for_numb_on_edge,
                                        size_t numb_of_digits, size_t idx_of_first_digit);


int main (int argc, char* argv[])
{
size_t i = 0;
int is_prev_char_digit = 0, is_printed = 0;
size_t idx_of_first_digit = 0, numb_of_digits = 0;
int sum = 0;

char prev_char = 0, cur_char = 0;
char* input_str = NULL;

int fd = 0, fd_for_numb_on_edge = 0;
int numb_of_read_bytes = 0;
char buffer[SIZE_OF_BUF] = {0}, buffer_for_numb_on_edge[SIZE_OF_BUF] = {0};
int is_copied = 0;
off_t cur_position = 0;

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

// all code corresponding to '_numb_on_edge' provide solution for the problem,
//      when a number gets divided by the edge of the buffers
if ((fd_for_numb_on_edge = open (argv[1], O_RDONLY)) == -1)
        HANDLE_ERROR("fd_for_numb_on_edge open");
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
                                        // makes warning about '.*': requires int, but 'numb_of_digits' is size_t
                                        // printf ("%.*s", numb_of_digits, &input_str[idx_of_first_digit]);

                                        if (is_copied == 1) // the number is lying on edge of buffers
                                                {
                                                printf_numb_on_edge (fd_for_numb_on_edge, buffer_for_numb_on_edge,
                                                                                        numb_of_digits, idx_of_first_digit);
                                                }
                                        else
                                                if ((printf_substring (input_str, idx_of_first_digit, numb_of_digits)) < 0)
                                                        HANDLE_ERROR("printf_substring");
                                        }

                                numb_of_digits = 0;
                                idx_of_first_digit = 0;
                                is_prev_char_digit = 0;
                                is_printed = 0;
                                sum = 0;

                                is_copied = 0;
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
                //------------------------------------------------------------------------------
                // this is after 'for' => current substring(buffer) is ended
                //------------------------------------------------------------------------------
                // if this is true => a number started in current buffer and this buffer ended
                //      before (or simultaneously with) the end of this number
                //-----
                // we will copy the current buffer and also we know the 'idx_of_first_digit' and
                //      'numb_of_digits' => later, if this number (that lies on the edge of buffers)
                //      is not divisible neither by 3 nor 5 => we will print 'numb_of_digits' chars
                //      from 'idx_of_first_digit' (that refers to the copied buffer) and dynamically
                //      reads all necessary chars from 'fd_for_numb_on_edge'
                if ((is_prev_char_digit == 1) & (is_copied == 0))
                        {
                        if (memcpy (buffer_for_numb_on_edge, buffer, numb_of_read_bytes + 1) == NULL)
                                HANDLE_ERROR("memcpy");

                        if ((cur_position = lseek (fd, 0, SEEK_CUR)) == (off_t)(-1))
                                HANDLE_ERROR("lseek");
                        if (lseek (fd_for_numb_on_edge, cur_position, SEEK_SET) == (off_t)(-1))
                                HANDLE_ERROR("lseek");

                        is_copied = 1;
                        }
}
//------------------------------------------------------------------------------

if (close (fd))
        HANDLE_ERROR("file close");

return 0;
}


int printf_substring (char* string, size_t idx_of_first_char, size_t length_of_substring)
{
char saved_char = 0;
int numb_of_chars_printed = 0;

if ((string == NULL) || (idx_of_first_char < 0) || (length_of_substring < 0))
        {
        printf ("Arguments of %s is invalid!\n", __func__);
        exit (EXIT_FAILURE);
        }

saved_char = string[idx_of_first_char + length_of_substring];
string[idx_of_first_char + length_of_substring] = '\0';

numb_of_chars_printed = printf ("%s", &string[idx_of_first_char]);

string[idx_of_first_char + length_of_substring] = saved_char;

return numb_of_chars_printed;
}


int printf_numb_on_edge (int fd_for_numb_on_edge, char* buffer_for_numb_on_edge,
                                        size_t numb_of_digits, size_t idx_of_first_digit)
{
int numb_of_read_bytes = 0, numb_of_printed_digits = 0;

if ((fd_for_numb_on_edge < 0) || (buffer_for_numb_on_edge == NULL) ||
                        (numb_of_digits < 0) || (idx_of_first_digit < 0))
        {
        printf ("Arguments of %s is invalid!\n", __func__);
        exit (EXIT_FAILURE);
        }

if ((numb_of_printed_digits = printf ("%s", &buffer_for_numb_on_edge[idx_of_first_digit])) < 0)
        HANDLE_ERROR("first printf_on_edge");
numb_of_digits -= numb_of_printed_digits;

while (numb_of_digits > 0)
        {
        if ((numb_of_read_bytes = read (fd_for_numb_on_edge,
                                buffer_for_numb_on_edge, SIZE_OF_BUF - 1)) == -1)
                HANDLE_ERROR("read");
        buffer_for_numb_on_edge[numb_of_read_bytes] = '\0';

        if (numb_of_digits > numb_of_read_bytes)
                {
                printf ("%s", buffer_for_numb_on_edge);

                numb_of_digits -= numb_of_read_bytes;
                continue;
                }
        else
                {
                if ((numb_of_printed_digits = printf_substring (buffer_for_numb_on_edge, 0, numb_of_digits)) < 0)
                        HANDLE_ERROR("printf_substring on edge");

                numb_of_digits -= numb_of_printed_digits;
                }
        }

return 0;
}

