
/*Christopher Torralba
  Jason Tracy
  Josh Zygmunt        	*/

  #include "REG932.h"
/*
  #include "uart.c"
  #include "uart.h"
*/

  //P0 = 0xFF;

  sbit upleftLED = P2^4;
  sbit upmidLED = P0^5;
  sbit uprightLED = P2^7;
  sbit midleftLED = P0^6;
  sbit midmidLED = P1^6;
  sbit midrightLED = P0^4;
  sbit downleftLED =  P2^5;
  sbit downmidLED = P0^7;
  sbit downrightLED = P2^6;

  sbit upleftINPUT = P2^0;
  sbit upmidINPUT = P0^1;
  sbit uprightINPUT = P2^3;
  sbit midleftINPUT = P0^2;
  sbit midmidINPUT = P1^4;
  sbit midrightINPUT = P0^0;
  sbit downleftINPUT = P2^1;
  sbit downmidINPUT = P0^3;
  sbit downrightINPUT = P2^2;
  
  void Delay()
  {
    TMOD = 0x01;
	TLO = 0x78;
	THO = 0xFF`;
	TRO = 1;
	while(TFO == 0);
	TRO = 0;
	TFO = 0;
  }
  
  bit GetInput(char Grid[3][3])
  {
    bit ans = 1;
	if(upleftINPUT == 0 && Grid[0][0] == 2) //you need to check if the slot is open
	{
	  upleftLED = 0;
	  Grid[0][0] = 0;
	}
	else if(upmidINPUT == 0 && Grid[0][1] == 2)
	{
	  upmidLED = 0;
	  Grid[0][1] = 0;
	}
	else if(uprightINPUT == 0 && Grid[0][2] == 2)
	{
	  uprightLED = 0;
	  Grid[0][2] = 0;
	}
	else if(midleftINPUT == 0 && Grid[1][0] == 2)
	{
	  midleftLED = 0;
	  Grid[1][0] = 0;
	}
	else if(midmidINPUT == 0 && Grid[1][1] == 2)
	{
	  midmidLED = 0;
	  Grid[1][1] = 0;
	}
	else if(midrightINPUT == 0 && Grid[1][2] == 2)
	{
	  midrightLED = 0;
	  Grid[1][2] = 0;
	}
	else if(downleftINPUT == 0 && Grid[2][0] == 2)
	{
	  downleftLED = 0;
	  Grid[2][0] = 0;
	}
	else if(downmidINPUT == 0 && Grid[2][1] == 2)
	{
	  downmidLED = 0;
	  Grid[2][1] = 0;
	}
	else if(downrightINPUT == 0 && Grid[2][2] == 2)
	{
	  downrightLED = 0;
	  Grid[2][2] = 0;
	}
	else
	  ans = 0;
	return ans;
  }
 

 void Flash(char Grid[3][3])//not complete needs delay
 {
	if(Grid[0][0] == 1)
	  upleftLED = 1;
	if(Grid[0][1] == 1)
	  upmidLED = 1;
	if(Grid[0][2] == 1)
	  uprightLED = 1;
	if(Grid[1][0] == 1)
	  midleftLED = 1;
	if(Grid[1][1] == 1)
	  midmidLED = 1;
	if(Grid[1][2] == 1)
	  midrightLED = 1;
	if(Grid[2][0] == 1)
	  downleftLED = 1;
	if(Grid[2][1] == 1)
	  downmidLED = 1;
	if(Grid[2][2] == 1)
	  downrightLED = 1;
	//delay
	Delay();
	
	if(Grid[0][0] == 1)
	  upleftLED = 0;
	if(Grid[0][1] == 1)
	  upmidLED = 0;
	if(Grid[0][2] == 1)
	  uprightLED = 0;
	if(Grid[1][0] == 1)
	  midleftLED = 0;
	if(Grid[1][1] == 1)
	  midmidLED = 0;
	if(Grid[1][2] == 1)
	  midrightLED = 0;
	if(Grid[2][0] == 1)
	  downleftLED = 0;
	if(Grid[2][1] == 1)
	  downmidLED = 0;
	if(Grid[2][2] == 1)
	  downrightLED = 0;
 }
 
void TickTackToeHard(char Grid[3][3], bit InputGrid[3][3], bit OutputGrid[3][3]) //in this function 0 represents players 0's
{             	//1's represents the ai's X's and 2's are empty spaces   	
   char i;
   char j;
   char move_num = 0;
   for(i = 0; i < 3; i++)
   {
 	for(j = 0; j < 3; j++)
	Grid[i][j] = 2;
   }
   while(!GetInput(Grid));
   if(Grid[1][1] == 2)
     Grid[1][1] = 1;
   else
     Grid[0][0] = 1;
   while(!GetInput(Grid))
     Flash(Grid);
   //while(move_num < 8)
   //{
	//ai’s move insert here/ai’s win condition
  	//while(!GetInput(Grid))
  	//{
      Flash(Grid);
  	//}
  	//move_num = move_num + 2; //account for player’s and ai’s move
        	//check for win condition insert here
   //}
} 

void main (void)
{
  char Grid[3][3]; ///y x
  bit InputGrid[3][3];
  bit OutputGrid[3][3];
  int i;
  int j;

  P2M1 = 0x00;
  P1M1 = 0x00;
  P0M1 = 0x00;
  for(i = 0; i < 3; i++)
  {
    for(j = 0; j < 3; j++)
	{
	  OutputGrid[i][j] = 1;
	  InputGrid[i][j] = 1;
	}
  }
  
  while(1)
  {
  	//add input methoad to add different modes
  	TickTackToeHard(Grid, InputGrid, OutputGrid);
  	for(i = 0; i < 3; i++)
    {
      for(j = 0; j < 3; j++)
	    OutputGrid[i][j] = 1;
    }
  }
}


