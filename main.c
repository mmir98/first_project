#include<stdio.h>
#include<stdbool.h>

bool format_input(char*, char*);


int main(int argc, char const *argv[])
{
    // int width, height;
    // char main_board[width][height];
    // int sub_width, sub_height;

    char *input_fn = "input.txt"; //! enter input file's name here
    char *output_fn = "formatted_input.txt";
    if (format_input(input_fn, output_fn) == false)
    {
        printf("Can't format input");
        return 0;
    }
    

    //! format_input func test
    // TODO remove
    FILE *input_fp = fopen("formatted_input.txt", "r");
    char c;
    while ((c = fgetc(input_fp)) != EOF)
    {
        printf("%c", c);
    }
    printf("\n");
    fclose(input_fp);
    
    return 0;
}


//* Used to remove spaces and newline chars from input file
bool format_input(char *input_fn, char *output_fn){
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
        if (c == ' ' || c == 10 || c == 13)
        {
            continue;
        }

        fputc(c, formatted_input_fp);
    }
    
    fclose(input_fp);
    fclose(formatted_input_fp);
    return true;
}