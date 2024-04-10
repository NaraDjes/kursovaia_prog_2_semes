#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <dirent.h>

#define ASCII_TABLE_SYMBOLS 256

int max(int a, int b)
{
    if (a > b)
        return a;
    else
        return b;
}

void stop_symbol_heuristic(char *pattern, int m, int StopSymbol[ASCII_TABLE_SYMBOLS])
{
    int i;
    int count = 1;
    for (i = 0; i < ASCII_TABLE_SYMBOLS; i++)
    {
        StopSymbol[i] = 0;
    }
    for (i = m - 2; i >= 0; i--)
    {
        if ((StopSymbol[(int)pattern[i]]) == 0)
        {
            StopSymbol[(int)pattern[i]] = count;
        }
        count++;
    }
    if ((StopSymbol[(int)pattern[m - 1]]) == 0)
    {
        StopSymbol[(int)pattern[m - 1]] = m - 1;
    }
}

void BM_MATCHER(char *text, char *pattern)
{
    int n = strlen(text);
    int m = strlen(pattern);
    int count = 0;
    int StopSymbol[ASCII_TABLE_SYMBOLS];
    int s = 0;

    stop_symbol_heuristic(pattern, m, StopSymbol);

    while (s <= n - m)
    {
        int j = m - 1;

        while (j >= 0 && pattern[j] == text[s + j])
        {
            j--;
        }

        if (j < 0)
        {
            printf("Pattern found by index %d\n", s);
            s++;
            count++;
        }
        else
        {
            int StopSymbolShift;
            if ((StopSymbol[(int)text[s + j]]) == 0)
            {
                StopSymbolShift = m - 1;
            }
            else
            {
                StopSymbolShift = StopSymbol[(int)text[s + j]];
            }
            s += StopSymbolShift;
        }
    }
    if (count == 0)
        printf("No patterns found\n");
}

int main(int argc, char *argv[])
{
    if (argc == 3)
    {
        char *string = argv[1];
        char *p = argv[2];
        char *p_new = strcat(p, "/");
        DIR *dp = opendir(p);

        if (!dp)
        {
            printf("Failed to open directory\n");
            return 1;
        }

        struct dirent *de;
        while ((de = readdir(dp)))
        {
            if ((strcmp(de->d_name, ".") != 0) && ((strcmp(de->d_name, "..")) != 0))
            {
                char str_temp[100];
                strcpy(str_temp, p_new);
                char *str_path = strcat(str_temp, de->d_name);
                printf("%s\n", de->d_name);
                FILE *sf = fopen(str_path, "r");

                if (!sf)
                {
                    printf("Failed to open file: %s\n", str_path);
                    continue;
                }

                fseek(sf, 0, SEEK_END);
                int end_file = ftell(sf);
                char *buf = malloc(end_file * sizeof(char));

                if (!buf)
                {
                    printf("Failed to allocate memory\n");
                    fclose(sf);
                    continue;
                }

                fseek(sf, 0, SEEK_SET);
                fread(buf, sizeof(char), end_file, sf);
                BM_MATCHER(buf, string);
                fclose(sf);
                free(buf);
            }
            else
            {
                continue;
            }
        }

        closedir(dp);
    }
    else
    {
        printf("Usage:\n./bmatcher \"<str>\" <pwd>\n");
    }
    return 0;
}
