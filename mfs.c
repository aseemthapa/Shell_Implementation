/*
	Name: Aseem Thapa
	ID : 1001543178
*/


// The MIT License (MIT)
// 
// Copyright (c) 2016, 2017 Trevor Bakker 
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#define _GNU_SOURCE

#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

#define WHITESPACE " \t\n"      // We want to split our command line up into tokens
                                // so we need to define what delimits our tokens.
                                // In this case  white space
                                // will separate the tokens on our command line
#define SEMICOLON ";"

#define MAX_COMMAND_SIZE 255    // The maximum command-line size

#define MAX_NUM_ARGUMENTS 5     // Mav shell only supports five arguments


void sig_int(int signum){
	//This function does nothing;
	//printf("Ctrl+C\n");
}

void sig_tstp(int signum){
	//This function does nothing;
	//printf("Ctrl+Z\n");
}

int main()
{	  
	  //Creating struct for signal handling
	  struct sigaction act; 
	  memset(&act,'\0',sizeof(act)); 
	  act.sa_handler = &sig_int; //For signal Ctrl + C
	  struct sigaction actstp; //Creating struct for signal handling
	  memset(&actstp,'\0',sizeof(actstp)); 
	  actstp.sa_handler = &sig_tstp; //For signal Ctrl + Z 
	  sigaction(SIGINT, &act, NULL); //Sigint(Ctrl+C) handler overwritten to do nothing
	  sigaction(SIGTSTP, &actstp, NULL); //Sigtstp(Ctrl+Z) handler overwritten to do nothing
	  char * cmd_str = (char*) malloc( MAX_COMMAND_SIZE );
	  pid_t pids[15]; // To store pids
	  // To store history in which 50 commands are to be stored:
	  char history[50][256]; 
	  //This variable used to see how many pids have been stored:
	  int pid_stored = 0; 
	  //This variable used to see how many commands have been stored in history:
	  int history_stored = 0; 

	  while( 1 )
	  {
	    // Print out the msh prompt
	    printf ("msh> ");

	    // Read the command from the commandline.  The
	    // maximum command that will be read is MAX_COMMAND_SIZE
	    // This while command will wait here until the user
	    // inputs something since fgets returns NULL when there
	    // is no input
	    while( !fgets (cmd_str, MAX_COMMAND_SIZE, stdin) );

	    /* Parse input */
	    char *token[MAX_NUM_ARGUMENTS];
	    char *tokensemi[15]; //Tokenize the string by ;

	    int   token_count = 0; 
	    int   token_semi_count = 0;                                
		                                                   
	    // Pointer to point to the token
	    // parsed by strsep
	    char *arg_ptr;                                         
	    char *arg_semiptr;                                                       
	    char *working_str  = strdup( cmd_str );                
	    char *working_semi_str = strdup (cmd_str);
	    // we are going to move the working_str pointer so
	    // keep track of its original value so we can deallocate
	    // the correct amount at the end
	    char *working_root = working_str;

	    // Tokenize the input stringswith whitespace used as the delimiter
	    while ( ( (arg_ptr = strsep(&working_str, WHITESPACE ) ) != NULL) && 
		      (token_count<MAX_NUM_ARGUMENTS))
	    {
	      token[token_count] = strndup( arg_ptr, MAX_COMMAND_SIZE );
	      if( strlen( token[token_count] ) == 0 )
	      {
		token[token_count] = NULL;
	      }
		token_count++;
	    }
		
	    // Tokenize the input stringswith semicolon:
	    while ( ( (arg_semiptr = strsep(&working_semi_str, SEMICOLON ) ) != NULL) && 
		      (token_semi_count<15))
	    {
	      tokensemi[token_semi_count] = strndup( arg_semiptr, MAX_COMMAND_SIZE );
	      if( strlen( tokensemi[token_semi_count] ) == 0 )
	      {
		tokensemi[token_semi_count] = NULL;
	      }
		token_semi_count++;
	    }

		/*int token_index  = 0;
	    	for( token_index = 0; token_index < token_semi_count; token_index ++ ) 
	    	{
	     	 printf("token[%d] = %s\n", token_index, tokensemi[token_index] );  
	    	} //== Testing to see if tokens are being seperated correctly*/

	    //Normalize the strings in Semicolon token:
	    //if the first character is white space remove it
            	
		int l;
		for(l=0;l<token_semi_count;l++)
		{
			if(tokensemi[l][0]==' ')
			{
				//printf("IN\n"); //Debug text
			   int n;
			   for(n=0;n<strlen(tokensemi[l]);n++)
				{
				tokensemi[l][n] = tokensemi[l][n+1];			
				}
			}
			//Check to see if it does not end in new line:
			if(tokensemi[l][strlen(tokensemi[l])-1] != '\n')
			{
			//If not change it to a new line
				tokensemi[l][strlen(tokensemi[l])] = '\n';					
			}
			//printf("char = %c\n",tokensemi[l][strlen(tokensemi[l])-1]); //Debug line
			//printf("leng = %ld\n",strlen(tokensemi[l])); //Debug line
		}
		//printf("Seperated by ; = %d\n",token_semi_count); //Debug Line

		// To Print msh after new line character:
		if (token[0]=='\0')
		{  
			//printf("INnl\n"); // Test case to see how to compare with new line character
			continue;
		}
			
		
		//CASE FOR "cd" :
		else if(strcmp(token[0],"cd")==0)
		{
			//printf("INcd\n");	//CD Debug 
			chdir(token[1]);	//Change directory to token 1
			continue;
		}

		//CASE FOR "bg":
		else if(strcmp(token[0],"bg")==0)
		{
			int f; //arbitary integer
			f = raise(SIGCONT); //Continue the background process
			if(f!=0) printf("No signal\n"); //Debug line
			continue;
		}

		//Case for !n functionality of history:	
		else if(token[0][0]=='!')
		{
			//printf("IN!\n"); //Line to check for bugs
			int x; //integer to store the command number in
			x = atoi(token[0]+1); //Convert the string after ! to integer
			//printf("x = %d\n",x); //Test the output
			//Check if the number after ! is not between 1 and 15 continue to next line or history is empty
			if(x>15 || x<1 || (strlen(history[x-1])==0))
			{ 
				printf("Command not in history.\n");
				continue;
			}
			else
			{
				char* cmd = (char*) malloc( MAX_COMMAND_SIZE );//Create a new variable to store command
				strcpy(cmd,history[x-1]); //Copy the element of history into this variable
				char* tokencmd[MAX_NUM_ARGUMENTS];
				int   token_cmd_count = 0;                                 				                                   
				// Pointer to point to the token
				// parsed by strsep
			  	char *arg_cmd_ptr;                                         			                                   
				char *working_cmd_str  = strdup( cmd );
				//printf("%s\n",working_cmd_str); //Debug line
				//Tokenize by white space:
				 while (((arg_cmd_ptr = strsep(&working_cmd_str,WHITESPACE))!=NULL)&&(token_cmd_count<MAX_NUM_ARGUMENTS))
				    {
				      	tokencmd[token_cmd_count] = strndup( arg_cmd_ptr, MAX_COMMAND_SIZE );
				      	if( strlen( tokencmd[token_cmd_count] ) == 0 )
				      	{
						tokencmd[token_cmd_count] = NULL;
				      	}
					token_cmd_count++;
				    }
				/*int token_cmd_index;
				for( token_cmd_index = 0; token_cmd_index < token_cmd_count; token_cmd_index ++ ) 
	    			{
	    			  printf("token[%d] = %s\n", token_cmd_index, tokencmd[token_cmd_index] );  
	    			} */ //== Testing to see if tokens are being seperated correctly
				//THE FOLLOWING CODE IS THE SAME AS WHEN STARTING A NEW PROCESS:
				pid_t ret = fork(); //Fork to create a child
				int status,rete;
				if(ret!=0) //This Conditional in parent process
				{ 
					if(pid_stored == 15) //Pids stored = 15
						{ 
						int j;
						for(j=0;j<14;j++)
						{
							//Move each pid up by one step:
							pids[j] = pids[j+1]; 	
											 
						}
					//Decrement where the pid counter points to store the next value:
						pid_stored--; 
					}
					if(history_stored == 50) //History stored = 50
						{ 
						int k;
						for(k=0;k<49;k++)
						{ 
							strcpy(history[k],history[k+1]); //Move history up by 1 								 
						}
						//Decrement where the history counter points to store the next value:
						history_stored--; 
					}

					//printf("PID = %d\n",ret);   //Used to check PIDS

					pids[pid_stored] = ret; //Copy child process pid to pid array
					strcpy(history[history_stored],cmd); //Copy thr command into history

					//printf("stored = %d\n",pids[pid_stored]); //Use to check if pids are stored correctly in pid array

					pid_stored = pid_stored + 1; //Increase array pointer by 1
					history_stored = history_stored + 1;
					
				}
				else if(ret == 0) //Run the command in child
					{ 
					rete = execvp(tokencmd[0],tokencmd); //Execute the given command
					if(rete==-1)
					{
						printf("%s: Command Not Found.\n",tokencmd[0]);	
						continue;	
					}	
				}
				waitpid(ret,&status,0); //wait for the child to finish executing
	       		     }
		}

		//Case for splitting by semicolon:
		
		else if(token_semi_count>1)
		{
			//printf("IN;\n");//Debug Line
			int i;
			//char* worikingINroot;
			for(i=0;i<token_semi_count;i++)
			{
				    //printf("%d\n",i); //Debug line
				    char *tokenIN[MAX_NUM_ARGUMENTS]; //New token
				    int   tokenINcount = 0;     
				    char *argINptr;                                                           
				    char* workingINstr  = strdup( tokensemi[i] ); //Save the value of string tokenized by ; 
				    // the correct amount at the end
				    char *workingINroot = working_str;
				    // Tokenize the strings with whitespace used as the delimiter
				    while ( ( (argINptr = strsep(&workingINstr, WHITESPACE ) ) != NULL) && 
					      (tokenINcount<MAX_NUM_ARGUMENTS))
				    {
				      tokenIN[tokenINcount] = strndup( argINptr, MAX_COMMAND_SIZE );
				      if( strlen( tokenIN[tokenINcount] ) == 0 )
				      {
					tokenIN[tokenINcount] = NULL;
				      }
					tokenINcount++;
				    }
				pid_t ret = fork(); //Fork to create a child
				int status,rete;
				if(ret!=0) //This Conditional in parent process
				{ 
					if(pid_stored == 15) //Pids stored = 15
						{ 
						int j;
						for(j=0;j<14;j++)
						{
							pids[j] = pids[j+1]; //Move each pid up by one step	
											 
						}
						pid_stored--; //Decrement where the pid counter points to store the next value
					}
					if(history_stored == 50) //History stored = 50
					{ 
						int k;
						for(k=0;k<49;k++){ 
							strcpy(history[k],history[k+1]); //Move each history up									 
						}
						history_stored--; //Decrement where the history counter points to store the next value
					}

					//printf("PID = %d\n",ret);   //Used to check PIDS

					pids[pid_stored] = ret; //Copy child process pid to pid array
					strcpy(history[history_stored],tokensemi[i]); //Copy thr command into history

					//printf("stored = %d\n",pids[pid_stored]); //Use to check if pids are stored correctly in pid array

					pid_stored = pid_stored + 1; //Increase array pointer by 1
					history_stored = history_stored + 1;
					
				}
				else if(ret == 0) //Run the command in child
					{ 
					rete = execvp(tokenIN[0],tokenIN); //Execute the given command
					if(rete==-1){
						printf("%s: Command Not Found.\n",tokenIN[0]);	
						continue;	
					}		
				}	
				waitpid(ret,&status,0); //wait for the child to finish executing
					
			}
			//free (workingINroot);		
			continue;
		}	
		
		//Case for quit/exit:
		else if((strcmp(token[0],"exit")==0)||(strcmp(token[0],"quit")==0))
		{
			//printf("INexit\n");
			exit(0); //end the program
		}

		//FUNCTIONALITY IMPLEMENTED: LISTPIDS:	
		else if(strcmp(token[0],"listpids")==0)
		{ 
			int i;
			for(i=0;i<pid_stored;i++){
				printf("PID %d = %d\n",i+1,pids[i]); //Print all the stored pids		
			}
			
		}

		//FUNCTIONALITY IMPLEMENTED: HISTORY:
		else if(strcmp(token[0],"history")==0)
		{
			int i;
			for(i=0;i<history_stored;i++)
			{
				printf("Command %d = %s\n",i+1,history[i]); //Print all the stored history	
			}
			
		}
		
		//To implement system functionalities:
		else
		{       
			//printf("INsys\n");               
			pid_t ret = fork(); //Fork to create a child
			int status,rete;
			if(ret!=0)  //This Conditional in parent process
			{
				if(pid_stored == 15) //Pids stored = 15
				{ 
					int j;
					for(j=0;j<14;j++)
					{
						pids[j] = pids[j+1]; //Move each pid up by one step	
										 
					}
					pid_stored--; //Decrement where the pid counter points to store the next value
				}
				if(history_stored == 50)
				{ //History stored = 50
					int k;
					for(k=0;k<49;k++){ 
						strcpy(history[k],history[k+1]); //Move each history up									 
					}
					history_stored--; //Decrement where the history counter points to store the next value
				}

				//printf("PID = %d\n",ret);   //Used to check PIDS

				pids[pid_stored] = ret; //Copy child process pid to pid array
				strcpy(history[history_stored],cmd_str); //Copy thr command into history

				//printf("stored = %d\n",pids[pid_stored]); //Use to check if pids are stored correctly in pid array

				pid_stored = pid_stored + 1; //Increase array pointer by 1
				history_stored = history_stored + 1;
				
			}
			else if(ret == 0)
			{ //Run the command in child
				rete = execvp(token[0],token); //Execute the given command
				//printf("Exec = %d\n",rete); //This line to check if Exec Works; Exec = -1 means exec is not working		
			}	
			waitpid(ret,&status,0); //wait for the child to finish executing
			if(rete==-1)
			{
				printf("%s: Command Not Found.\n",token[0]);		
			}
		}
	    // Now print the tokenized input as a debug check
	    /*int token_index  = 0;
	    for( token_index = 0; token_index < token_count; token_index ++ ) 
	    {
	      printf("token[%d] = %s\n", token_index, token[token_index] );  
	    }*/ //== Testing to see if tokens are being seperated correctly

	    free( working_root );
	    
	  }
  return 0;
}
