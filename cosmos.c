#include<stdio.h>
#include <string.h>

/* Define your functions here. */

void PrintMenu(void){
   printf("MENU\nc - Number of non-whitespace characters\nw - Number of words\n");
   printf("f - Fix capitalization\nr - Replace all !'s\ns - Shorten spaces\nq - Quit\n");
   }

int GetNumOfNonWSCharacters(char text[300]){
   int white = 0;
   for (int i=0;i<strlen(text);i++){
      if(text[i] == ' '){
            white++;
         }
      }
   
   return strlen(text)-white;   
   }

char* ShortenSpace(char text[300]){
   int white = 0;
   int n=0;
   char res[300];
   int flag = 0;
   for (int i=0;i<strlen(text);i++){
      if(text[i] != ' '){
            res[n] = text[i];
            n++;
            flag = 0;
         }
      if (text[i] == ' ' && !flag){
            res[n] = text[i];
            flag = 1;
         }
      }
   
   return res;   
   }

char* FixCapitalization(char *text){
   
   for (int i=0;i<strlen(text);i++){
      if(((int)text[i] >= 65)&&((int)text[i] <= 90)){
         text[i] = text[i] + 32;
         }
      }
   return text;   
   }

int GetNumOfWords(char text[300]){
   int words = 0;
   char *res; 
   res = ShortenSpace(text);
   printf("%s",res);
   for (int i=0;i<strlen(res);i++){
      if (res[i] == ' '){
         words++;
         }
      }
   return words + 1;
   }

char* ReplaceExclamation(char *text){
   for (int i=0;i<strlen(text);i++){
      if (text[i] == '!'){
         text[i] = '.';
         }
      }
      
   return text;
   }


void ExecuteMenu(char choice, char text[300]){
   if (choice == 'c'){
      printf("Number of non-whitespace characters: %d\n", GetNumOfNonWSCharacters(text));
      return; 
      }
   if (choice == 'w'){
        printf("Number of words: %d\n", GetNumOfWords(text));  
        }
   if (choice == 'f'){
       printf("Edited text: %s\n", FixCapitalization(text));
      }
   if (choice == 'r'){
       printf("Edited text: %s\n", ReplaceExclamation(text));
      }
   if (choice == 's'){
      printf("Edited text: %s\n", ShortenSpace(text));
      }
   if (choice == 'q'){
      return;
      }
   
   
   }


int main(void) {
   
   /* Type your code here. */

   char text[300];

   printf("Enter a sample text: ");
   scanf("%s", text);
   
   printf("You entered: %s", text);

   char choice;

   while(1){
      PrintMenu();
      printf("Choose an option: ");
      scanf(" %c", &choice);
      ExecuteMenu(choice, text);
   
      }

   return 0;
}