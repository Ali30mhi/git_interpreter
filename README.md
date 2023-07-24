# git_interpreter
A translator for Forth language, which also uses Git 
git init - Initializes a new Git repository in the current directory. 
git clone - Creates a copy of a remote Git repository onto your local machine.
git commit - Commits changes in the staging area to the local repository
git push - Pushes changes from the local repository to a remote repository.


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <ctype.h>

void git_commit() {
    // Initialize a new Git repository
    system("git init");

    // Add all files to the staging area
    system("git add .");

    // Commit changes with a message
    system("initial your commit message");
    

    // Push changes to the remote repository
    system("git push origin master");

}


int global_mode=0;

struct stack
{
  int top;
  int array[1000];
};

void push(struct stack *s, int n)
{
  s->top++;
  s->array[s->top] = n;
}

int pop(struct stack *s)
{
  
  int n = s->array[s->top];
  s->array[s->top] = 0;
  s->top--;
  return n;
}

int f(char input[100], char output[100][100])
{
  int i1 = 0, i2 = 0;
  for (int i = 0; i < strlen(input) - 1; i++)
  {
    if (input[i] == ' ')
    {
      output[i1][i2] = '\0';
      i1++;
      i2 = 0;
      continue;
    }
    output[i1][i2] = input[i];
    i2++;
  }
  output[i1][i2] = '\0';
  return i1 + 1;
}

int shafaq(char array[100])
{
  int sum = 0;
  for (int i = 0; i < strlen(array); i++)
  {
    if (!isdigit(array[i]))

    {
      return -1;
    }
    sum = sum * 10 + (array[i] - '0');
  }
  return sum;
}

void dot(struct stack *s)
{
  int operand2 = pop(s);
  int operand1 = pop(s);
  printf("%d.%d\n", operand1, operand2);
}

void dot_s(struct stack *s)
{
  int operand2 = pop(s);
  int operand1 = pop(s);
  printf("%d %d\n", operand1, operand2);
}

void addition(struct stack *s)
{
  int operand2 = pop(s);
  int operand1 = pop(s);
  push(s, operand1 + operand2);
}

void subtraction(struct stack *s)
{
  int operand2 = pop(s);
  int operand1 = pop(s);
  push(s, operand1 - operand2);
}

void multiplication(struct stack *s)
{
  int operand2 = pop(s);
  int operand1 = pop(s);
  push(s, operand1 * operand2);
}

void division(struct stack *s)
{
  int operand2 = pop(s);
  int operand1 = pop(s);
  push(s, operand1 / operand2);
}

void mod_division(struct stack *s)
{
  int operand2 = pop(s);
  int operand1 = pop(s);
  push(s, operand1 % operand2);
}

void mod_operation(struct stack *s)
{
  int operand2 = pop(s);
  int operand1 = pop(s);
  int mod = operand1 % operand2;
  if (mod < 0)
  {
    mod += operand2;
  }
  push(s, mod);
}

void equal(struct stack *s)
{
  int operand2 = pop(s);
  int operand1 = pop(s);
  push(s, operand1 == operand2 ? -1 : 0);
}

void less_than(struct stack *s)
{
  int operand2 = pop(s);
  int operand1 = pop(s);
  push(s, operand1 < operand2 ? -1 : 0);
}

void greater_than(struct stack *s)
{
  int operand2 = pop(s);
  int operand1 = pop(s);
  push(s, operand1 > operand2 ? -1 : 0);
}

void and_operation(struct stack *s)
{
  int operand2 = pop(s);
  int operand1 = pop(s);
  push(s, operand1 && operand2 ? -1 : 0);
}

void or_operation(struct stack *s)
{
  int operand2 = pop(s);
  int operand1 = pop(s);
  push(s, operand1 || operand2 ? -1 : 0);
}

void invert(struct stack *s)
{
  int operand = pop(s);
  push(s, !operand);
}

void i_operation(struct stack *s)
{
  int operand2 = pop(s);
  int operand1 = pop(s);
  push(s, operand1 + operand2);
}

void j_operation(struct stack *s)
{
  int operand2 = pop(s);
  int operand1 = pop(s);
}


void clear_stack(struct stack *s)
{
  s->top = -1;
}

void dup(struct stack *s)
{
  if (s->top >= 0)
  {
    push(s, s->array[s->top]);
  }
 }

 void drop(struct stack *s) {
    if (s->top == -1) {
        printf("stack underflow");
        return;
    }
    s->top--;
 }

 int is_empty(struct stack *s)
{
  
  return s->top == -1;
}

void emit(struct stack* s) {
   printf("Hi");
   if(!is_empty(s)){
    char ascii_char = (char)pop(s);
    printf("%c", ascii_char);
   }
}

 
void swap(struct stack *s)
{
  if (s->top >= 1)
  {
    int temp = s->array[s->top];
    s->array[s->top] = s->array[s->top - 1];
    s->array[s->top - 1] = temp;
  }
}

int peek(struct stack *s)
{
  if (s->top >= 0)
  {
    return s->array[s->top];
  }
  else
  {
    printf("Error: Stack is empty.\n");
    return -1;
  }
}


int size(struct stack *s)
{
  return s->top + 1;
}

void rot(struct stack *s)
{
  if (s->top >= 2)
  {
    int temp = s->array[s->top];
    s->array[s->top] = s->array[s->top - 1];
    s->array[s->top - 1] = s->array[s->top - 2];
    s->array[s->top - 2] = temp;
  }
}

void point(struct stack *s)
{

  printf("Your string here");

}


 void colon(struct stack *s)
 {
   
       int num1, num2;

    if (is_empty(s)) {
        printf("The stack is empty. No operation performed.\n");
        return;
    }

    num1 = pop(s);

    if (is_empty(s)) {
        printf("Not enough numbers in the stack. No operation performed.\n");
        push(s, num1);
        return;
    }

    num2 = pop(s);

    push(s, num1);
}


void over(struct stack *s) {
  if (s->top > 1) {
    printf("Error: stack underflow\n");
    return;
  }
  int temp = s->array[s->top+1];
  s->array[s->top+1] = temp;
}

 

int execute_operator(char operator[1000], struct stack *s)
{
  if(strcmp("then",operator)==0){
    global_mode =0;
    return 0;
  }
  if (global_mode == 1)
  {
    if(strcmp(operator , "else")==0){
      global_mode=2;
      return 0;
    }else{
      return 0;
    }
  }
 
  else if (global_mode == 2 ){
      if ( strcmp(operator , "else")==0)
      {
        global_mode = 1;
        return 0;
      }
  }
  else if (global_mode == 3)
  {
    if(strcmp(operator ,"else")==0)
    {
      global_mode = 3 ;
      return 0;
    }
  }
   
  if (strcmp(operator, ".") == 0)
  {
    dot(s);
  }
  else if (strcmp(operator, ".s") == 0)
  {
    dot_s(s);
  }

  else if (strcmp(operator, "+") == 0)
  { 
    addition(s);
  }
  else if (strcmp(operator, "-") == 0)
  {
    subtraction(s);
  }
  else if (strcmp(operator, "*") == 0)
  {
    multiplication(s);
  }
   else if(strcmp(operator ,"is_empty")==0)
    {
       is_empty(s);
    }
  else if (strcmp(operator, "/") == 0)
  {
    division(s);
  }
  else if (strcmp(operator, "/mod") == 0)
  {
    mod_division(s);
  }
  else if (strcmp(operator, "mod") == 0)
  {
    mod_operation(s);
  }
  else if (strcmp(operator, "=") == 0)
  {
    equal(s);
  }
  else if (strcmp(operator, "<") == 0)
  {
    less_than(s);
  }
  else if (strcmp(operator, ">") == 0)
  {
    greater_than(s);
  }
  else if (strcmp(operator, "and") == 0)
  {
    and_operation(s);
  }

  else if (strcmp(operator, "or") == 0)
  {
    or_operation(s);
  }
  else if (strcmp(operator, "invert") == 0)
  {
    invert(s);
  }
  else if (strcmp(operator, ":")== 0)
  {
    colon(s);
  }
  else if (strcmp(operator, "i"))
  {
    i_operation(s);
  }
  else if (strcmp(operator, "j" ) == 0)
  {
    j_operation(s);
  }
  else if(strcmp(operator,"emit"))
  {
    emit(s);
  }
  else if(strcmp(operator,"cr"))
  {
    printf("\n");
  }
  else if(strcmp(operator,"drop"))
  {
    drop(s);
  }
  else if (strcmp(operator,"dup"))
  {
    dup(s);
  }
  else if(strcmp(operator,"swap"))
  {
    swap(s);
  }
  else if(strcmp(operator,"over"))
  {
    over(s);
  }
  else if(strcmp(operator,"rot"))
  {
    rot(s);
  }
  else if(strcmp(operator,":")==0)
  {
      point(s);
      global_mode = 3;
  }
  else if (strcmp(operator,"if")==0)
  {
   
    int x=pop(s);
    if(x==0){
      global_mode=1;
    }else{
      global_mode=2;
    }
    
}    
else if(strcmp(operator,"point")==0)
{
  point(s);
}

}


int main()
{
  struct stack s;
  s.top = -1;
  char input[100];
  char output[100][100];

  while (1)
  {
    printf("Enter a command or type 'exit' to quit: ");
    fgets(input, 100, stdin);

    if (strcmp(input, "exit\n") == 0)
    {
      break;
    }

    int count = f(input, output);
    for (int i = 0; i < count; i++)
    {
      int sum = shafaq(output[i]);
      if (sum != -1)
      {
        push(&s, sum);
      }
      else
      {
        execute_operator(output[i], &s);
      }
    }
    printf("%d %d\n", s.array[s.top], s.top);
    

  }
}    



