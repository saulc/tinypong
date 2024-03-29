 
#include "SSD1306_minimal.h"
#include <avr/pgmspace.h>

SSD1306_Mini oled;

int buzz = 1;

void startBuzz(){
  for(int x = 0; x<255; x+=50){
      analogWrite(buzz, x);
      delay(200+x);
      analogWrite(buzz, 0);
      delay(100);
  }
     
}

void heartBeat(){

    digitalWrite(4, HIGH);   // set the LED on  
    delay(200);  
    digitalWrite(4, LOW);    // set the LED off
    delay(100);

    digitalWrite(4, HIGH);   // set the LED on  
    delay(500);  
    digitalWrite(4, LOW);    // set the LED off
    delay(100);
  
}

/*
    display 128x64
    snake size 4x4
   
    128 / 8 => 16cols
     64 / 8 => 8rows
    
*/

#define ColCount 32
#define RowCount 16 

// there are different wall types
unsigned char wall[5][4]= { 
  0x0, 0x0, 0x0, 0x0,
  0xf, 0xf, 0xf, 0xf,
  0xf, 0x9, 0x9, 0xf,
  0x9, 0x9, 0x9, 0x9,
  0x9, 0x6, 0x6, 0x9,
};
// the ball shape
unsigned char ball[4]= { 0x6, 0x9, 0x9, 0x6 };
unsigned char padal[4]= { 0x7, 0x9, 0x7, 0x9 };


static unsigned char snakeRow= 10;
static unsigned char snakeCol= 7;  
static char snakeRowDir= +1;
static char snakeColDir= -1;
static char wallIdx=1;

static unsigned char p1 = RowCount/2;
static unsigned char p2 = RowCount/2;



// this is the room shape //32 x 16
const static uint8_t tinypong[] PROGMEM ={
1,1,1,1,1,1,1,1 , 1,1,1,1,1,1,1,1 , 1,1,1,1,1,1,1,1 , 1,1,1,1,1,1,1,1,
0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0,
0,1,1,1,1,1,1,0 , 0,1,1,1,1,1,1,0 , 1,1,0,0,0,0,1,0 , 1,1,0,0,0,0,1,1,
0,0,0,1,1,0,0,0 , 0,0,0,1,1,0,0,0 , 1,1,1,0,0,0,1,0 , 1,1,0,0,0,0,1,1,
0,0,0,1,1,0,0,0 , 0,0,0,1,1,0,0,0 , 1,0,1,1,0,0,1,0 , 1,1,0,0,0,0,1,1,
0,0,0,1,1,0,0,0 , 0,0,0,1,1,0,0,0 , 1,0,0,1,1,0,1,0 , 1,1,1,1,1,1,1,1,
0,0,0,1,1,0,0,0 , 0,0,0,1,1,0,0,0 , 1,0,0,0,1,1,1,0 , 0,0,0,1,1,0,0,0,
0,0,0,1,1,0,0,0 , 0,1,1,1,1,1,1,0 , 1,0,0,0,0,1,1,0 , 0,0,0,1,1,0,0,0,
0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0,
1,1,1,1,1,1,1,0 , 1,1,1,1,1,1,1,0 , 1,1,0,0,0,0,1,0 , 1,1,1,1,1,1,1,1,
1,1,0,0,0,0,1,0 , 1,1,0,0,0,0,1,0 , 1,1,1,0,0,0,1,0 , 1,1,0,0,0,0,1,1,
1,1,0,0,0,0,1,0 , 1,1,0,0,0,0,1,0 , 1,0,1,1,0,0,1,0 , 1,1,0,0,0,0,0,0,
1,1,1,1,1,1,1,0 , 1,1,0,0,0,0,1,0 , 1,0,0,1,1,0,1,0 , 1,1,0,0,1,1,1,1,
1,1,0,0,0,0,0,0 , 1,1,0,0,0,0,1,0 , 1,0,0,0,1,1,1,0 , 1,1,0,0,0,0,1,1,
1,1,0,0,0,0,0,0 , 1,1,1,1,1,1,1,0 , 1,0,0,0,0,1,1,0 , 1,1,1,1,1,1,1,1,
1,1,1,1,1,1,1,1 , 1,1,1,1,1,1,1,1 , 1,1,1,1,1,1,1,1 , 1,1,1,1,1,1,1,1,
};

const static uint8_t room[] PROGMEM ={
1,1,1,1,1,1,1,1 , 1,1,1,1,1,1,1,1 , 1,1,1,1,1,1,1,1 , 1,1,1,1,1,1,1,1,
0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0,
0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0,
1,1,1,1,1,1,1,1 , 1,1,1,1,1,1,1,1 , 1,1,1,1,1,1,1,1 , 1,1,1,1,1,1,1,1,
};

//const static uint8_t room[] PROGMEM ={
//1,1,1,1,1,1,1,1 , 1,1,1,1,1,1,1,1 , 1,1,1,1,1,1,1,1 , 1,1,1,1,1,1,1,1,
//1,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,1,
//1,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,1,
//1,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,1,
//1,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,2,0,0,0,0,0 , 0,0,0,0,0,0,0,1,
//1,0,0,0,2,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,2,0,0,0,0,2 , 2,2,2,0,0,0,0,1,
//1,0,0,0,2,0,0,0 , 0,0,2,2,2,0,0,0 , 0,0,2,0,0,0,0,0 , 0,0,0,0,0,0,0,1,
//1,0,0,0,2,0,0,0 , 0,0,0,0,2,0,0,0 , 0,0,2,0,0,0,0,0 , 0,0,0,0,0,0,0,1,
//1,0,0,0,2,0,0,0 , 0,0,0,0,2,2,2,0 , 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,1,
//1,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,1,
//1,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,1,
//1,0,0,0,0,0,0,0 , 0,2,2,2,2,0,0,0 , 0,2,0,0,0,0,0,2 , 2,2,2,2,0,0,0,1,
//1,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,2,0,0,0,0,0,0 , 0,0,0,0,0,0,0,1,
//1,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,1,
//1,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,0 , 0,0,0,0,0,0,0,1,
//1,1,1,1,1,1,1,1 , 1,1,1,1,1,1,1,1 , 1,1,1,1,1,1,1,1 , 1,1,1,1,1,1,1,1,
//};


unsigned char getRoom( unsigned char row, unsigned char col  ){

  unsigned char data= pgm_read_byte( &(room[row*ColCount + col]) );
  
  return data;
}

unsigned char getSnake( unsigned char row, unsigned char col ){
  
  
  unsigned char data=0;
  
  if ((row == snakeRow) && (col == snakeCol)){
    data=2;    
  }
  
  return data;  
}


unsigned char getPadal( unsigned char row, unsigned char col ){
  
  
  unsigned char data=0;
  
  if ( (row >= p1 -1) && (row <= p1 + 1) && (col == 0) ){
    data=3;    
  }
  
     if ( (row >= p2-1) && (row <= p2+1) && (col == ColCount-1) ){
    data=4;    
  }
  
  return data;  
}
  
unsigned char hitRoom( unsigned char row, unsigned char col ){

  uint8_t data = getRoom( row, col );
 
  return data;
}



unsigned char getSplash( unsigned char row, unsigned char col  ){

  unsigned char data= pgm_read_byte( &(tinypong[row*ColCount + col]) );
  
  return data;
}

void splash(){

  oled.startScreen();

  uint8_t upperRow;
  uint8_t lowerRow;

  uint8_t data[4];
  

        
          // send a bunch of data in one xmission
     
//            upperRow= getRoom(r, c);
//            lowerRow= getRoom(r+1, c);
//     

            //data= 0x0;
            
      for (char r=0;r<RowCount; r=r+2 ){
      for (char c=0;c<ColCount; c++){
            upperRow= getSplash(r, c);
            lowerRow= getSplash(r+1, c);

               data[0]= 0x0;
               data[1]= 0x0;
               data[2]= 0x0;
               data[3]= 0x0;
                if (upperRow){
               data[0]|= wall[upperRow][0] << 0;
               data[1]|= wall[upperRow][1] << 0;
               data[2]|= wall[upperRow][2] << 0;
               data[3]|= wall[upperRow][3] << 0;
            }
            
            if (lowerRow){
               data[0]|= wall[lowerRow][0] << 4;
               data[1]|= wall[lowerRow][1] << 4;
               data[2]|= wall[lowerRow][2] << 4;
               data[3]|= wall[lowerRow][3] << 4;
            }
            
          Wire.beginTransmission(SlaveAddress);
          Wire.send(GOFi2cOLED_Data_Mode);            // data mode


            Wire.send( data[0] );
            Wire.send( data[1] );
            Wire.send( data[2] );
            Wire.send( data[3] );
          Wire.endTransmission();
      }
      }
             
//            
//            // room
//            if (upperRow){
//               data[0]|= wall[upperRow][0] << 0;
//               data[1]|= wall[upperRow][1] << 0;
//               data[2]|= wall[upperRow][2] << 0;
//               data[3]|= wall[upperRow][3] << 0;
//            }
//            
//            if (lowerRow){
//               data[0]|= wall[lowerRow][0] << 4;
//               data[1]|= wall[lowerRow][1] << 4;
//               data[2]|= wall[lowerRow][2] << 4;
//               data[3]|= wall[lowerRow][3] << 4;
//            }
       
        
    
  
}



void displayRoom(){

  oled.startScreen();

  uint8_t upperRow;
  uint8_t lowerRow;

  uint8_t upperSnake;
  uint8_t lowerSnake;

    uint8_t upperPadal;
    uint8_t lowerPadal;
    
  uint8_t data[4];
  

  int i=0;
  for (char r=0;r<RowCount; r=r+2 ){
    for (char c=0;c<ColCount; c++){
        
          // send a bunch of data in one xmission
          Wire.beginTransmission(SlaveAddress);
          Wire.send(GOFi2cOLED_Data_Mode);            // data mode

            upperRow= getRoom(r, c);
            lowerRow= getRoom(r+1, c);
            
            upperSnake= getSnake(r, c);
            lowerSnake= getSnake(r+1, c);

            upperPadal = getPadal(r, c);
            lowerPadal = getPadal(r+1, c);

            //data= 0x0;
               data[0]= 0x0;
               data[1]= 0x0;
               data[2]= 0x0;
               data[3]= 0x0;
            
            // room
            if (upperRow){
               data[0]|= wall[upperRow][0] << 0;
               data[1]|= wall[upperRow][1] << 0;
               data[2]|= wall[upperRow][2] << 0;
               data[3]|= wall[upperRow][3] << 0;
            }
            
            if (lowerRow){
               data[0]|= wall[lowerRow][0] << 4;
               data[1]|= wall[lowerRow][1] << 4;
               data[2]|= wall[lowerRow][2] << 4;
               data[3]|= wall[lowerRow][3] << 4;
            }
            
             //data|= (upperRow?0x0F:0x00) | (lowerRow?0xF0:0);            
            // snake
            if (upperSnake){
               data[0]|= ball[0] << 0;
               data[1]|= ball[1] << 0;
               data[2]|= ball[2] << 0;
               data[3]|= ball[3] << 0;
            }
            if (lowerSnake){
               data[0]|= ball[0] << 4;
               data[1]|= ball[1] << 4;
               data[2]|= ball[2] << 4;
               data[3]|= ball[3] << 4;
            }
            //data|= (upperSnake?0x0A:0x00) | (lowerSnake?0xA0:0);            
            //padals
             if (upperPadal){
               data[0]|= padal[0] << 0;
               data[1]|= padal[1] << 0;
               data[2]|= padal[2] << 0;
               data[3]|= padal[3] << 0;
            }
            if (lowerPadal){
               data[0]|= padal[0] << 4;
               data[1]|= padal[1] << 4;
               data[2]|= padal[2] << 4;
               data[3]|= padal[3] << 4;
            }
            
            Wire.send( data[0] );
            Wire.send( data[1] );
            Wire.send( data[2] );
            Wire.send( data[3] );
          Wire.endTransmission();
          
    }
  }  
}

void changeWallIdx(){
      wallIdx++;
      if (wallIdx>1){
        wallIdx=0;
      }
}

void die( unsigned char r, unsigned char c ){

  Wire.beginTransmission(SlaveAddress);
    Wire.send(GOFi2cOLED_Data_Mode);            // data mode

    Wire.send( 0xFF );
    Wire.send( 0x81 );
    Wire.send( 0x81 );
    Wire.send( 0xFF );
  Wire.endTransmission();
}

void snake(){
  
  oled.cursorTo(3,63);

  die( 0, 0);
  
}

void readInput(){
  int a1 = analogRead(2);
  int a2 = analogRead(3);

   a1 = map(a1, 0, 1023, 1, RowCount-1);
   a2 = map(a2, 0, 1023, 1, RowCount-1);

   p1 = a1;
   p2 = a2;
   
}
void move(){
  
  char bHitRoom;
  char hitType;
  
  do {  
    bHitRoom= 0;
    // test row
    hitType = hitRoom( snakeRow + snakeRowDir, snakeCol);
    if (hitType != 0){
      snakeRowDir= -1* snakeRowDir;
      bHitRoom= hitType;
    }
    // test col
    hitType = hitRoom( snakeRow, snakeCol + snakeColDir);
    if (hitType != 0){
      snakeColDir= -1* snakeColDir;
      bHitRoom= hitType;
    }
    //test padals
    if(snakeCol + snakeColDir == 0){
    hitType = getPadal( snakeRow, 0);
    if(hitType !=0){
       snakeColDir= -1* snakeColDir;
        bHitRoom= hitType;
    }
    }
    if(snakeCol + snakeColDir == ColCount-1){
     hitType = getPadal( snakeRow, ColCount-1);
    if(hitType !=0){
       snakeColDir= -1* snakeColDir;
         bHitRoom= hitType;
    }
    }
//    if( (snakeCol + snakeColDir == 1) && (p1 == snakeRow) ){
//      hitType = 3;
//      snakeColDir= -1* snakeColDir;
//     snakeColDir= -1* snakeColDir;
//    }
//    if( (snakeCol + snakeColDir == ColCount-2) && (p2 == snakeRow) ){
//      hitType = 4;
//      snakeColDir= -1* snakeColDir;
//      //snakeRowDir= -1* snakeRowDir;
//      bHitRoom= hitType;
//    }
    
    //
    if (bHitRoom==1){
      //nur an der wand ändern
      //changeWallIdx();
    }
    
  } while (bHitRoom);
  
  snakeRow= snakeRow + snakeRowDir;
  snakeCol= snakeCol + snakeColDir;
  
  
}


void setup()
{
  //pinMode(4, OUTPUT);   
  
   // heartBeat();

    oled.init(0x3d);
    oled.clear();
    splash();
 // displayRoom();
    startBuzz();
    
//    for (int k=0;k<100;k++){
//    for (int i=0;i<1000;i++){
//      displayRoom();
//      
//      move();
//     
//      delay(5); 
//    }
//      
//    }
//    
   
//    delay(4000);
    
/*
    for (int i=0;i<10000;i++){
    for (int k=0;k<10000;k++){
      oled.displayX(k);
    }
    }
    
    */
}
 
void loop()
{
      displayRoom();
      move();
      readInput();
      delay(1); 
} 
