/* COMP 530: Tar Heel SHell */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <fcntl.h>

// Assume no input line will be longer than 1024 bytes
#define MAX_INPUT 1024

//global variables
char *OLDPWD;
//end global variables

//start linked list implementation
struct node {
    char *data; //stored data of the path
    struct node *next;
} *head, *tail;


void insert(char *data) {
    struct node *new_node = malloc(sizeof(struct node));
    new_node->data = data;
    new_node->next = NULL;
    
	if(head == NULL) {
		head = new_node;
		tail = new_node;
	}
	else {
		tail->next = new_node;
		tail = new_node;
	}
}

void printLinkedList() {
	struct node *temp = malloc(sizeof(struct node));
	temp = head;
	while(temp != NULL) {
		printf("%s\n", temp->data);
		temp = temp->next;
	}
	free(temp);
}

//end linked list implementation

//path check start
//this method checks for strictly "PATH=". helps exclude
//others such as "HOMEPATH"
int checkPath(char *env) {
    char *name = "PATH=";
    int i, check = 1;
    char * path;
    path = env;
    for(i = 0; i < 5; i++) {
        if(name[i] != *path) {
            check = 0;
        }
        path++;
    }
    return check;
}
//path check end

//path check start
//this method checks for strictly "PATH=". helps exclude
//others such as "HOMEPATH"
int checkHome(char *env) {
    char *name = "HOME=";
    int i, check = 1;
    char * path;
    path = env;
    for(i = 0; i < 5; i++) {
        if(name[i] != *path) {
            check = 0;
        }
        path++;
    }
    return check;
}
//path check end

//path check start
//this method checks for strictly "PATH=". helps exclude
//others such as "HOMEPATH"
int checkOldD(char *env) {
    char *name = "OLDPWD=";
    int i, check = 1;
    char * path;
    path = env;
    for(i = 0; i < 7; i++) {
        if(name[i] != *path) {
            check = 0;
        }
        path++;
    }
    return check;
}
//path check end

//change old directory start
void changeOldD() {
	if(OLDPWD == NULL) {
		OLDPWD = malloc(MAX_INPUT * sizeof(char *));
	}
//	char **oldPwd;
//	for (oldPwd = envp; *oldPwd != 0; oldPwd++)
//  	{
//		char* path_finder;
//		path_finder = strstr(*oldPwd, "OLDPWD="); //using strstr to find path in environment
//		if(path_finder != NULL){
//			break;
//		}
//	}
	char *cwd = malloc(MAX_INPUT * sizeof(char *));
	getcwd(cwd, MAX_INPUT);
//	char *temp = malloc(MAX_INPUT * sizeof(char *));
	memset(OLDPWD, '\0', strlen(cwd) + 1);
//	strcat(OLDPWD, "OLDPWD=");
	strcat(OLDPWD, cwd);
	
	cwd = NULL;
	free(cwd);
}
//change old directory end

//path search start
char *searchPath(char *cmdline) {
	struct stat *buff = malloc(sizeof(struct stat));
	if(stat(cmdline,buff) == 0) { //initial check to see if command is executable;
		buff = NULL;
		free(buff);
		char *tempstr = calloc(strlen(cmdline)+1, sizeof(char *));//create a tempstr so orignal data will not be altered
		strcpy(tempstr, cmdline);
		return tempstr;
	}
	buff = NULL;
	free(buff);
	char *cmd = strtok(cmdline, " ");
	if (cmd == NULL) { //no other characters bsides the main part of command
		strcpy(cmd, cmdline);
	}
	struct node *temp = malloc(sizeof(struct node));
	temp = head;
	while(temp != NULL) { //check path to find command
		char *path = malloc(sizeof(char));
		strcpy (path, temp->data);
		path = strcat(path,"/");
		path = strcat(path,cmd);
		struct stat *buf = malloc(sizeof(struct stat));
		if(stat(path,buf) == 0) {
			temp = NULL; //included to fix problem of free(temp) freeing the current node
			buf = NULL;
			free(temp);
			free(buf);
			return path;
		} else {
			path = NULL;
			free(path);
		}
		temp = temp->next;
		buf = NULL;
		free(buf);
	}
	temp = NULL;
	free(temp);
	return NULL;
}
//path search end

//start process job
void processJob(char *cmddirectory, char * cmd, int debug_status) {
	int status;
	
	if(cmddirectory != NULL) {
		
		char *parm_list[MAX_INPUT]; //list set up for straightforward(sf) process
		char *redir_list[MAX_INPUT]; //list set up to handle redirection
		char *tempstr = calloc(strlen(cmd)+1, sizeof(char *));//create a tempstr so orignal data will not be altered
		strcpy(tempstr, cmd);
		char **nextcmd = parm_list; //sf-process pointer
		char **nextrdr = redir_list; //redirection pointer
		char *token = strtok(tempstr," ");
		while(token != NULL) { //using the tokenized string to set up the arg list
			if((strcmp(token,">")==0) ||(strcmp(token,"<")==0)||(strcmp(token,"|")==0) ) { //check to see if token is any of the redirection symbols
				*nextrdr = token;
				*nextcmd = token;
				nextrdr++;
//				nextcmd++;
//				token = strtok(NULL, " ");
//				while(token !=NULL) {
//					*nextrdr = token;
//					nextrdr++;
//					token = strtok(NULL, " ");
//				}
//				break;
			} else { //if not, continue as if straightforward
			*nextcmd = token;
			}
//			if(getenv(*nextcmd) != NULL) {
//				*nextcmd = getenv(*nextcmd);
//			}
			if( memcmp(token, "$", 1) == 0) {//if 0 this is a variable
				char *tempTok = calloc(strlen(token)+1, sizeof(char *));//create a tempstr so orignal data will not be altered
//				strcpy(temptok, cmd);
				memmove(*nextcmd,*nextcmd+1,strlen(token)); //overwrite the $ to leave env varname
//				nextcmd[strlen(*nextcmd)-1] = "\0";
				strcpy(tempTok,getenv(*nextcmd)); //get env variable
				*nextcmd = NULL;
				*nextcmd = tempTok;
				tempTok = NULL;	
				free(tempTok);
			}
			nextcmd++;
			token = strtok(NULL, " ");			
		}
//		printf("%i\n", 1);
		*nextcmd = NULL; //null terminate to handle garbage
		*nextrdr = NULL;
		nextrdr = redir_list;
//		printf("%i\n", 2);
		if(*nextrdr == NULL) {//sf-process
//			printf("%i\n", 3);
			int pid = fork();
			if (pid == 0) {//child
				execv(cmddirectory, parm_list);
				printf("%s\n", "Invalid command. Should not have returned");
				exit(EXIT_SUCCESS);
			} else {//parent
				waitpid(pid, &status, 0);
				printf("%s\n", "Child has returned");
				char *return_status = malloc(sizeof(char *));
				sprintf(return_status, "%d", status);
				setenv("?", return_status,1);
				return_status = NULL;		
				free(return_status);

			}
		} else {//redirection
			//setting up variables to seperate commands by the redirection sumbols
			//the current command will be ran and the rest reran through recursion
			char *current_cmd[MAX_INPUT];
			char *current_file = malloc(MAX_INPUT * sizeof(char *));
			char *rest_cmd = malloc(strlen(cmd) * sizeof(char *));
//			memset(rest_cmd,'\0', strlen(rest_cmd));
			char **nextcurrent = current_cmd;
			nextcmd = parm_list;
			nextrdr = redir_list;
			while(nextcmd != NULL) {//gather current command
				if((strcmp(*nextcmd,">")==0) ||(strcmp(*nextcmd,"<")==0)||(strcmp(*nextcmd,"|")==0)) {
					nextcmd++;
					current_file = *nextcmd++;
					break;
				}
				*nextcurrent++ = *nextcmd++;
			}
			
			while(*nextcmd != NULL) {//gather the rest of the command into a single stream
				char *temp = calloc(strlen(*nextcmd), sizeof(char *));
				strcpy(temp,*nextcmd);
				strcat(rest_cmd,temp);
				strcat(rest_cmd, " ");
				nextcmd++;
				free(temp);
			}
			int file_fd;
			int pipe_fd[2];
			pipe(pipe_fd);
			int cpid = fork();
//			char pipe_buf;
			if(cpid == 0) {
				if(strcmp(*nextrdr, "<") == 0) {//handling for file input
	//				char *new_cmd = malloc(strlen(cmd) * sizeof(char *));
//					char *lt_location;
	//				lt_location = strchr(cmd, '<');
	//				printf("%i\n", lt_location-cmd+1);
//					cmd[(lt_location-cmd)] = ' ';
	//				printf("%s\n", cmd);
//					processJob(cmddirectory, cmd, debug_status);
	//				nextrdr++;
	//				FILE *input_file;
	//					
	//				input_file = fopen(*nextrdr, "r");
	//				if((input_file = popen(cmd,"r")) == NULL){
	//					perror("Error opening file");
	//					return;
	//				}else{
	//					printf("%i\n", 4);
	//				}
	//				long file_size;
	//				char *file_contents;

	//				input_file = fopen(*nextrdr,"r");
	//				if(input_file == NULL) {
	//					perror("Error opening file");
	//				} else {
	//					char c;
	//					while ((c = fgetc(input_file)) != EOF) {
	//						putchar(c);	
	//					}
	//					fseek(input_file, 0, SEEK_END);
	//					file_size = ftell(input_file);
	//					rewind(input_file);
	//					file_contents = malloc(file_size * sizeof(char *));
	//					fread(file_contents,1,file_size,input_file);
						//back here homie

	//				}
					FILE *input = fopen(current_file, "r");
					file_fd = fileno(input);
					dup2(file_fd, STDIN_FILENO);
//					fclose(input);
					execv(cmddirectory, current_cmd);
				} else if(strcmp(*nextrdr, ">") == 0) {//handling for file output
					FILE *output = fopen(current_file, "w");
					file_fd = fileno(output);
					dup2(file_fd, STDOUT_FILENO);
//					fclose(output);
					execv(cmddirectory, current_cmd);
				} else if(strcmp(*nextrdr, "|")) {

				}
			} else {
				waitpid(cpid, &status, 0);	
			}
		}
	} else {
		printf("%s\n", "Could not find command");
		status = -1;
	}
	if(debug_status == 1) {
		printf("%s%s%s%d%s\n", "ENDED: ", cmd, " (ret=", status, ")");
	}
}

//end process job

//start parse command
void parseCommand(char *cmd, char *prompt, char **envp, int debug_status) {
	char *tempstr = calloc(strlen(cmd)+1, sizeof(char *));//create a tempstr so orignal data will not be altered
	strcpy(tempstr, cmd);
	char *token = strtok(tempstr, " ");
	int success;
	if(strcmp(token,"exit") == 0) {
		printf("%s\n", "Byeeeeeee");
		FILE *unc;
		unc = fopen("unc.txt","r");
		if(unc == NULL){
			perror("UNC may be broken, but still wishes you a nice exit");
		} else {
			char c;
			while ((c = fgetc(unc)) != EOF) {
				putchar(c);	
			}
		}
		
		exit(3);
	} else
	if(strcmp(token,"cd") == 0) {
		char** env;
		token = strtok(NULL, " ");
		if(token == NULL || strcmp(token,"~") == 0) {
			char* path_finder;
			for (env = envp; *env != 0; env++)
  			{
 				path_finder = strstr(*env, "HOME="); //using strstr to find path in environment
    			if(path_finder != NULL){
					char *thisEnv = calloc(strlen(*env)+1, sizeof(char *));//create a tempstr so orignal data will not be altered
					strcpy(thisEnv, *env);
        			if(checkHome(*env)) { //checks for correct "HOME="
						char *home_token = strtok(thisEnv, "=");
						home_token = strtok(NULL, "\n");
						//home_token[strlen(home_token)+1] = NULL;
						changeOldD(); //currently debugging change old directory
						success = chdir(home_token);
					}
				}
			}
			path_finder=NULL;
			free(path_finder);
		} else if(strcmp(token,"-") == 0) {
			char* path_finder;
			for (env = envp; *env != 0; env++)
  			{
				
 				path_finder = strstr(*env, "OLDPWD="); //using strstr to find path in environment
    			if(path_finder != NULL){
					char *thisEnv = calloc(strlen(*env)+1, sizeof(char *));//create a tempstr so orignal data will not be altered
					strcpy(thisEnv, *env);

        			if(OLDPWD != NULL) { //checks for correct "OLDPWD="
						char *oldd = OLDPWD;
						OLDPWD = NULL;
						free(OLDPWD);
						OLDPWD = malloc(MAX_INPUT * sizeof(char *));
						changeOldD();
						success = chdir(oldd);
					} else {
						printf("%s\n", "Previous working directory not set.");
					}
				}
			}
			path_finder=NULL;
			free(path_finder);
		}else{
			changeOldD();
			success = chdir(token);
		}
		if(success == 0) {
			char *cwd = malloc(MAX_INPUT * sizeof(char *));
  			getcwd(cwd, MAX_INPUT);
			memset(prompt, '\0', MAX_INPUT);
			strcat(prompt,"[");
			strcat(prompt, cwd);
			strcat(prompt, "] thsh> ");
			cwd = NULL;
			free(cwd);
		} else {
			printf("%s\n", "Invalid directory, cd command failed.");
		}
	} else if(strcmp(token,"set") == 0){ //from here handle separating based on =
		token = strtok(NULL, " ");
		if (token != NULL) {
			putenv(token);
			
		} else {
			printf("%s\n", "Hmmmmm? It doesn't look like you gave a variable to set");
		}
	} else if(strcmp(token,"goheels") == 0) {
			FILE *ramsees;
			printf("%i\n", 1);
			ramsees = fopen("ramsees.txt","r");
			if(ramsees == NULL){
				perror("RIP, Ramsees is currently down for the count");
			} else {
				char c;
				while ((c = fgetc(ramsees)) != EOF) {
					putchar(c);	
				}
			}
	}else {
		printf("%s\n", "Looks like a command we'll need to make");
	}
	
	token = NULL;
	tempstr=NULL;	
	free(token);
	free(tempstr);
	char *return_status = malloc(sizeof(char *)); //$?
	sprintf(return_status, "%d", success);
	setenv("?",return_status,1);
	return_status = NULL;
	free(return_status);
	if(debug_status == 1) {
		printf("%s%s%s%d%s\n", "ENDED: ", cmd, " (ret=", success, ")");
	}
	
}
//end parse command

//main
int
main (int argc, char ** argv, char **envp) {
  int finished = 0;
  int debug_status = 0;
  if(argv[1] != NULL) {
  	if(strcmp(argv[1],"-d\n")) {
		debug_status = 1;
	}
  }
  	
  char *cwd = malloc(MAX_INPUT * sizeof(char *));
  getcwd(cwd, MAX_INPUT);
  char *prompt = malloc(MAX_INPUT * sizeof(char *));
  strcat(prompt, "[");
  strcat(prompt, cwd);
  strcat(prompt, "] thsh> \0");
  //char *prompt = strcat(cwd," thsh> ");
  char cmd[MAX_INPUT];

  char** env;
  for (env = envp; *env != 0; env++)
  {
  	//assign each to a variable directed to their name

    char* path_finder;
    path_finder = strstr(*env, "PATH="); //using strstr to find path in environment
    if(path_finder){
		char *thisEnv = calloc(strlen(*env)+1, sizeof(char *));//create a tempstr so orignal data will not be altered
		strcpy(thisEnv, *env);
        //char* thisEnv = *env;

        if(checkPath(*env)) { //checks for correct "PATH="
            const char path_token[5] = "PATH=";
            char *path_wo_head, *token;
            token = strtok(thisEnv, path_token); // use strtok to remove "PATH=" from the front of the path"
			while(token != NULL)
			{
				path_wo_head = token; //store the path without "PATH=" in path_wo_head variable
				token = strtok(NULL, path_token);
				
			}
			//break up path into sepearate paths and store them in the linked list
			token = NULL;
			token = strtok(path_wo_head,":");
			while(token != NULL)
			{
				insert(token);
				token = strtok(NULL, ":");
			}
			break;
        }
		
    }
	

  }
  putenv("?=0");
  //setenv("$?","0",1);//set variable for return status;
  while (!finished) {
    char *cursor;
    char last_char;
    int rv;
    int count;


    // Print the prompt
    rv = write(1, prompt, strlen(prompt));
    if (!rv) {
      finished = 1;
      break;
    }

    // read and parse the input
    for(rv = 1, count = 0,
	  cursor = cmd, last_char = 1;
	rv
	  && (++count < (MAX_INPUT-1))
	  && (last_char != '\n');
	cursor++) {

      rv = read(0, cursor, 1);
      last_char = *cursor;
    }
    *cursor = '\0';

    if (!rv) {
      finished = 1;
      break;
    }


    // Execute the command, handling built-in commands separately
    // Just echo the command line for now
    //write(1, cmd, strnlen(cmd, MAX_INPUT));
	char *new_line = malloc(strlen(cmd) * sizeof(char *));
	new_line = strchr(cmd, '\n');
	if(new_line != NULL) { //remove trailing newline from end of command
		*new_line = '\0';
	}
	char *tempstr = calloc(strlen(cmd)+1, sizeof(char *));//create a tempstr so orignal data will not be altered
	strcpy(tempstr, cmd);
	
	char *current_dir = searchPath(tempstr); //find which path current cmd in, if any
	if(debug_status == 1) {
		if(debug_status == 1) {
		printf("%s%s\n", "RUNNING: ", cmd);
	}
	}
	if(current_dir != NULL) {
		
		processJob(current_dir, cmd, debug_status);
	} else {
		tempstr = realloc(tempstr, strlen(cmd)+1);
		strcpy(tempstr, cmd);
		char *token = strtok(tempstr, " ");
		if(token != NULL) {
			parseCommand(cmd, prompt, envp, debug_status); //currentlly debugging parse command
		}
	}
	new_line = NULL;
	current_dir = NULL;
	tempstr = NULL;
	free(new_line);
	free(current_dir);
	free(tempstr);
	
	//free(token); double free or corruption

  }

  return 0;
}
