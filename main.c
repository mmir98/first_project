#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>

bool format_input(char *, char *, int *, int *);
bool row_validation(int width, int height, char[width][height]);
bool column_validation(int width, int height, char[width][height]);
bool subRect_validation(int width, int height, char[width][height], int, int);

int main(int argc, char const *argv[])
{
    // int width, height;
    // char main_board[width][height];
    // int sub_width, sub_height;

    int nums = 0, buffer_size = 0;
    char *input_fn = "input.txt"; //! Enter input file's name here
    char *output_fn = "formatted_input.txt";
    if (format_input(input_fn, output_fn, &nums, &buffer_size) == false)
    {
        printf("Can't format input");
        return 0;
    }

    int board_width, board_height;
    int subrects_width, subrects_height;

    FILE *input_fp = fopen("formatted_input.txt", "r");
    char buffer[buffer_size];
    int count = fread(&buffer, sizeof(char), buffer_size, input_fp);
    char num_delim[] = "*";
    char *st_ptr = strtok(buffer, num_delim);
    int flag = 1;

    while (st_ptr != NULL && flag <= nums)
    {
        if (flag == 1)
        {
            // board width
            // printf("width :: %d\n", atoi(st_ptr));
            board_width = atoi(st_ptr);
        }
        if (flag == 2)
        {
            // board height
            // printf("height :: %d\n", atoi(st_ptr));
            board_height = atoi(st_ptr);
        }
        if (flag > 2 && flag <= 2 + (nums - 2) / 2)
        {
            // sub-rect width
            // printf("sub-rect width :: %d\n", atoi(st_ptr));
            subrects_width = atoi(st_ptr);
        }
        if (flag > 2 + (nums - 2) / 2 && flag <= nums)
        {
            // sub-rect height
            // printf("sub-rect height  :: %d\n", atoi(st_ptr));
            subrects_height = atoi(st_ptr);
        }
        // printf("%s\n", st_ptr);
        st_ptr = strtok(NULL, num_delim);
        flag += 1;
    }

    char board[board_width][board_height];
    buffer_size = board_width + 1;
    int count2 = fread(&buffer, sizeof(char), buffer_size, input_fp);
    printf("%s\n", buffer);

    fclose(input_fp);

    //! tests
    // TODO remove
    input_fp = fopen("formatted_input.txt", "r");
    char c;
    while ((c = fgetc(input_fp)) != EOF)
    {
        printf("%c", c);
    }
    printf("\n");
    fclose(input_fp);
    printf("%d : nums \n %d : buffer_size \n", nums, buffer_size);

    for (int i = 0; i < count; i++)
    {
        printf("%c", buffer[i]);
    }

    printf("\n");
    printf("count :: %d\n%ld\n", count, sizeof(int));

    printf("board width :: %d\n", board_width);
    printf("board height :: %d\n", board_height);
    for (int i = 0; i < sizeof(subrects_width) / sizeof(int); i++)
    {
        printf("subrect %d width :: %d\n", i, subrects_width);
        printf("subrect %d height :: %d\n", i, subrects_height);
    }

    printf("%d\n", ('Z' - 'A'));

    return 0;
}

//* Used to remove spaces and newline chars from input file
bool format_input(char *input_fn, char *output_fn, int *nums, int *buffer_size)
{
    FILE *input_fp = fopen(input_fn, "r");
    FILE *formatted_input_fp = fopen(output_fn, "w");
    if (NULL == input_fp)
    {
        printf("Can't open input file!");
        return false;
    }
    if (NULL == formatted_input_fp)
    {
        printf("Can't open formatted file");
        return false;
    }

    char c;
    while ((c = fgetc(input_fp)) != EOF)
    {
        if (c == ' ' || c == 9 || c == 10 || c == 13)
        {
            continue;
        }
        if (c == '*')
        {
            *nums += 1;
            *buffer_size += 1;
        }
        if (c >= '0' && c <= '9')
        {
            *buffer_size += 1;
        }

        fputc(c, formatted_input_fp);
    }

    fclose(input_fp);
    fclose(formatted_input_fp);
    return true;
}

//* Validates all rows within the given board. return true if validation succeeds and false o.w.
bool row_validation(int width, int height, char board[width][height])
{
    int flag[26]; //! Either 0 or 1
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            int index = board[j][i] - 'a';
            if (flag[index] == 1)
            {
                /* validation failed */
                return false;
            }
            else
            {
                flag[index] = 1;
            }
        }
    }
    /* validation succeed */
    return true;
}

//* Validate all columns within the given board. return true if validation succeeds and false o.w.
bool column_validation(int width, int height, char board[width][height])
{
    int flag[26]; //! Either 0 or 1
    for (int i = 0; i < width; i++)
    {
        for (int j = 0; j < height; j++)
        {
            int index = board[i][j] - 'A';
            if (flag[index] == 1)
            {
                /* validation failed */
                return false;
            }
            else
            {
                flag[index] = 1;
            }
        }
    }
    /* validation succeed */
    return true;
}

//* Validate all sub-boards within the given board. returns true if validation succeeds and false o.w.
bool subRect_validation(int width, int height, char board[width][height], int subWidth, int subHeight)
{
    char subBoard[subWidth][subHeight];
    int subRectCount = (width / subWidth) * (height / subHeight);

    for (int w = 0; w < (width / subWidth); w++)
    {
        for (int h = 0; h < (height / subHeight); h++)
        {
            // Partitioning main board
            for (int i = 0; i < subWidth; i++)
            {
                for (int j = 0; j < subHeight; j++)
                {
                    subBoard[i][j] = board[subWidth * w + i][subHeight * h + j];
                }
            }

            // validating sub-boards
            if (row_validation(subWidth, subHeight, subBoard) == false)
            {
                // validation failed
                return false;
            }
            if (column_validation(subWidth, subHeight, subBoard) == false)
            {
                // validation failed
                return false;
            }
        }
    }
}
