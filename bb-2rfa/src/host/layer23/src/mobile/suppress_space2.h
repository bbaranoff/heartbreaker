#include <stdio.h>
char res2[100];
char* spaces2(char str [])
{
int i = 0;int j = 0;
       while (str[i] != '\0')
       {
          if ((str[i] == ' ') != 1) {
            res[j] = str[i];
            j++;
          }
          i++;
       }
       res2[j] = '\0';
return res2;}

