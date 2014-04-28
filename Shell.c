/*
 * Shell.c
 *
 *  Created on: Mar 12, 2014
 *      Author: itamarsh
 */
//IO includes
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// shell includes
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

//signals includes
#include <signal.h>

//methods
void PrintTokens (char** tokens);
char** ReadTokens(FILE* stream);
void FreeTokens(char** tokens);
void MainLine(int returned);
int RunExec(char** tokens);
void sig_handler(int sig);

//statics and defines
static int opened=-1;
static char* filename;
#define ERROR 255
int main ()
{

	char** tokens;// the line
	int returned = 0;//return code
	while (1)//infinite loop
	{
		opened = -1;//mark file as closed
		MainLine(returned);//print Main Line
		tokens = ReadTokens(stdin);//read the line and convert it to words array
		if(tokens != NULL)//if the array is not empty
		{

			if(strcmp(tokens[0],"exit")==0&&tokens[1]==NULL)// exit condition
				return EXIT_FAILURE;
			returned = RunExec(tokens);//run command

		}
		FreeTokens(tokens);//clean after ourself
	}
	FreeTokens(tokens);//case something was left
	return EXIT_SUCCESS;
}

/*
 * function to create the "<return code><login-name>@<hostname>$"
 */
void MainLine(int returned)
{
	char* userName = getlogin() ;//get login name
	char hostName[1024];//create string for host name
	hostName[1023] = '\0';//mark the end
	gethostname(hostName,1023);//copy host name into it
	printf("%d %s@%s$ ",returned, userName,hostName);//print the line
}
/*
 * to handle with signals
 */
void sig_handler(int sig)
{
	switch (sig)//check the signal
	{
	case SIGINT://if kill
		kill(getpid(),SIGINT);//kill running child
		printf("\n");//go down a line
		break;
	default:
		abort();
	}
}

/*
 * this function will run the commands
 */
int RunExec(char** exec)
{
	//init vars
	int ret = 0;
	int file;
	//process vars
	pid_t pid;//child pid
	pid_t wait_pid;//pid to check if hild done
	signal(SIGINT,SIG_IGN);//ignore the INt signal
	pid = fork();//create child
	if(pid == 0)//child process
	{
		signal(SIGINT,sig_handler);//run in case SIGNAL was gotten
		if (opened==-1)//case file is closed
		{
			ret= execvp(exec[0],exec);//run command
		}
		if(opened!=-1)//if file is open
		{
			printf("---performed: %s on file: %s\n---", exec[0], filename);//print file info
			file = open(filename,  O_RDWR | O_CREAT,0666);//open file
			dup2(file, STDOUT_FILENO);//duplicate to stdout
			close(file);//close file
			ret= execvp(exec[0],exec);//run command
		}
		opened = -1;//mark file as closed
		if(ret==-1)// if error accured
		{
			PrintTokens(exec);//command not found error
			exit(ret);//close 'son'
		}

	}
	if(pid < 0)//if cant create process
		printf("can't create process! \n");
	else if(pid>0)//father process
	{
		do
		{
			wait_pid = wait(&ret);//wait until son is done and get his return code
			ret = WEXITSTATUS(ret);
		}
		while (pid!=wait_pid);
		return ret;

	}
	return ret;
}
/*
 * free memory
 */
void FreeTokens(char** tokens)
{
	int i;
	for(i = 0;tokens[i]!=NULL;i++)// run on the array and free memory
		free(tokens[i]);

	free(tokens);//free main pointer
}

/*
 * print the tokens
 */
void PrintTokens (char** tokens)
{
	int i=0;
	for (i=0; tokens[i]!=NULL;i++)
	{
		printf("%s ",tokens[i] );//print all the words

	}
	printf(": Command Not Found\n");
}
/*
 * convert tokens into words array
 */
char** ReadTokens(FILE* stream)
{
	//variables
	char token1[512], token2[512];
	char** words;
	char* temp;
	int i = 0,size = 0, found = 0;

	fgets(token1,512,stream);//create string from file
	for (i=0;token1[i]!='\n';i++);//get the last char' place
	token1[i] = '\0';//put NULL in the end
	if (opened ==-1	)// to run every time
	{
		for (i=0;i<strlen(token1);i++)// search for > in the input
		{
			if (token1[i]=='>')//found >!
			{
				if(found==1)//case more > FOUND
				{
					fprintf(stderr, "cant put '>' in filename!\n");
					found =0;// mark not found
					break;
				}
				token1[i]='\0';//set this place to NULL so the string will break here
				filename = &token1[i+1];//create a new file name
				opened = i;//mark the place
				found = 1;//to mark as found
				break;//stop! found one
			}
		}
		if(token1[opened+1]==' ')
		{
			filename++;//create a new file name
			found = 0;
		}
	}
	i = 0;
	strcpy(token2,token1);//copy to backup string
	temp = strtok(token1, " ");//get all word on by one
	while (temp!=NULL)
	{
		temp = strtok(NULL, " ");
		size++;//count words number
	}

	words = (char**)malloc((size+1)*sizeof(char*));//memory allocation
	if(words==NULL)//case allocation failed
		return NULL;//to mark as empty
	i=0;//back to 0
	temp =strtok(token2, " ");
	while (i<size)//acctually creates the array
	{
		words[i] = (char*)malloc((strlen(temp)+1)*sizeof(char));//memory allocation
		if (words[i]==NULL)//if malloc filed
		{
			return NULL;//to mark as empty
		}
		strcpy(words[i],temp);//copy to temporary string
		temp = strtok(NULL," ");
		++i;
	}
	if(i>0)// if there ar more than one words
		words[i] = NULL;

	return words;//return string
}
