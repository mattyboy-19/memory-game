#include <Wire.h>
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>
Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

#define UP_ARROW 0
byte up[] = {4, 14, 31, 4, 4, 4, 0, 0};
#define DOWN_ARROW 1
byte down[] = {0, 0, 4, 4, 4, 31, 14, 4};
#define LEFT_ARROW 2
byte left[] = {0, 0, 4, 12, 31, 12, 4, 0};
#define RIGHT_ARROW 3
byte right[] = {0, 0, 4, 6, 31, 6, 4, 0};

int M = 2;
bool Mset = false;
int N = 4;
bool Nset = false;
int D = 1000;
int S[10];

int score = 0;
bool done = true; 
bool gameover = false;

String mode = ""; 
String msg = "PRACTICE";

char * welcomemessage = "     Welcome to the memory game! Choose mode:      ";
char * setMmessage = "      Symbols:      ";
char * setNmessage = "      Length:     ";
int icursor = 0; 

void setup() {
  
  Serial.begin(9600);
  lcd.clear();
  lcd.begin(16,2);
  randomSeed(analogRead(A0));

  lcd.createChar(UP_ARROW, up);
  lcd.createChar(DOWN_ARROW, down);
  lcd.createChar(LEFT_ARROW, left);
  lcd.createChar(RIGHT_ARROW, right);
}

void generate_sequence(){
  for (int i=0; i<N; i++){
    S[i] = random(0,M);
  }
  for (int i=0; i<N; i++){
    display_arrow(S[i]);
  }
}

void display_arrow(int i){ 
  delay(D);
  switch(i){
    case 0:
      lcd.write(UP_ARROW);
      break;
    case 1:
      lcd.write(DOWN_ARROW);
      break;
    case 2:
      lcd.write(LEFT_ARROW);
      break;
    case 3:
      lcd.write(RIGHT_ARROW);
      break;
  }
  delay(D);
  lcd.clear();
}

void loop() {
  if(done == true){
    Mset = false;
    Nset = false;
    mode = display_menu();
  }
  if(mode == "STORY"){
    // do story mode, increase values of M, N and D as game progresses.
    lcd.clear();
    if(N>=7){
      if(M != 4){
        M+= 1;
        N = 4;
      }else{
        N+=1;
      }
    }
    if(gameover == false){
      generate_sequence();
      get_sequence();
      delay(1000);
      lcd.print("Score:");
      lcd.setCursor(0,1);
      lcd.print(score, DEC);
      delay(1000);
      lcd.clear();
      delay(500);
      N+=1;
      if(gameover == true){
        score = 0;
        N = 4;
        M = 2;
        done = true;
      }
    }
  }
  if(mode == "PRACTICE"){
    //start the practice mode, the user chooses the values of M, N and D.
    lcd.clear();
    if(Mset == false){
      delay(100);
      set_practice_m_value();
    }
    if((Nset == false) && (Mset == true)){
      delay(100);
      set_practice_n_value();
    }
    if((Nset == true) && (Mset == true)){
      lcd.clear();
      delay(1000);
      generate_sequence();
      get_sequence();
      delay(1000);
      M = 2;
      N = 4;
      done = true;
      delay(1000);
    }
  }
}

void get_sequence(){
  lcd.print("GO:");
  lcd.setCursor(0,1);
  bool seq = false;
  int seqlen = 0;
  int C[10];
  while(seq == false){
    int b = lcd.readButtons();
    if(b & BUTTON_UP){
      lcd.write(UP_ARROW);
      delay(1000);
      C[seqlen] = 0;
      seqlen+=1;
    }
    if(b & BUTTON_DOWN){
      lcd.write(DOWN_ARROW);
      delay(1000);
      C[seqlen] = 1;
      seqlen+=1;
    } 
    if(b & BUTTON_LEFT){
      lcd.write(LEFT_ARROW);
      delay(1000);
      C[seqlen] = 2;
      seqlen+=1;
    }
    if(b & BUTTON_RIGHT){
      lcd.write(RIGHT_ARROW);
      delay(1000);
      C[seqlen] = 3;
      seqlen+=1;
    }
    if(seqlen == N){
      int checker = 0;
      for(int i=0; i<N;i++){
        if(S[i] == C[i]){
          checker +=1;
        }
      }
      if(checker == N){
        lcd.clear();
        lcd.setBacklight(0x2);
        lcd.print("CORRECT");
        delay(3000);
        lcd.clear();
        lcd.setBacklight(0x7);
        score += 10;
        seq = true;
      }else{
        lcd.clear();
        lcd.setBacklight(0x1);
        lcd.print("INCORRECT");
        delay(3000);
        lcd.clear();
        lcd.setBacklight(0x7);
        seq = true;
        gameover = true;
      }
    }
  }
}

void set_practice_m_value(){
  lcd.setCursor(0, 1);
  lcd.print(M, DEC);
  update_display(setMmessage);
  delay(350);
  int b = lcd.readButtons();
  if (b & BUTTON_UP) {
    lcd.setCursor(0, 1);
    lcd.print("         ");
    if(M != 4){
      M += 1;
    }else{
      M = 2;
    }
  }
  if (b & BUTTON_DOWN) {
    lcd.setCursor(0, 1);
    lcd.print("         ");
    if(M != 2){
      M -= 1;
    }else{
      M = 4;
    }
  }
  if (b & BUTTON_SELECT) {
    Mset = true;
  }
}

void set_practice_n_value(){
  lcd.setCursor(0, 1);
  lcd.print(N, DEC);
  update_display(setNmessage);
  delay(350);
  int b = lcd.readButtons();
  if (b & BUTTON_UP) {
    lcd.setCursor(0, 1);
    lcd.print("         ");
    if(N != 10){
      N += 1;
    }else{
      N = 4;
    }
  }
  if (b & BUTTON_DOWN) {
    lcd.setCursor(0, 1);
    lcd.print("         ");
    if(N != 4){
      N -= 1;
    }else{
      N = 10;
    }
  }
  if (b & BUTTON_SELECT) {
    Nset = true;
  }
}

String display_menu(){
  update_display(welcomemessage);
  delay(350);
  int b = lcd.readButtons();
  if (b & BUTTON_UP) {
    lcd.setCursor(0, 1);
    lcd.print("         ");
    if(msg != "STORY"){
      msg = "STORY";
    }else{
      msg = "PRACTICE";
    }
  }
  if (b & BUTTON_DOWN) {
    lcd.setCursor(0, 1);
    lcd.print("         ");
    if(msg != "PRACTICE"){
      msg = "PRACTICE";
    }else{
      msg = "STORY";
    }
  }
  if (b & BUTTON_SELECT) {
    done = false;
    return msg;
  }
  lcd.setCursor(0, 1);
  lcd.print(msg);
}

void update_display(char * message){
  int messagelength = strlen(message);
  if(icursor == (messagelength - 1)){
    icursor = 0;
  }
  lcd.setCursor(0,0);
  if(icursor < (messagelength -16)){
    for(int i = icursor; i < icursor+16; i++){
      lcd.print(message[i]);
    }
  }
  else{
    for (int i = icursor; i < (messagelength - 1); i++){
      lcd.print(message[i]);
    }
    for (int i = 0; i <= 16 - (messagelength - icursor); i++){
      lcd.print(message[i]);
    }
  }
  icursor++;
}
