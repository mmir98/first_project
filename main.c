#include<stdio.h>
#include<stdbool.h>
#include<string.h>

bool format_input(char*, char*, int*, int*);


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

    int dimensions [nums];
    FILE *input_fp = fopen("formatted_input.txt", "r");
    char buffer [buffer_size];
    int count = fread(&buffer, sizeof(char), buffer_size, input_fp);
    
    char num_delim[] = "*";
    int flag = 1;
    char *st_ptr = strtok(buffer, num_delim);
    while (st_ptr != NULL || flag <= nums);
    {
        printf("%s\n", st_ptr);
        st_ptr = strtok(NULL, num_delim);
    }
    
    
    fclose(input_fp);


    //! format_input func test
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
    
    return 0;
}


//* Used to remove spaces and newline chars from input file
bool format_input(char *input_fn, char *output_fn, int *nums, int *buffer_size){
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