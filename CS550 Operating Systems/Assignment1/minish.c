#include <stdio.h>
#include <sys/types.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include<signal.h>

//Structure for storing Process
struct Process 
{
   pid_t Chld_pid;
   char  Chld_Cmd[100];
   char  Proc_type;
   
};

void sigint_handler(int);
pid_t kill_process_id=-1;

//Function to kill process when ctrl+c is pressed

void sigint_handler(int sig)
{
    printf("killing process %d\n",kill_process_id);
    kill(kill_process_id, SIGKILL);
}
//Function for printing all the background processes

void PrintBgProcessList(struct Process *P,int ChildCount)
{
	int index = 0;
	printf("List of backgrounded processes:\n");
	while(index < ChildCount)
	{
		if(P->Chld_pid != 0 && P->Proc_type == 'b')
		{
			printf("Command '%s' with PID %d Status:RUNNING\n",P->Chld_Cmd, P->Chld_pid);
		}
		P++;
		index++;
	}
}

//Function for checking if a process exists in the list of processes
int CheckProcessList(struct Process *P,int ChildCount,pid_t pid)
{
	int index = 0,flag=0;
	while(index < ChildCount)
	{
		if(P->Chld_pid == pid)
		{
			flag = 1;
			break;
		}
		P++;
		index++;
	}
	return flag;

}


void main()
{
	pid_t pid;
	char command[100],main_command[100],arg[100],*ptr[100],*ptr_cmd[100];
	int i=0,j=0,k=0,len=0,status;
	struct Process P[100],*p_ptr;

	//Function to kill process when ctrl+c is pressed
	signal(SIGINT, sigint_handler);       
	//Infinite loop for running more commands
	while(1)
        {
		setbuf(stdout,NULL);
        	printf("Minish> ");
        	gets(command); //Get command input
        	i=0;
        	j=0;

        	len = strlen(command);

		//Checking if command it’s a command to exit process
        	if(len == 4 && strcmp(command,"exit") == 0)
        	{
            		exit(0);//Exit if command is exit
        	}
        	else if ((command[0] == 'E' || command[0] == 'e') && (command[1] == 'X' || command[1] == 'x') && (command[2] == 'I' || command[2] == 'i') && (command[3] == 'T' || command[3] == 't'))
        	{
            		exit(0);//Exit if command is exit
        	}
		//Checking of there’s null input
        	if(command[0]=='\n' || command[0] == '\0')
		{
			continue;//Moving onto next loop for getting next input
		}
		//Checking if the command is to list all background jobs
		if(strcmp(command,"listjobs") == 0)
		{
			p_ptr = P;
			PrintBgProcessList(p_ptr,k);//Printing all the background processes
			continue;//Moving onto next loop for getting next user input
		}
		
		i=0;
		int counter=0;
		//Checking if the command is to be run in Background
		while (command[i] != '\0')
		{
	                if (command[i] != '&') 
        	        {
					arg[i]=command[i];
					i++;
                	}
                	else 
                	{
	
                   		break;
                	}
        	}
		arg[i]= '\0';
			
		int isPresent=0;
		char *token;
		token = strtok(arg, " ");
		ptr_cmd[counter++] = token;
		//Checking if it’s a command for making a process foreground
		if(strcmp(token, "fg")==0)
		{
			token = strtok(NULL, " ");
			int fg_pid=atoi(token);
			p_ptr = P;
			isPresent = CheckProcessList(p_ptr,k,fg_pid); //Checking if PID is in List of background process
			if(isPresent == 1)
			{
				printf("Bringing background process %d to foreground\n",fg_pid);
				kill_process_id = pid;
				while (1) //Waiting for Process to finish Execution
				{
					if (wait(&status) == pid)
					{
						break;
					}	
				}
			}
			continue;
		}
		//Splitting String by space and sorting them in variables.
		*ptr = token;
		while( token != NULL )
   		{
			token = strtok(NULL, " ");
			ptr_cmd[counter++]  = token;	
		}
		ptr_cmd[counter] = NULL;

		i=0;
		while (command[i] != '\0')
        	{
                	if (command[i] != '&') 
                	{
				i++;
                	}
			else
			{
				break;
                	}
        	}
		//Creating a new Process
		pid = fork();
		if(pid == 0)
		{
			execvp(ptr[0],ptr_cmd); //Executing the command entered by user
			fprintf(stderr, "Execvp Operation Failed\n");
			exit(2);
		}
		else if(pid < 0)
		{
			fprintf(stderr, "Fork Operation Failed\n");
			exit(1);
		}
		else
		{
                	P[k].Chld_pid = pid;
			strcpy(P[k].Chld_Cmd,command); 
			//Foreground process Execution
			if(command[i] != '&')
			{
				kill_process_id = pid;
				while (1)
				{
					if (wait(&status) == pid)
					{
						P[k].Proc_type='f';
						break;
					}
				}
			}
			else // Doing nothing if it’s a background process 
			{
					P[k].Proc_type='b';
			}
			k++;
		}
	}
}
