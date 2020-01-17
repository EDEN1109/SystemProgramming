#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>

#define NAMELENGTH 7  
#define NROOMS 10  

char namebuf[NAMELENGTH];
int infile;;
off_t offset;
ssize_t nread;


char *getoccupier(int roomno)
{
	 infile = -1;
	 if(infile == -1 && (infile = open("residents", O_RDONLY)) == -1) 
		 return (NULL);
				   
	 offset = (roomno -1) *NAMELENGTH; 
		 
	 if(lseek(infile, offset, SEEK_SET) == -1)  
		 return (NULL);
	 if((nread = read(infile, namebuf, NAMELENGTH)) <= 0)
		 return (NULL);

	 namebuf[nread -1] = '\0';   
	 return (namebuf);   
}

void deloccupier(int roomno)
{
	 infile = -1;
	 if(infile == -1 && (infile = open("residents", O_RDWR)) == -1)
		 return ;
		 
	 offset = (roomno-1)*NAMELENGTH;  
		
	 if(lseek(infile, offset, SEEK_SET) == -1)
		 return ;	  
	 else
	 {
		 for(int i=0; i<NAMELENGTH-1; i++)
			 namebuf[i] = ' ';
		 
						        
		 namebuf[NAMELENGTH-1] = '\0';
		
		 write(infile, namebuf,NAMELENGTH);
	 }
	 close(infile);  
}

void addoccupier(int roomno,char *p)
{
	 infile = -1;
	 if(infile == -1 && (infile = open("residents", O_RDWR)) == -1)
		 return ;
	 offset = (roomno-1)*NAMELENGTH;
		  
	 if(lseek(infile, offset, SEEK_SET) == -1) 
		 return ;
	 if((nread = read(infile, namebuf, NAMELENGTH)) < 0)
		 return ;
	 if(lseek(infile, -NAMELENGTH, SEEK_CUR) == -1) 
		 return ;
			 
	 if(namebuf[0] == ' ')     
	 {
		 p[NAMELENGTH-1] = '\0';   
		 write(infile, p,NAMELENGTH);  
	 }
	 else       
		 printf("This room is Full.\n");
	 close(infile);
}


int main()
{
	int select, room;  
	char *getoccupier (int), *p ; 
	char name[NAMELENGTH];  
	while(select != 4)
	{ 
		printf("================\n");
		printf(" 1. Occupier Info\n");
		printf(" 2. Delete Occupier\n");
		printf(" 3. Add Occupier\n");
		printf(" 4. Quit\n");
		printf("================\n");
		printf("Select the Menu >>");
							 
		scanf("%d",&select);  

		if(select == 1)
		{
				for(int j=1; j<=NROOMS; j++)
				{
					if(p=getoccupier(j))
						printf("Room %2d, %s\n", j, p);
					else
						printf("Error on room %d\n",j); 
				}
		}
		else if(select == 2)
		{
			printf("Which room would be delete? >>");
			scanf("%d", &room);
			deloccupier(room); 
		}
		else if(select == 3)
		{
			printf("Which room you want? >>"); 
			scanf("%d", &room);
			printf("Enter the Name >>"); 
			scanf("%s", name);
            
            for(int i=0; i<NAMELENGTH-1; i++)
            {
                    if(!isalpha(name[i]))
                            name[i] = ' ';
            }
            name[NAMELENGTH-1] = '\0';

			addoccupier(room,name);
		}
		else if(select == 4)	
			break;
		else
			printf("Please, Enter the Number 1 to 4\n");
	}
	close(infile);
	return 0;
}
