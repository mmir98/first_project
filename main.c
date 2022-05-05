#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>

#define FIFO_PATH "fifo"
#define RVP_FAILED "rvp_f"
#define RVP_SUCCEED "rvp_s"
#define CVP_FAILED "cvp_f"
#define CVP_SUCCEED "cvp_s"
#define SVP_FAILED "svp_f"
#define SVP_SUCCEED "svp_s"

bool format_input(char *, char *, int *, int *);
bool row_validation(int width, int height, char[width][height]);
bool column_validation(int width, int height, char[width][height]);
bool subRect_validation(int width, int height, char[width][height], int, int);
char decryptor(char);
void decryptor_process(int width, int height, char[width][height]);
void row_validator_process(int width, int height, char[width][height]);
void column_validator_process(int width, int height, char[width][height]);
void subRect_validator_process(int width, int height, char[width][height], int, int);

int main(int argc, char const *argv[])
{
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

    while (st_ptr != NULL && flag <= nums + 1)
    {
        if (nums == 2)
        {
            /* Main board is a square */
            if (flag == 1)
            {
                // board dimensions
                board_width = atoi(st_ptr);
                board_height = board_width;
            }
            if (flag == 2)
            {
                // sub-board width
                subrects_width = atoi(st_ptr);
            }
            if (flag == 3)
            {
                // sub-board height
                subrects_height = atoi(st_ptr);
            }
        }
        else if (nums == 3)
        {
            /* Main board is a Rect */
            if (flag == 1)
            {
                // main board width
                board_width = atoi(st_ptr);
            }
            if (flag == 2)
            {
                // main board height
                board_height = atoi(st_ptr);
            }
            if (flag == 3)
            {
                // sub-board width
                subrects_width = atoi(st_ptr);
            }
            if (flag == 4)
            {
                // sub-board height
                subrects_height = atoi(st_ptr);
            }
        }

        // printf("%s\n", st_ptr);
        st_ptr = strtok(NULL, num_delim);
        flag += 1;
    }

    char mainBoard[board_width][board_height];
    buffer_size = board_width + 1;
    char row_input[buffer_size];
    for (int i = 0; i < board_height; i++)
    {
        /* Reading rows from input file  */
        count = fread(&row_input, sizeof(char), buffer_size, input_fp);
        for (int j = 0; j < board_width; j++)
        {
            /* Filling mainBoard array */
            mainBoard[j][i] = row_input[j];
        }
    }
    fclose(input_fp);

    // Creating pipe
    int fifo = mkfifo(FIFO_PATH, 0666);
    if (fifo == -1)
    {
        printf("Creating fifo failed!\n");
        return 0;
    }

    // Creating child-processes
    int dp = fork();
    if (dp < 0)
    {
        /* error */
        printf("Can't create decryptor process\n");
        return 0;
    }
    if (dp == 0)
    {
        /* decryptor process */
        decryptor_process(board_width, board_height, mainBoard);
        exit(0);
    }
    /* parent */
    int fd = open(FIFO_PATH, O_RDONLY);
    char *dec_buff;
    char decrypted_board[board_width][board_height];
    for (int i = 0; i < board_height; i++)
    {
        for (int j = 0; j < board_width; j++)
        { // sleep(1);
            // printf("fd in reader %d\n", fd);
            read(fd, dec_buff, sizeof(char));
            decrypted_board[j][i] = *dec_buff;
            // printf("parent reads ::: %c\n", *dec_buff);
        }
    }
    close(fd);

    int rvp = fork();
    int cvp = fork();
    int svp = fork();
    if (rvp < 0 && cvp < 0 && svp < 0)
    {
        /* forking validators failed */
        printf("Forking validators failed!");
        return 0;
    }
    if (rvp == 0 && cvp != 0 && svp != 0)
    {
        /* Row validator process */
        row_validator_process(board_width, board_height, decrypted_board);
        exit(0);
    }
    if (rvp != 0 && cvp == 0 && svp != 0)
    {
        /* Column validator process */
        column_validator_process(board_width, board_height, decrypted_board);
        exit(0);
    }
    if (rvp != 0 && cvp != 0 && svp == 0)
    {
        /* SubRect validator process */
        subRect_validator_process(board_width, board_height, decrypted_board, subrects_width, subrects_height);
        exit(0);
    }
    if (rvp > 0 && cvp > 0 && svp > 0)
    {
        /* Parent */
        int fd = open(FIFO_PATH, O_RDONLY);
        if (fd == -1)
        {
            /* parent can't read fd */
            printf("Parent can't open FIFO to read!");
            return 0;
        }
        char res_buffer[6];
        if (read(fd, res_buffer, sizeof(RVP_SUCCEED)) == -1)
        {
            /*  */
            printf("Parent can't read from FIFO");
            return 0;
        }
        printf("%s\n", res_buffer);
        sleep(1);
        if (read(fd, res_buffer, sizeof(RVP_SUCCEED)) == -1)
        {
            /*  */
            printf("Parent can't read from FIFO");
            return 0;
        }
        printf("%s\n", res_buffer);
        sleep(1);
        if (read(fd, res_buffer, sizeof(RVP_SUCCEED)) == -1)
        {
            /*  */
            printf("Parent can't read from FIFO");
            return 0;
        }
        printf("%s\n", res_buffer);
        close(fd);


        //! debug tests
        // TODO remove
        // input_fp = fopen("formatted_input.txt", "r");
        // char c;
        // while ((c = fgetc(input_fp)) != EOF)
        // {
        //     printf("%c", c);
        // }
        // printf("\n");
        // fclose(input_fp);
        // printf("%d : nums \n%d : buffer_size \n", nums, buffer_size);

        // for (int i = 0; i < count; i++)
        // {
        //     printf("%c", buffer[i]);
        // }

        // printf("\n");
        // printf("count :: %d\n", count);

        // printf("board width :: %d\n", board_width);
        // printf("board height :: %d\n", board_height);

        // printf("subrect  width :: %d\n", subrects_width);
        // printf("subrect  height :: %d\n", subrects_height);
        // printf("%d\n", ('Z' - 'A'));
        // printf("%d - %d\n", 'a', 'z');
        // printf("%d - %c\n", 'A' - 2, 'Z'-1);
        // printf("%c\n", 67);
        // for (int i = 0; i < board_height; i++)
        // {
        //     /* code */
        //     for (int j = 0; j < board_width; j++)
        //     {
        //         /* code */
        //         printf("%c ", mainBoard[j][i]);
        //     }
        //     printf("\n");
        // }
        // printf("\t\nDecrypted version!\n");
        // for (int i = 0; i < board_height; i++)
        // {
        //     /* code */
        //     for (int j = 0; j < board_width; j++)
        //     {
        //         /* code */
        //         printf("%c ", decrypted_board[j][i]);
        //     }
        //     printf("\n");
        // }
    }
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
    for (int i = 0; i < height; i++)
    {
        int flag[26] = {0}; //! Either 0 or 1
        for (int j = 0; j < width; j++)
        {
            int index = board[j][i] - 'A';
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

    for (int i = 0; i < width; i++)
    {
        int flag[26] = {0}; //! Either 0 or 1
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
    return true;
}

//* Capitalise input_char and decrypt it.
char decryptor(char input_char)
{
    char res;

    // Capitalise input_char if it's not
    if (input_char >= 97 && input_char <= 122)
    {
        res = 65 + (input_char - 'a');
    }
    else
    {
        res = input_char;
    }

    // Decode
    res -= 2;
    if (res == 64)
    {
        res = 'Z';
    }
    if (res == 63)
    {
        res = 89;
    }

    return res;
}

//* Child Processes
//* Uses decryptor func to decrypt board parameter
void decryptor_process(int width, int height, char board[width][height])
{
    int fd = open(FIFO_PATH, O_WRONLY);
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            char w_buff = decryptor(board[j][i]);
            // printf("Dec writes ::: %c\n", w_buff);
            write(fd, &w_buff, sizeof(char));
        }
    }
    close(fd);
}

//* Uses row_validator func to validate rows within the board parameter.
//* If valid sends [RVP_SUCCEED] msg to parent.
//* If invalid sends [RVP_FAILED] msg to parent.
void row_validator_process(int width, int height, char board[width][height])
{
    if (row_validation(width, height, board) == false)
    {
        /* Validation failed */
        int fd = open(FIFO_PATH, O_WRONLY);
        if (fd == -1)
        {
            /* Openning fifo fd failed */
            printf("Opening FIFO fd failed in RVP.");
            return;
        }
        if (write(fd, RVP_FAILED, sizeof(RVP_FAILED)) == -1)
        {
            printf("Can't write to FIFO in RVP.");
            return;
        }
        close(fd);
        return;
    }
    /* Validation succeed */
    int fd = open(FIFO_PATH, O_WRONLY);
    if (fd == -1)
    {
        /* Openning fifo fd failed */
        printf("Opening FIFO fd failed in RVP.");
        return;
    }
    if (write(fd, RVP_SUCCEED, sizeof(RVP_SUCCEED)) == -1)
    {
        printf("Can't write to FIFO in RVP.");
        return;
    }
    close(fd);
}

//* Uses column_validator func to validate columns within the board parameter.
//* If valid sends [CVP_SUCCEED] msg to parent.
//* If invalid sends [CVP_FAILED] msg to parent.
void column_validator_process(int width, int height, char board[width][height])
{
    if (column_validation(width, height, board) == false)
    {
        /* Validation failed */
        int fd = open(FIFO_PATH, O_WRONLY);
        if (fd == -1)
        {
            /* Openning fifo fd failed */
            printf("Opening FIFO fd failed in CVP.");
            return;
        }
        if (write(fd, CVP_FAILED, sizeof(CVP_FAILED)) == -1)
        {
            printf("Can't write to FIFO in CVP.");
            return;
        }
        close(fd);
        return;
    }
    /* Validation succeed */
    int fd = open(FIFO_PATH, O_WRONLY);
    if (fd == -1)
    {
        /* Openning fifo fd failed */
        printf("Opening FIFO fd failed in CVP.");
        return;
    }
    if (write(fd, CVP_SUCCEED, sizeof(CVP_SUCCEED)) == -1)
    {
        printf("Can't write to FIFO in CVP.");
        return;
    }
    close(fd);
}

//* Uses subRect_validator func to validate subRects within the board parameter.
//* If valid sends [SVP_SUCCEED] msg to parent.
//* If invalid sends [SVP_FAILED] msg to parent.
void subRect_validator_process(int width, int height, char board[width][height], int subWidth, int subHeight)
{
    if (subRect_validation(width, height, board, subWidth, subHeight) == false)
    {
        /* Validation failed */
        int fd = open(FIFO_PATH, O_WRONLY);
        if (fd == -1)
        {
            /* Openning fifo fd failed */
            printf("Opening FIFO fd failed in SVP.");
            return;
        }
        if (write(fd, SVP_FAILED, sizeof(SVP_FAILED)) == -1)
        {
            printf("Can't write to FIFO in SVP.");
            return;
        }
        close(fd);
        return;
    }
    /* Validation succeed */
    int fd = open(FIFO_PATH, O_WRONLY);
    if (fd == -1)
    {
        /* Openning fifo fd failed */
        printf("Opening FIFO fd failed in SVP.");
        return;
    }
    if (write(fd, SVP_SUCCEED, sizeof(SVP_SUCCEED)) == -1)
    {
        printf("Can't write to FIFO in SVP.");
        return;
    }
    close(fd);
}
