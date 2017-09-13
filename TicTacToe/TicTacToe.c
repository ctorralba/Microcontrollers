
/*Christopher Torralba
  Jason Tracy
  Josh Zygmunt            */

  #include "REG932.h"
  #include <stdlib.h>
  #include "uart.h"

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

  sbit Chip_enable = P1^3;

  sbit sound = P1^7;
  char highPreload;
  char lowPreload;
  /*In our 3x3 Grid for tic-tac-toe we use 0’s to represent the player’s movements,
	 1’s to represent the AI’s movements, and 2’s to represent empty spaces.
   Our tic-tac-toe board is represented as a 2-D Array [0][0], [0][1], [0][2]
																											 [1][0], [1][1]. [1][2]
																											 [2][0], [2][1], [2][2]
  */

  void set()  //used in timed tic-tac-toe mode for calculating the time needed before the AI goes instead of the player
  {
    TF0 = 0;
    TR0 = 0;
    TMOD = 1;
    TL0 = 0xFF;
    TH0 = 0x6F;
    TR0 = 1;
  }

  void GoRand(char Grid[3][3], const char c) // this is used in all of the ai functionality for an AI to place randomly
  {
    char x = 0;
    char y = 0;
    while(Grid[y][x] != 2) //checks if the random variable between 0-2 is empty
    {
      x = rand() % 3; //random variable between 0-2
      y = rand() % 3;
    }
    Grid[y][x] = c; //write whether a player or an ai used this
  }  

  void Delay()  //used for the delay between the flashing LED’s for the AI’s representation
  {
    char i = 0;
    while(i <= 50) //iterate this 50 times
    {
      TF0 = 0;
      TR0 = 0;
      TMOD = 1;
      TL0 = 0x99;
      TH0 = 0xF1;   //.001*50=.05
      TR0 = 1;
      while(TF0 == 0);
      i++;
    }
  }
 
 
  bit GetInput(char Grid[3][3]) // function is used for the player to input a valid spot
  {
    bit ans = 1;
    if(upleftINPUT == 0 && Grid[0][0] == 2) //you need to check if the slot is open
    {
      upleftLED = 0; //lights up the LED according to the player’s input
      Grid[0][0] = 0; //used to represent our tic-tac-toe board.
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

  bit Get2PlayerInput(char Grid[3][3]) //this is used to get the second player input for the 2-player mode
  {
    bit ans = 1;
    if(upleftINPUT == 0 && Grid[0][0] == 2)
      Grid[0][0] = 1; //this is usually represented as AI
    else if(upmidINPUT == 0 && Grid[0][1] == 2)
      Grid[0][1] = 1;
    else if(uprightINPUT == 0 && Grid[0][2] == 2)
      Grid[0][2] = 1;
    else if(midleftINPUT == 0 && Grid[1][0] == 2)
      Grid[1][0] = 1;
    else if(midmidINPUT == 0 && Grid[1][1] == 2)
      Grid[1][1] = 1;
    else if(midrightINPUT == 0 && Grid[1][2] == 2)
      Grid[1][2] = 1;
    else if(downleftINPUT == 0 && Grid[2][0] == 2)
      Grid[2][0] = 1;
    else if(downmidINPUT == 0 && Grid[2][1] == 2)
      Grid[2][1] = 1;
    else if(downrightINPUT == 0 && Grid[2][2] == 2)
      Grid[2][2] = 1;
    else
      ans = 0;
    return ans;
  }

bit setup(char Grid[3][3])//this was used for the hard AI mode & hard timed mode
{//this function specifically looks for a row/column that has 2 empty spaces and one AI move so that next turn if the player doesn't block it the AI will win if there is no possible setup returns 0
  char AiMoves;  //counts number of ai moves in a row               
  char empty; //counters number of empty spaces
  char x, y; //these are one of the emptyspace’s cordinates
  char i, j; //iterators used for loops
  //uart_transmit('A');
  for(i = 0; i < 3; i++) //check all across
  {
    AiMoves = 0;
    empty = 0;
    for(j = 0; j < 3; j++)
    {
      if(Grid[i][j] == 2)
      {
        empty++;
        x = j;
        y = i;
      }
      else if(Grid[i][j] == 1)
        AiMoves++;
    }
    if(AiMoves == 1 && empty == 2)
    {
      Grid[y][x] = 1;
      return 1;
    }
  }

  for(j = 0; j < 3; j++) //check all down
  {
    AiMoves = 0;
    empty = 0;
    for(i = 0; i < 3; i++)
    {
      if(Grid[i][j] == 2)
      {
        empty++;
        x = j;
        y = i;
      }
      else if(Grid[i][j] == 1)
        AiMoves++;
    }
    if(AiMoves == 1 && empty == 2)
    {
      Grid[y][x] = 1;
      return 1;
    }
  }

  AiMoves = 0;
  empty = 0;
  for(j = 0, i = 0; j < 3; j++, i++)//checking diagonally
  {
    if(Grid[i][j] == 2)
      {
        empty++;
        x = j;
        y = i;
      }
      else if(Grid[i][j] == 1)
        AiMoves++;
  }

  if(AiMoves == 1 && empty == 2)
  {
      Grid[y][x] = 1;
      return 1;
  }

  AiMoves = 0;
  empty = 0;
  for(j = 2, i = 0; j >= 0; j--, i++)//checking diagonally
  {
    if(Grid[i][j] == 2)
      {
        empty++;
        x = j;
        y = i;
      }
      else if(Grid[i][j] == 1)
        AiMoves++;
  }
  if(AiMoves == 1 && empty == 2)
  {
      uart_transmit('b');
      Grid[y][x] = 1;
      return 1;
  }
  //uart_transmit('B');
  return 0;
}

bit winingmove(char Grid[3][3])  /* used in hard ai mode and hard ai timed mode, checks to see if there is a winning move and if there is takes it, if there isn’t returns a 0*/
{                                         
  char AiMoves; //number of ai moves in a row or column                 
  char empty; //number of empty spaces in a row or column
  char x, y; //stores the coordinate of a empty spot on the grid
  char i, j; //iterators used in loops
  //uart_transmit('C');
  for(i = 0; i < 3; i++) //check all across
  {
    AiMoves = 0;
    empty = 0;
    for(j = 0; j < 3; j++)
    {
      if(Grid[i][j] == 2)
      {
        empty++;
        x = j;
        y = i;
      }
      else if(Grid[i][j] == 1)
        AiMoves++;
    }
    if(AiMoves == 2 && empty == 1)
    {
      Grid[y][x] = 1;
      return 1;
    }
  }

  for(j = 0; j < 3; j++) //check all down
  {
    AiMoves = 0;
    empty = 0;
    for(i = 0; i < 3; i++)
    {
      if(Grid[i][j] == 2)
      {
        empty++;
        x = j;
        y = i;
      }
      else if(Grid[i][j] == 1)
        AiMoves++;
    }
    if(AiMoves == 2 && empty == 1)
    {
      Grid[y][x] = 1;
      return 1;
    }
  }

  AiMoves = 0;
  empty = 0;
  for(j = 0, i = 0; j < 3; j++, i++) //checks diagonally
  {
    if(Grid[i][j] == 2)
      {
        empty++;
        x = j;
        y = i;
      }
      else if(Grid[i][j] == 1)
        AiMoves++;
  }

  if(AiMoves == 2 && empty == 1)
  {
      Grid[y][x] = 1;
      return 1;
  }

  AiMoves = 0;
  empty = 0;
  for(j = 2, i = 0; j >= 0; j--, i++) //checks diagonally
  {
    if(Grid[i][j] == 2)
      {
        empty++;
        x = j;
        y = i;
      }
      else if(Grid[i][j] == 1)
        AiMoves++;
  }
  if(AiMoves == 2 && empty == 1)
  {
      Grid[y][x] = 1;
      return 1;
  }
  //uart_transmit('D');
  return 0;
}

bit Defend(char Grid[3][3]) //checks if the human player is about to win
{                                         //if they are the ai blocks it
                                          //returns 1 if the move is blocked 0 if there is no “obvious” block
  char playerM; //number of player moves in a row or column
  char empty; //number of empty spots in a row or column
  char x, y; //x and y coordinate of the empty space is a row or column
  char i, j; //iterators used in loops
  //uart_transmit('E');
  for(i = 0; i < 3; i++) //check all across
  {
    playerM = 0;
    empty = 0;
    for(j = 0; j < 3; j++)
    {
        if(Grid[i][j] == 2)
        {
        empty++;
        x = j;
        y = i;
        }
        else if(Grid[i][j] == 0)
        playerM++;
    }
    if(playerM == 2 && empty == 1)
    {
        Grid[y][x] = 1;
        return 1;
    }
    
  }
 
  for(j = 0; j < 3; j++) //check all down
  {
    playerM = 0;
    empty = 0;
    for(i = 0; i < 3; i++)
    {
      if(Grid[i][j] == 2)
      {
        empty++;
        x = j;
        y = i;
      }
      else if(Grid[i][j] == 0)
        playerM++;
    }
    if(playerM == 2 && empty == 1)
    {
      Grid[y][x] = 1;
      return 1;
    }
  }
 
  playerM = 0;
  empty = 0;
  for(j = 0, i = 0; j < 3; j++, i++) //checking diagonally
  {
    if(Grid[i][j] == 2)
      {
        empty++;
        x = j;
        y = i;
      }
      else if(Grid[i][j] == 0)
        playerM++;
  }
 
  if(playerM == 2 && empty == 1)
  {
      Grid[y][x] = 1;
      return 1;
  }
 
  playerM = 0;
  empty = 0;
  for(j = 2, i = 0; j >= 0; j--, i++)//checking diagonally
  {
    if(Grid[i][j] == 2)
      {
        empty++;
        x = j;
        y = i;
      }
      else if(Grid[i][j] == 0)
        playerM++;
  }
 
  if(playerM == 2 && empty == 1)
  {
      Grid[y][x] = 1;
      return 1;
  }
  //uart_transmit('F');
  return 0;
}
 void Reset(char Grid[3][3]) //use to reset the board by clearing all the LED’s and writing 2’s to our board
 {
  char i, j;
  upleftLED = 1;
  upmidLED = 1;
  uprightLED = 1;
  midleftLED = 1;
  midmidLED = 1;
  midrightLED = 1;
  downleftLED = 1;
  downmidLED = 1;
  downrightLED = 1;
  for (i = 0; i < 3; i++)
  {
    for (j=0; j<3; j++)
      Grid[i][j] = 2;
  }
 }
 void Flash(char Grid[3][3])//used to represent the second player input
 {                                        //as flashing LEDs
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
    Delay();
 }

  void GetInputTimed(char Grid[3][3]) /* used to time the player in HardTimed mode if the player doesn't make a move in the allowed time this function will skip the player's input for the round*/
{
   int w = 0;
   TF0 = 1;
   while(!GetInput(Grid))
   {
     Flash(Grid);
     if(TF0 == 1)
     {
       set(); //sets up the timer
       w++;
     }
     if(w == 50)
       break;
   }
  TR0 = 0;
}

 char endCondition (char Grid[3][3]) /*this is used to check who won the tic-tac-toe game, if the game is still going on, if there’s a tie etc */
{
    if ((Grid[0][0] == 0 && Grid[0][1] == 0 && Grid[0][2] == 0) ||
       (Grid[1][0] == 0 && Grid[1][1] == 0 && Grid[1][2] == 0) ||
       (Grid[2][0] == 0 && Grid[2][1] == 0 && Grid[2][2] == 0) ||
       (Grid[0][0] == 0 && Grid[1][0] == 0 && Grid[2][0] == 0) ||
       (Grid[0][1] == 0 && Grid[1][1] == 0 && Grid[2][1] == 0) ||
       (Grid[0][2] == 0 && Grid[1][2] == 0 && Grid[2][2] == 0) ||
         (Grid[0][0] == 0 && Grid[1][1] == 0 && Grid[2][2] == 0) ||
         (Grid[0][2] == 0 && Grid[1][1] == 0 && Grid[2][0] == 0))
        return 1; //player won
    else if ((Grid[0][0] == 1 && Grid[0][1] == 1 && Grid[0][2] == 1) ||
       (Grid[1][0] == 1 && Grid[1][1] == 1 && Grid[1][2] == 1) ||
       (Grid[2][0] == 1 && Grid[2][1] == 1 && Grid[2][2] == 1) ||
       (Grid[0][0] == 1 && Grid[1][0] == 1 && Grid[2][0] == 1) ||
       (Grid[0][1] == 1 && Grid[1][1] == 1 && Grid[2][1] == 1) ||
       (Grid[0][2] == 1 && Grid[1][2] == 1 && Grid[2][2] == 1) ||
         (Grid[0][0] == 1 && Grid[1][1] == 1 && Grid[2][2] == 1) ||
         (Grid[0][2] == 1 && Grid[1][1] == 1 && Grid[2][0] == 1))
         return 2; //ai won
    else if (Grid[0][0] != 2 && Grid[0][1] != 2 && Grid[0][2] != 2 &&
         Grid[1][0] != 2 && Grid[1][1] != 2 && Grid[1][2] != 2 &&
         Grid[2][0] != 2 && Grid[2][1] != 2 && Grid[2][2] != 2)
        return 3; //tie
      else
        return 0; //error
}

char TickTackToeHard(char Grid[3][3]) //in this function 0 represents players 0's
{                 //1's represents the ai's X's and 2's are empty spaces
   while(!GetInput(Grid)); //1st move
   if(Grid[1][1] == 2) //2nd move
     Grid[1][1] = 1;
   else
     Grid[0][0] = 1;

   while(!GetInput(Grid)) //3rd move
     Flash(Grid);

  if(!Defend(Grid))//4th move
 {
  if(Grid[1][1] == 1)
  {
  if(Grid[1][0] == 2)
    Grid[1][0] = 1;
  else if(Grid[1][2] == 2)
    Grid[1][2] = 1;
  else
    Grid[0][1] = 1;
  }
  else setup(Grid);
 }
  while(!GetInput(Grid)) //5th move
    Flash(Grid);
   if(endCondition(Grid) != 0) //checks to see if by this point the player has won the game
    return endCondition(Grid);
   while(1)
   {
    if(winingmove(Grid)); //if there is a winning move the AI takes it
    else if(Defend(Grid)); //if there is not a winning move the AI looks to see if it has to defend
    else if (setup(Grid)); //if there is no point to defend the AI will try to set up a winning move
    else
      GoRand(Grid, 1); //if there is no setup move the AI will move randomly
    
    if(endCondition(Grid) != 0)  //checks to see if the game is still going on
        return endCondition(Grid); //if the game isn't still going on it returns who won
    
    while(!GetInput(Grid)) //getting player input and flashing grid while waiting
        Flash(Grid);

    if(endCondition(Grid) != 0) //checks to see if the player won
        return endCondition(Grid); //if the game isn’t still going on it returns who won
   }
}

char TickTackToeHardTimed(char Grid[3][3]) //in this function 0 represents players 0's
{                 //1's represents the ai's X's and 2's are empty spaces       
   GetInputTimed(Grid);
   if(Grid[1][1] == 2) //2nd move
     Grid[1][1] = 1;
   else
     Grid[0][0] = 1;
   GetInputTimed(Grid); //3rd move
   
   if(!Defend(Grid))//4th move
 {
  if(Grid[1][1] == 1)
  {
  if(Grid[1][0] == 2)
    Grid[1][0] = 1;
  else if(Grid[1][2] == 2)
    Grid[1][2] = 1;
  else
    Grid[0][1] = 1;
  }
  else setup(Grid);
 }   

   GetInputTimed(Grid);
   if(endCondition(Grid) != 0)
    return endCondition(Grid);
   while(1)
   {
    if(winingmove(Grid));
    else if(Defend(Grid));
    else if (setup(Grid));
    else
      GoRand(Grid, 1);
    
    if(endCondition(Grid) != 0)
        return endCondition(Grid);
    
    GetInputTimed(Grid);
    if(endCondition(Grid) != 0) //checks to see if the game is still going on
        return endCondition(Grid); //if the game isnt going on returns who 
   }
}


char TickTackToeMed(char Grid[3][3]) //this difficulty adds blocking while placing randomly
{
 while(endCondition(Grid) == 0) //if no one has won
 {
   while(!GetInput(Grid))       //player input
     Flash(Grid);  //flashing grid to show ai moves while player is choosing input
   if (endCondition(Grid) == 0) //if the player didn't win with his play
   {
     if(Defend(Grid)); //checks to see if there is a point to defend 
     else GoRand(Grid, 1); //if there is no point to defend the AI picks a random empty spot
   }
 }  
 return endCondition(Grid);//at this point the game is over and the function returns who won
}

char TickTackToeEasy(char Grid[3][3])//just places randomly on the board
{
 while(endCondition(Grid) == 0)
 {
   Flash(Grid);
   while(!GetInput(Grid))  //player goes
     Flash(Grid);
   if (!endCondition(Grid)) //ai goes
     GoRand(Grid, 1);
  }
  return (endCondition(Grid));
}

char TickTackToe2(char Grid[3][3]) //used for 2 human players
{
  while (endCondition(Grid) == 0)
  {
    while(!GetInput(Grid)) //getting player 1 input
      Flash(Grid); //flashing grid to show player 2 moves
    if(!endCondition(Grid))
    {
      while(!Get2PlayerInput(Grid)) //getting player 2 input
        Flash(Grid);
    }
  }
  return endCondition(Grid);
}

void play(const char mode) //this is used to represent when a mode has been changed, higher difficulties have higher frequencies.
{
  char i;
  TR0 = 0;
  TF0 = 0;
  TMOD = 0x01;
  if (mode == 0)
  {
    highPreload = 0xf3; //600 hertz frequency
    lowPreload = 0xff;
  }
  else if (mode == 1)
  {
    highPreload = 0xf5; //700 hertz frequency
    lowPreload = 0xb6;
  }
  else if (mode == 2)
  {
    highPreload = 0xf6; //800 hertz frequency
    lowPreload = 0xff;
  }
  else if (mode == 3)
  {    
    highPreload = 0xf8; //1000 hertz frequency
    lowPreload = 0xcc;
  }
  else if (mode == 4)
  {
    highPreload = 0xf9;  //1200 hertz frequency
    lowPreload = 0xff;
  }
    
    for (i = 0; i < 120; i++)
  {
    TR0 = 0;
    TF0 = 0;
    TH0 = highPreload;
      TL0 = lowPreload;
      TR0=1;
      while(TF0==0);
    sound = ~sound;
   }
}


void Song(const char whoWon)//used to play a song corresponding to who won the game

{
    unsigned int i;
    unsigned char x;
    unsigned char song_low[]={0x56,0x85,0x8F,0x56,0x8F,0x85};   //The lower byte of notes for the winning song
    unsigned char song_high[]={0xE4,0xED,0xEF,0xE4,0xEF,0xEF};   //The higher byte of notes of the winning song

   if(whoWon == 2)
    {
    // The losing song
        song_low[0] = 0x85;
        song_low[1] = 0x85;
        song_low[2] = 0x5A;
        song_low[3] = 0x5A;
        song_low[4] = 0x56;
        song_low[5] = 0x56;

        song_high[0] = 0xED;
        song_high[1] = 0xED;
        song_high[2] = 0xE7;
        song_high[3] = 0xE7;
        song_high[4] = 0xE4;
        song_high[5] = 0xE4;
     }
     if(whoWon == 3)
    {
        //The song played for a tie
        song_low[0] = 0x8F;
        song_low[1] = 0x57;
        song_low[2] = 0x8F;
        song_low[3] = 0x57;
        song_low[4] = 0x8F;
        song_low[5] = 0x57;

        song_high[0] = 0xEF;
        song_high[1] = 0xF1;
        song_high[2] = 0xEF;
        song_high[3] = 0xF1;
        song_high[4] = 0xEF;
        song_high[5] = 0xF1;
     }
    TMOD=0x01;
    for(x=0;x<5;x++)    //used to play each note is the songs.  Since it is used for all the songs the songs lengths have to be the same
    {
        for(i=0;i<250;i++)    //the delay for the notes note set to a specific amount of time
        {
            TR0=0;
            TF0=0;
            TH0 = song_high[x];     //setting timer 0 high byte to notes high byte byte
            TL0 = song_low[x];	//setting timer 0 high byte to notes low byte
            TR0=1;
            while(TF0==0);
            sound = ~sound;
        }
    }   
}
void score(char num_of_wins) //used for writing to the 7 segment display based off player wins
{    

    //display data setup
    char display[]={0,0,0,0,0,0,0};

    //turn the clk low on the flip-flop chip
    Chip_enable = 0;

 //sets display equal to the number of wins - number of loses
    if (num_of_wins == 0)  
        {
        display[0]=0;
        display[1]=0;
        display[2]=0;
        display[3]=0;
        display[4]=0;
        display[5]=0;
        display[6]=1;
        }
    else if (num_of_wins == 1)
        {
        display[0]=1;
        display[1]=0;
        display[2]=0;
        display[3]=1;
        display[4]=1;
        display[5]=1;
        display[6]=1;
        }
    else if (num_of_wins == 2)
        {
        display[0]=0;
        display[1]=1;
        display[2]=0;
        display[3]=0;
        display[4]=0;
        display[5]=1;
        display[6]=0;
        }
    else if (num_of_wins == 3)
        {
        display[0]=0;
        display[1]=0;
        display[2]=0;
        display[3]=0;
        display[4]=1;
        display[5]=1;
        display[6]=0;
        }
    else if (num_of_wins == 4)
        {
        display[0]=1;
        display[1]=0;
        display[2]=0;
        display[3]=1;
        display[4]=1;
        display[5]=0;
        display[6]=0;
        }
    else if (num_of_wins == 5)
        {
        display[0]=0;
        display[1]=0;
        display[2]=1;
        display[3]=0;
        display[4]=1;
        display[5]=0;
        display[6]=0;
        }
    else if (num_of_wins == 6)
        {
        display[0]=0;
        display[1]=0;
        display[2]=1;
        display[3]=0;
        display[4]=0;
        display[5]=0;
        display[6]=0;
        }
    else if (num_of_wins == 7)
        {
        display[0]=0;
        display[1]=0;
        display[2]=0;
        display[3]=1;
        display[4]=1;
        display[5]=1;
        display[6]=1;
        }
    else if (num_of_wins == 8)
        {
        display[0]=0;
        display[1]=0;
        display[2]=0;
        display[3]=0;
        display[4]=0;
        display[5]=0;
        display[6]=0;
        }
    else if (num_of_wins == 9)
        {
        display[0]=0;
        display[1]=0;
        display[2]=0;
        display[3]=0;
        display[4]=1;
        display[5]=0;
        display[6]=0;
        }
    else
    {
      display[0]=1;
      display[1]=0;
      display[2]=0;
      display[3]=0;
      display[4]=0;
      display[5]=0;
      display[6]=1;
    }
    
//set the displays segments with corresponding port
    upleftLED = display[0];
    upmidLED = display[1];
    uprightLED = display[2];
    midleftLED =  display[3];
    midmidLED =  display[4];
    midrightLED =  display[5];
    downleftLED =   display[6];
    Chip_enable = 1;   //.rising clk input writes allows the data to be written to the flip-flop
}

void main (void)
{
  code char Easy[] = {'E', 'a', 's', 'y', ' ', 'D', 'i', 'f','f','i','c','u','l','t','y'};
  code char Med[] = {'M', 'e', 'd','i','u','m', ' ', 'D', 'i', 'f','f','i','c','u','l','t','y'};
  code char Hard[] = {'H', 'a', 'r', 'd', ' ' , 'D', 'i', 'f','f','i','c','u','l','t','y'};
  code char TwoPlayer[] = {'2',' ','P','l','a','y','e','r',' ','M','o','d','e'};
  code char HardSpeed[] = {'H', 'a', 'r', 'd', ' ', 'S', 'p', 'e', 'e', 'd'};
  char Grid[3][3] = {2}; // Grid used as ticktacktoe board, initialized as empty spaces, Grid[y][x]
  char i;//iterator used for loop
  bit played = 0; //bit to tell if player has just played a game and if score should be updated
  char whoWon; //character that stores which player won
  char num_of_wins = 0; //stores the amount of time human player has won 

  P2M1 = 0x00;
  P1M1 = 0x00;
  P0M1 = 0x00;
  uart_init();
  score(num_of_wins); //updates score to zero wins
  Reset(Grid);
  while(1)
  {
      //add input method to add different modes
    if(upleftINPUT == 0) //playing easy
    {
      for (i = 0; i<15; i++)
          uart_transmit(Easy[i]);		//display’s mode
      uart_transmit('\r');			 //puts cursor at beginning of line
      uart_transmit('\n'); 		//puts cursor one line below
      Reset(Grid); 			//resets the board
      play(0); 				//play the mode changing sound
      while(upleftINPUT == 0);		//check for double input at the beginning
      whoWon = TickTackToeEasy(Grid);
      while((upleftINPUT == 0) || (upmidINPUT == 0) || (uprightINPUT == 0) || (midleftINPUT == 0) || (midmidINPUT == 0)); 		//check for double input at the end
      played = 1; 			//set to check if the player has played a game fully
    }
    else if(upmidINPUT == 0) //playing medium
    {
      for (i = 0; i<17; i++)
          uart_transmit(Med[i]);
      uart_transmit('\r');
      uart_transmit('\n');
      Reset(Grid);
      play(1);
      while(upmidINPUT == 0);
      whoWon = TickTackToeMed(Grid);
      while((upleftINPUT == 0) || (upmidINPUT == 0) || (uprightINPUT == 0) || (midleftINPUT == 0) || (midmidINPUT == 0));
      played = 1;
    }
    else if(uprightINPUT == 0) //played hard
    {
      for (i = 0; i<15; i++)
          uart_transmit(Hard[i]);
      uart_transmit('\r');
      uart_transmit('\n');
      Reset(Grid);
      play(2);
      while(uprightINPUT == 0);
      whoWon = TickTackToeHard(Grid);
          while((upleftINPUT == 0) || (upmidINPUT == 0) || (uprightINPUT == 0) || (midleftINPUT == 0) || (midmidINPUT == 0));
      played = 1;
    }
    else if (midleftINPUT == 0) //playing 2-player
    {
      for(i = 0; i < 13; i++)
          uart_transmit(TwoPlayer[i]);
      uart_transmit('\r');
      uart_transmit('\n');
      Reset(Grid);
      play(3);
      while(midleftINPUT == 0);
      whoWon = TickTackToe2(Grid);
      while((upleftINPUT == 0) || (upmidINPUT == 0) || (uprightINPUT == 0) || (midleftINPUT == 0) || (midmidINPUT == 0));
      played = 1;
    }
    else if (midmidINPUT == 0)
    {
      for(i = 0; i < 10; i++)
             uart_transmit(HardSpeed[i]);
      uart_transmit('\r');
      uart_transmit('\n');
      Reset(Grid);
      play(4);
      while(midmidINPUT ==0);
      whoWon = TickTackToeHardTimed(Grid);
      while((upleftINPUT == 0) || (upmidINPUT == 0) || (uprightINPUT == 0) || (midleftINPUT == 0) || (midmidINPUT == 0));
      played = 1;
    }
    if(played) //will only display who won if the player has played a game
    {
      if (whoWon == 1) // player won
      {
        num_of_wins++;
        Song(whoWon);
      }
      else if (whoWon == 2)
      { // ai won
        num_of_wins--;
        if(num_of_wins < 0)
          num_of_wins = 0;
        Song(whoWon);
      }
      else //tie
      {
        Song(whoWon);
      }
      played = 0; //player need to play another game for this to be updated
      score(num_of_wins);
      Reset(Grid);
    }
  Flash(Grid); //flashing grid while player hasn’t chosen a mode
  }
}


