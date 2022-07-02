#include "shell.h"


// vars to load system programs
char output_file_path[PATH_MAX];
char local_path[PATH_MAX] = "/bin/";
char program_name[PATH_MAX];

/*
  Custom color helper function
*/
void red()
{
  printf("\033[1;31m");
}
void yellow()
{
  printf("\033[1;33m");
}
void reset()
{
  printf("\033[0m");
}

void setup_program_path(char **args)
{
  char *name = args[0];
  strcpy(program_name, output_file_path);
  strcat(program_name, local_path);
  strcat(program_name, name);
}

void print_error_message(char **args)
{
  char error_message[SHELL_BUFFERSIZE];
  sprintf(error_message, "Command %s not found", args[0]);
  perror(error_message);
}

int exec_sys_prog(char **args)
{
  setup_program_path(args);

  if (execvp(program_name, args) == -1)
  {
    print_error_message(args);
  }

  // This line will only be reached if execvp fails to create new process image
  return -1;
}

/****************** Internal Shell Functions ************************/
/**
   Change shell current directory, internal shell code
 */
int shell_cd(char **args)
{
  if (args[1] == NULL)
  {
    fprintf(stderr, "CSEShell: expected argument to \"cd\"\n");
  }
  else
  {
    // chdir() changes the current working directory of the calling process
    // to the directory specified in path.
    if (chdir(args[1]) != 0)
    { // use chdir
      perror("CSEShell:");
    }
  }

  return 1;
}

/**
   Prints out the usage and
   list of commands implemented
 */
/*
  Builtin function implementations.
*/
int shell_usage(char **args)
{
  int function_index = -1;

  if (current_number_tokens < 2)
  {
    printf("Command not given. Type usage <command>.\n");
    return 1;
  }
  // Check if the commands exist in the command list
  for (int i = 0; i < num_builtin_functions(); i++)
  {
    if (strcmp(args[1], builtin_commands[i]) == 0)
    {
      // pass it to the functions
      function_index = i;
    }
  }

  switch (function_index)
  {
  case 0:
    printf("Type: cd directory_name to change the current working directory of the shell\n");
    break;
  case 1:
    printf("Type: help for supported commands\n");
    break;
  case 2:
    printf("Type: exit to terminate the shell gracefully\n");
    break;
  case 3:
    printf("Type: usage cd/help/exit\n");
    break;
  }

  return 1;
}

/**
   Prints out the usage and
   list of commands implemented
 */
int shell_help(char **args)
{
  int i;
  printf("CSEShell Interface\n");
  printf("Usage: command arguments\n");
  printf("The following commands are implemented within the shell:\n");

  for (i = 0; i < num_builtin_functions(); i++)
  {
    printf("  %s\n", builtin_commands[i]); // print all the commands that have been implemented in the shell program
  }

  printf("This shell also supports: listdir, listdirall, summond, checkdaemon, find, and countline.\n");

  return 1;
}

/**
  Returns 0, to terminate execution from the main_loop
 */
int shell_exit(char **args)
{
  return 0;
}

/**************************************************************/

/**
   Call shell builtin functions if the command matches builtin_commands
   Otherwise, execute the system program
 */
int process_command(char **args)
{
  int child_exit_status = -1;
  /** TASK 3 **/

  // 1. Check if args[0] is NULL. If it is, an empty command is entered, return 1
  // 2. Otherwise, check if args[0] is in any of our builtin_commands: cd, help, exit, or usage.
  // 3. If conditions in (2) are satisfied, call builtin shell commands, otherwise perform fork() to exec the system program. Check if fork() is successful.
  // 4. For the child process, call exec_sys_prog(args) to execute the matching system program. exec_sys_prog is already implemented for you.
  // 5. For the parent process, wait for the child process to complete and fetch the child's exit status value to child_exit_status
  // DO NOT PRINT ANYTHING TO THE OUTPUT

  /***** BEGIN ANSWER HERE *****/
  if (args[0] == NULL)
  {return 1;}

  for (int i=0; i <num_builtin_functions(); i++)
  {
    if (strcmp(builtin_commands[i],args[0]) == 0)
    {return (builtin_command_func[i](args));}
  }

  pid_t pid;
  pid = fork();
  
  if (pid < 0)
  {return 1;}

  else if (pid == 0)
  {
    int output = exec_sys_prog(args);
    if (output == -1)
    { exit(0); }
  }

  else //(pid > 0) in child process 
  {
    int status; 
    waitpid(pid,&status, WUNTRACED); //parent
    while(wait(NULL) > 0);
    if (WIFEXITED(status))
    {child_exit_status = WEXITSTATUS(status);}
  }
  /*********************/
  if (child_exit_status != 1)
  {
    printf("Command %s has terminated abruptly.\n", args[0]);
  }
  return 1;
}

/**
   Read line from stdin, return a pointer to the array containing the command string entered by the user
 */
char *read_line_stdin(void)
{
  size_t buf_size = SHELL_BUFFERSIZE;           // size of the buffer
  char* line = malloc(sizeof(char) * buf_size); // allocate memory space for the line*
  /** TASK 1 **/
  // read one line from stdin using getline()
  // 1. Check that the char* returned by malloc is not NULL
  // 2. Fetch an entire line from input stream stdin using getline() function. getline() will store user input onto the memory location allocated in (1)
  // 3. Return the char*
  // DO NOT PRINT ANYTHING TO THE OUTPUT

  /***** BEGIN ANSWER HERE *****/
  /*********************/
    if (line == NULL) 
  {
    return 0;
  }

  size_t characters = getline(&line, &buf_size, stdin);

  return line;
}

/**
 Receives the *line, and return char** that tokenize the line
**/

char **tokenize_line_stdin(char *line)
{

  // create local variables to store the array of pointers to the first char of each word in the line
  int buf_size = SHELL_BUFFERSIZE, position = 0;     // assume there's also BUFFERSIZE amount of token, which is certainly enough because there's only BUFFERSIZE amount of chars
  char **tokens = malloc(buf_size * sizeof(char *)); // an array of pointers to the first char that marks a token in line
  char *token;

  /** TASK 2 **/
  // 1. Check that char ** that is returned by malloc is not NULL
  // 2. Tokenize the input *line using strtok() function
  // 3. Store the address to first letter of each word in the command in tokens
  // 4. Add NULL termination in tokens so we know how many "valid" addresses there are in tokens
  /***** BEGIN ANSWER HERE *****/
  int i = 0;

  if (tokens == NULL) 
  {
    return 0;
  }

  token = strtok(line, SHELL_INPUT_DELIM);

  while (token != NULL)
  {
    tokens[i] = token;
    token = strtok(NULL, SHELL_INPUT_DELIM);
    i++;
  }
  current_number_tokens = i;
  /*********************/

  return tokens;
}

/**
  The main loop where one reads line,
  tokenize it, and then executes the command
 */
void main_loop(void)
{
  // instantiate local variables
  //char *line;  // to accept the line of string from user
  //char **args; // to tokenize them as arguments separated by spaces
  int status;  // if status == 1, prompt new user input. else, terminate the shell program.

  /** TASK 4 **/
  // write a loop where you do the following:
  // 1. invoke read_line_stdin() and store the output at line
  // 2. invoke tokenize_line_stdin(line) and store the output at args**
  // 3. execute the tokens using process_command(args)

  // Basic cleanup for the next loop
  // 4. free memory location containing the strings of characters
  // 5. free memory location containing char* to the first letter of each word in the input string
  // 6. check if process_command returns 1. If yes, loop back to Step 1 and prompt user with new input. Otherwise, exit the shell.
  // DO NOT PRINT ANYTHING TO THE OUTPUT
  do
  {
    // decorate the prompt
    time_t raw_time;
    struct tm *time_info;
    time(&raw_time);
    time_info = localtime(&raw_time);
    printf("🐚");
    yellow();
    char *timeString = asctime(time_info);
    timeString[strlen(timeString) - 1] = '\0';
    printf(" %s", timeString);
    red();
    printf(" CSEShell\n↳ ");
    reset();
    fflush(stdout); //clear the buffer and move the output to the console using fflush

    /***** BEGIN ANSWER HERE *****/
    char *line = read_line_stdin();
    char **args = tokenize_line_stdin(line);
    status = process_command(args);

    if (status == 1)
    {
      free(line);
      free(args);
    }
    //free(line);
    //free(args);

    else //ok
    {break;}
    /*********************/
  } while (status);
}

int main(int argc, char **argv)
{

  printf("CSEShell Run successful. Running now: \n");

  // Setup path
  if (getcwd(output_file_path, sizeof(output_file_path)) != NULL)
  {
    printf("Current working dir: %s\n", output_file_path);
  }
  else
  {
    perror("getcwd() error, exiting now.");
    return 1;
  }

  // Run command loop
  main_loop();

  return 0;
}

// int main(int argc, char **argv) // task1
// {
 
//  char* line = read_line_stdin();
//  printf("The fetched line is : %s \n", line);
 
//  return 0;
// }

// int main(int argc, char **argv)
// {
 
//  printf("Shell Run successful. Running now: \n");
 
//  char* line = read_line_stdin();
//  printf("The fetched line is : %s \n", line);
 
//  char** args = tokenize_line_stdin(line);
//  printf("The first token is %s \n", args[0]);
//  printf("The second token is %s \n", args[1]);
 
//  return 0;
// }

// int main(int argc, char **argv) //task 3
// {

//   printf("Shell Run successful. Running now: \n");

//   char *line = read_line_stdin();
//   printf("The fetched line is : %s \n", line);

//   char **args = tokenize_line_stdin(line);
//   printf("The first token is %s \n", args[0]);
//   printf("The second token is %s \n", args[1]);

//   //Setup path
//   if (getcwd(output_file_path, sizeof(output_file_path)) != NULL)
//   {
//     printf("Current working dir: %s\n", output_file_path);
//   }
//   else
//   {
//     perror("getcwd() error, exiting now.");
//     return 1;
//   }
//   process_command(args);

//   return 0;
// }