#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
struct instruction
{
   int type;
   int current_state;
   int arrival_clock;
   int destination;
   int source_1; 
   int source_2;
   int valid;
   int IF_time;
   int ID_time;
   int IS_time;
   int EX_time;
   int WB_time;
   int timer;
   int status;
};
struct instruction ROB[10000];
struct Register_Table
{
   int tag;
   int valid;
   int value;
};
int instruction_over;
struct Register_Table Register[128];
char *file;      
FILE * fp;
int clock;
long int PC;
int instruction_type;
int destination;
int source_1;
int source_2;
int i;
int fetch_counter;
   int input_over;
int dispatch_list_counter,issue_list_counter,execute_list_counter,queue_size,fetch_size; 
int counter=0;
struct dispatch_list
{  int tag;
   int current_state;  
   struct dispatch_list* next;   
};

struct issue_list
{
   int tag;
   int type;
   int current_state;
   int destination;
   int source_1_status;
   int source_1_ready;
   int source_2_status;
   int source_2_ready;
   struct issue_list* next;
   int status;
};
struct execute_list
{
  int tag;
  int timer;
  struct execute_list* next;

};
struct execute_list* execute_head;
struct dispatch_list* dispatch_head;
struct dispatch_list* temp_list;
struct issue_list* issue_head;   
void print(struct dispatch_list* ptr)
  {
	struct dispatch_list* current;
	current=ptr;
        if (current==NULL) {
                    //printf("Dispatch queue is empty");
        }
	while ( current!=NULL)
	{
		//printf("%d %d\t",current->tag,current->current_state);
		current=current->next;
	}
        //printf("\n");
   }
void print_issue(struct issue_list* ptr)
  {
	struct issue_list* current;
	current=ptr;
        if (current==NULL) {
                   //printf("Issue queue is empty\n");
        }
	while ( current!=NULL)
	{
		//printf("Tag %d actual dest %d  renamed dest %d actual source_1 %d  renamed source_1 %d  Ready_1 %d  actual source_2 %d  renamed source_2 %d Ready_2 %d Ready %d\n",current->tag,ROB[current->tag].destination,current->destination,ROB[current->tag].source_1,current->source_1_status,current->source_1_ready,ROB[current->tag].source_2,current->source_2_status,current->source_2_ready,current->status);
		current=current->next;
	}
   }
 void print_execute(struct execute_list* ptr)
  {
	struct execute_list* current;
	current=ptr;
        if (current==NULL) {
              //printf("Execute queue is empty");
        }
	while ( current!=NULL)
	{
		//printf("%d %d \t",current->tag,current->timer);
		current=current->next;
	}
           //printf("\n");
   } 
void FakeRetire()
{
     for(i=0;i<input_over;i++)
        {  if(ROB[i].current_state==4)
             {   // if(i==56)
               //   //printf("fuck you \n");
                  ROB[i].status=1;
                  instruction_over=i;
           } 
        }
}
void Execute()
{
 //printf("In Execute Clock is %d #####################################\n",clock);             
    struct execute_list* newnode;
    struct execute_list* newnode2;
    struct issue_list* temp;
    temp=issue_head;
    newnode=execute_head;
    while(newnode!=NULL)    
    {
             if(newnode->timer==0)
             {   //printf("Before Deleting\t");
           //      //print_execute(execute_head);
              
          //       //printf("Instruction %d completed execution deleting from queue\n",newnode->tag);
                 ROB[newnode->tag].WB_time=clock;
                 ROB[newnode->tag].current_state=4;
                 execute_list_counter--; 
                 ROB[newnode->tag].status=1;
		         while(temp!=NULL)
                 {                  
                                   
                            if(temp->source_1_status==newnode->tag)
                          {     
                                temp->source_1_ready=1;                                                            
                          }  
                          if(temp->source_2_status==newnode->tag)
                          {       temp->source_2_ready=1;
                                 
                           }
                                 temp=temp->next;
                 }
                    temp=issue_head;
                         if(newnode==execute_head)
                         {
                         
                               execute_head=execute_head->next;   
                    
                         }
           
                         else 
                         {    
                                      newnode2->next=newnode->next;
                                      
                                //      //printf("After deleting");
                                  //    //print_execute(execute_head);
                         
                         }
                         newnode=newnode->next; 
                 }
                 else
               {  newnode->timer--;
                        newnode2=newnode; 
                  newnode=newnode->next; 
           }
     
          
       //  //printf("newnode2 %d newnode %d\n",newnode2->tag,newnode->tag);
    }
  //printf("Out Execute \n"); 
       //printf("After deleting");
      //print_execute(execute_head);
        //printf("Exec_list counter %d \n",execute_list_counter); 















}

void Issue()
{   int issue_counter=0;
   //printf("In Issue \n");             
    struct issue_list* newnode;
    struct issue_list* newnode2;
    struct issue_list* temp;
    newnode=issue_head;
    while(newnode!=NULL)    
    {
             if(newnode->source_1_ready==1 && newnode->source_2_ready==1 && issue_counter<fetch_size)
             {   newnode->status=1;
                 ROB[newnode->tag].current_state=3;
                 ROB[newnode->tag].EX_time=clock;
                 execute_list_counter++; 
                 issue_counter++;
             
		         struct execute_list* newnode1;
	             struct execute_list* current;
	             newnode1=malloc(sizeof (struct execute_list));
                 newnode1->tag=newnode->tag;
                 if(ROB[newnode->tag].type==0)
                 newnode1->timer=0;
                 else if(ROB[newnode->tag].type==1)  
                 newnode1->timer=1;
                 else if(ROB[newnode->tag].type==2)
                 newnode1->timer=4; 
                 newnode1->next=NULL;
 
                         if(execute_head==NULL)
	                     {      
		                 execute_head=newnode1;
                             }
	                     else
	                     {   
		          			current=execute_head;
                            while (current->next!=NULL)
		                    { 
				                     current=current->next;
		      	            }
	 	       			    current->next=newnode1;            
		
     	                      }
                           issue_list_counter--;
                         if(newnode==issue_head)
                         {
                             temp=newnode;
                             issue_head=issue_head->next;   
                          //   free(temp);
                         }
                                  else 
                         {    
                                      newnode2->next=newnode->next;
                                      
                                //      //printf("After deleting");
                                  //    //print_execute(execute_head);
                         
                         }
                         newnode=newnode->next; 
                 }
                 else
               { // newnode->timer--;
                        newnode2=newnode; 
                  newnode=newnode->next; 
           }
     
    }
       //printf("Out Issue\n"); 
      

}

void Dispatch()
{   
  //printf("In Dispatch \n");  
         //printf("Exec Queue\n");
         //print_execute(execute_head);         
    struct dispatch_list* newnode;
    struct dispatch_list* temp;
    newnode=dispatch_head;
    while(newnode!=NULL)    
    {
             if(newnode->current_state==0)
             {   
                 ROB[newnode->tag].IF_time=clock-1;
                 ROB[newnode->tag].current_state=1;
                 newnode->current_state=1;
             } 
             else if(newnode->current_state==1)
             {  
                 ROB[newnode->tag].ID_time=  ROB[newnode->tag].IF_time+1;
                
                 if(issue_list_counter<queue_size)
                 { 
                      issue_list_counter++; 
		      struct issue_list* newnode1;
	              struct issue_list* current;
	              newnode1=malloc(sizeof (struct issue_list));
                      newnode1->tag=newnode->tag;
                      ROB[newnode->tag].current_state=2;
                      newnode1->current_state=2; 
                      ROB[newnode->tag].IS_time=clock;
                      newnode1->next=NULL;
                      newnode1->status=1;
                      newnode1->type=ROB[newnode1->tag].type;  
                         if(ROB[newnode->tag].source_1!=-1)
                         {
                                if(Register[ROB[newnode->tag].source_1].tag!=-1)
                                {           if(ROB[Register[ROB[newnode->tag].source_1].tag].status!=1)
                                         {   newnode1->source_1_status=Register[ROB[newnode->tag].source_1].tag;      
                                            newnode1->source_2_ready=0; 
                                            newnode1->status=0;
                                         }
                                          else
                                            newnode1->source_1_ready=1;
                                }
                                   else
                                            newnode1->source_1_ready=1;

                         }
                                 else
                                            newnode1->source_1_ready=1;

                        
                      if(ROB[newnode->tag].source_2!=-1)
                         {
                                if(Register[ROB[newnode->tag].source_2].tag!=-1)
                                 {         if(ROB[Register[ROB[newnode->tag].source_2].tag].status!=1)
                                         { 
                                             newnode1->source_2_status=Register[ROB[newnode->tag].source_2].tag;      
                                             newnode1->source_2_ready=0;
                                             newnode1->status=0; }
                                             else
                                            newnode1->source_2_ready=1;
                                 }
                                 else
                                            newnode1->source_2_ready=1;
                         }
                         else
                         newnode1->source_2_ready=1;
                         if(ROB[newnode->tag].destination!=-1) 
                         {
                            Register[ROB[newnode->tag].destination].tag=newnode->tag;
                            newnode1->destination=Register[ROB[newnode->tag].destination].tag;

                         }  
                         else
                         newnode1->destination=-1;
                         if(issue_head==NULL)
	                     {      
		                 issue_head=newnode1;
                         }
	                     else
	                     {   
		          			current=issue_head;
                            while (current->next!=NULL)
		                    { 
				                     current=current->next;
		      	            }
	 	       			    current->next=newnode1;            
		
     	                 }
                         if(newnode==dispatch_head)
                         {
                             temp=newnode;
                             dispatch_head=dispatch_head->next;   
                             free(temp);
                         }
  //                      
                  
                         dispatch_list_counter--;
                 }
                
                 
           } 
          newnode=newnode->next; 
    }
     //printf("Out Dispatch\n"); 
       //printf("Issue_list counter %d \n",issue_list_counter);
}

int Advance_cycle()
{  
 //printf("In Advance \n");
 //printf("Dispatch Queue\n");
 //print(dispatch_head);
 //printf("Out Advance \n");
 if(clock==10000)
 return 0;
 else
 return 1;
} 
void fetch()
{        //printf("In Fetch \n");  
         //printf("Issue Queue\n");
         //print_issue(issue_head);
        
         int fetch_count = fetch_size;   
         while(fetch_count!=0 && dispatch_list_counter!=2*fetch_size && counter!=input_over)
                 {

                  fetch_count--;   
                  dispatch_list_counter++; 
		  struct dispatch_list* newnode;
	              struct dispatch_list* current;
	              newnode=malloc(sizeof (struct dispatch_list));
                  if(newnode==NULL)
                  return;
                  newnode->tag=counter; 
                  newnode->next=NULL;
                  newnode->current_state=0;
                  if(dispatch_head==NULL)
	              {
		                dispatch_head=newnode;
                      }
	              else
	              {
		                  current=dispatch_head;
                          while (current->next!=NULL)
		                  {
							current=current->next;
		      	          }
	 	       			  current->next=newnode;            
		
     	             }
                  counter++; 
              
                 }
                 //printf("Out of Fetch \n");   
 
}
 int main(int argc, char *argv[])
{         
	   if (argc != 4) 
           {
           //printf("Error message - Agruments invalid\n");  
           return -1;
	   }   
           clock=0;
           dispatch_list_counter=0; 
           execute_list_counter=0;
           issue_list_counter=0;
           queue_size=atoi(argv[1]);
           fetch_size=atoi(argv[2]);
           file=argv[3];
           fp = fopen (file,"r"); 
           for(i=0;i<128;i++)
            Register[i].tag=-1;  
            instruction_over=0;
            while(fscanf(fp,"%lx %d %d %d %d",&(PC),&(instruction_type),&(destination),&(source_1),&(source_2))==5) 
	{ 	  fgetc(fp);
                 
                  
                  ROB[counter].destination=destination;
                  ROB[counter].type=instruction_type;
                  ROB[counter].source_1=source_1;
                  ROB[counter].source_2=source_2;
                  ROB[counter].current_state=0;
                  if(ROB[counter].type==0)
                  ROB[counter].timer=1;
                  else if(ROB[counter].type==1)  
                  ROB[counter].timer=2;
                  else if(ROB[counter].type==2)
                  ROB[counter].timer=5;          
                  counter++;
                  
             
         }
           input_over=counter;
           counter=0;
           do
          {
           FakeRetire();
           Execute();
           Issue();
           Dispatch();
           fetch();
           clock++;
           }
           while(Advance_cycle());
 
           for(i=0;i<counter;i++)      
                 { 
                  // //printf(" %d fu{%d} src{%d,%d} dst{%d} IF{%d,1} ID{%d,%d} IS{%d,%d} EX{%d,%d} WB{%d,1}\n "/*current_state = %d and arrival time is %d type %d status %d\n"*/,i,ROB[i].type,ROB[i].source_1,ROB[i].source_2,ROB[i].destination,ROB[i].IF_time,ROB[i].ID_time,ROB[i].IS_time-ROB[i].ID_time,ROB[i].IS_time,ROB[i].EX_time-ROB[i].IS_time,ROB[i].EX_time,ROB[i].timer,ROB[i].WB_time/*,ROB[i].current_state,ROB[i].arrival_clock,ROB[i].type,ROB[i].status*/);
                    printf(" %d fu{%d} src{%d,%d} dst{%d} IF{%d,1} ID{%d,%d} IS{%d,%d} EX{%d,%d} WB{%d,1}\n"/*current_state = %d and arrival time is %d type %d\n"*/,i,ROB[i].type,ROB[i].source_1,ROB[i].source_2,ROB[i].destination,ROB[i].IF_time,ROB[i].ID_time,ROB[i].IS_time-ROB[i].ID_time,ROB[i].IS_time,ROB[i].EX_time-ROB[i].IS_time,ROB[i].EX_time,ROB[i].timer,ROB[i].WB_time/*,ROB[i].current_state,ROB[i].arrival_clock,ROB[i].type*/);
               }
            return 0;
		
} 
