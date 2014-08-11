/*
  Flappy Bit Version 1.1
  By Daniel Ratcliffe
  http://www.twitter.com/DanTwoHundred
 */
 
// CONFIGURATION
// Set these values to the pin numbers you're using on your Arduino.
// You may set any value to zero to ignore this input or output
int rowPins[] = { 5, 10, A3, 8, 2, A2, A0, A1 };
int columnPins[] = { 9, 4, 3, 6, A5, 7, 11, 12 };
int buttonPin = A4;

// STATE
boolean menu = true;
boolean gameOver = false;
int lastButtonState = 0;

int birdPos = 2;
int pipePos = 8;
int pipeGap = 3;
int ticks = 0;
int score = 0;

int inGameScreen[] = {
  0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,
};

int menuScreen[] = {
  ///*
  // Bird
  0,0,0,1,1,0,0,0,
  0,0,1,0,0,1,0,0,
  0,0,1,0,1,1,0,0,
  1,1,1,0,0,1,0,0,
  1,0,0,1,1,1,1,0,
  1,0,1,0,1,0,0,1,
  0,1,0,0,1,1,1,0,
  0,0,1,1,1,0,0,0,
  //*/
  /*
  // Heart
  0,1,1,0,0,1,1,0,
  1,0,0,1,1,0,0,1,
  1,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,1,
  1,0,0,0,0,0,0,1,
  0,1,0,0,0,0,1,0,
  0,0,1,0,0,1,0,0,
  0,0,0,1,1,0,0,0,
  */
  /*
  // Creeper
  0,0,0,0,0,0,0,0,
  0,1,1,0,0,1,1,0,
  0,1,1,0,0,1,1,0,
  0,0,0,1,1,0,0,0,
  0,0,1,1,1,1,0,0,
  0,0,1,1,1,1,0,0,
  0,0,1,0,0,1,0,0,
  0,0,0,0,0,0,0,0,
  */
};

// UTILITY
void drawInGameScreen( int colour )
{  
  if( birdPos >= 0 && birdPos < 8 )
  {
    if( !gameOver || (gameOver && ((ticks / 24) % 2) == 1) )
    {
      inGameScreen[ 1 + birdPos*8 ] = colour;
    }
  }
  for( int y=0; y<8; ++y )
  {
    if( y < pipeGap || y >= pipeGap + 3 )
    {
      if( pipePos >= 0 && pipePos < 8 )
      {
        inGameScreen[ pipePos + y*8 ] = colour;
      }
      if( pipePos >= -1 && pipePos < 7 )
      {
        inGameScreen[ pipePos + 1 + y*8 ] = colour;
      }
    }
  }
}

// MAIN
void setup()
{             
  // Set all pin modes  
  for( int n=0; n<8; ++n )
  {
    int column = columnPins[n];
    if( column > 0 )
    {
      pinMode( column, OUTPUT );
      digitalWrite( column, HIGH );
    }
    int row = rowPins[n];
    if( row > 0 )
    {
      pinMode( row, OUTPUT );
      digitalWrite( row, LOW );
    }
  }
  if( buttonPin > 0 )
  {
    pinMode( buttonPin, INPUT );
  }
}

void loop() 
{  
  // Update
  int buttonState = (buttonPin > 0 && digitalRead( buttonPin ) == HIGH) ? 1 : 0;
  if( menu )
  {
    // IN MENU
    ++ticks;
    if( (ticks % 12) == 0 )
    {
      // Animate the eye
      ///*
      if( rand()%30 == 0 ) 
      {
        menuScreen[ 19 ] = 1;
        menuScreen[ 20 ] = 0;
      }
      else
      {
        menuScreen[ 19 ] = 0;
        menuScreen[ 20 ] = 1;
      }
      //*/
    }

    if( buttonState != lastButtonState && buttonState )
    {
      // Start game    
      menu = false;
      ticks = 0;
      birdPos = 2;
      pipePos = 20;
      pipeGap = 3;
      gameOver = false;
      score = 0;
      drawInGameScreen( 1 );
    }
  }
  else
  {
    // IN GAME
    // Clear the screen
    drawInGameScreen( 0 );
    
    // Update the state
    ++ticks;
    if( !gameOver && ((ticks % 12) == 0) )
    {
      // Move the pipe
      int lastPipePos = pipePos;
      pipePos--;
      if( pipePos < -1 )
      {
        pipePos = 7; 
        pipeGap = 1 + rand()%4;
      }
      else
           
      // Move the bird
      int lastBirdPos = birdPos;
      if( buttonState )
      {
        birdPos = max( birdPos - 1, 0 );
      }
      else
      {
        birdPos = birdPos + 1;
        if( birdPos >= 8 )
        {
          gameOver = true;
          pipePos = lastPipePos;
          birdPos = lastBirdPos;
          ticks = 0;
        }
      }
      
      // Test for pipe collision
      if( (pipePos == 1 || pipePos == 0) && (birdPos < pipeGap || birdPos >= pipeGap + 3) )
      {
        gameOver = true;
        pipePos = lastPipePos;
        birdPos = lastBirdPos;
        ticks = 0;
      }
      else if( pipePos == -1 )
      {
        score++;
        Serial.println( score );
      }        
    }
    else if( gameOver && ticks >= 96 )
    {
      // Return to menu
      menu = true;
    }
      
    // Redraw the screen
    if( !menu )
    {
      drawInGameScreen( 1 );
    }
  }
  lastButtonState = buttonState;
  
  // Render
  int* screen = menu ? menuScreen : inGameScreen;
  for( int x=0; x<8; ++x )
  {
    for( int y=0; y<8; ++y )
    {
      int row = rowPins[y];
      if( row > 0 )
      {
        if( screen[ (7-y)+x*8 ] > 0 ) // Uncomment to rotate screen orientation
        //if( screen[ x+y*8 ] > 0 )
        {
          digitalWrite( row, HIGH );
        }
        else
        {
          digitalWrite( row, LOW );
        }
      }
    }
    int column = columnPins[x];
    if( column > 0 )
    {
      digitalWrite( column, LOW );
    }
    delay( 2 );
    if( column > 0 )
    {
      digitalWrite( column, HIGH );
    }
  }
}
