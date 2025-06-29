#include <Keypad.h>

const int buzzerPin   = 8;            // passive buzzer + → D8, – → GND
const int debugLed    = LED_BUILTIN;  // onboard LED to show serial hits
const int MAX_EVENTS  = 100;

// keypad wiring
const byte ROWS = 4, COLS = 4;
byte rowPins[ROWS] = {30, 31, 32, 33};
byte colPins[COLS] = {34, 35, 36, 37};
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
Keypad keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// note tables
const int lowOct[8]  = {0, 262, 294, 330, 349, 392, 440, 494};
const int highOct[8] = {0, 523, 587, 659, 698, 784, 880, 988};

bool useHighOctave = false;
int  recorded[MAX_EVENTS];
int  recordCount   = 0;
bool recording     = false;
unsigned long recordStart = 0;
int  lastReceived  = -1;

// play a pitched note
void playNote(int idx, int duration=300) {
  if (idx<1 || idx>7) return;
  int freq = useHighOctave ? highOct[idx] : lowOct[idx];
  tone(buzzerPin, freq, duration);
  delay(duration + 20);
  noTone(buzzerPin);
}

// half‐second beep for C/D
void beepShort() {
  tone(buzzerPin, 1000, 500);
  delay(550);
  noTone(buzzerPin);
}

void setup(){
  Serial.begin(9600);
  pinMode(buzzerPin, OUTPUT);
  pinMode(debugLed, OUTPUT);
}

void loop(){
  // —— Serial parser ——
  static char buf[16];
  static uint8_t bi = 0;
  while(Serial.available()){
    char c = Serial.read();
    if(c=='\r') continue;
    if(c=='\n'){
      if(bi){
        int v=-1;
        for(uint8_t i=0;i<bi;i++){
          if(isDigit(buf[i])){
            v=buf[i]-'0';
            break;
          }
        }
        if(v>=1 && v<=7 && v!=lastReceived){
          // flash LED
          digitalWrite(debugLed, HIGH);
          delay(20);
          digitalWrite(debugLed, LOW);
          // record & play
          if(recording && recordCount<MAX_EVENTS){
            recorded[recordCount++] = v;
          }
          playNote(v);
          lastReceived = v;
        }
      }
      bi = 0;
    } else if(bi < sizeof(buf)-1){
      buf[bi++] = c;
    }
  }

  // —— Keypad handling ——
  char k = keypad.getKey();
  if(k){
    switch(k){
      case 'A':
        useHighOctave = false;
        playNote(1,200);
        break;
      case 'B':
        useHighOctave = true;
        playNote(1,200);
        break;
      case 'C':
        // start 30 s recording
        recordCount  = 0;
        recording    = true;
        recordStart  = millis();
        lastReceived = -1;
        beepShort();
        break;
      case 'D':
        // end or manual playback
        beepShort();
        for(int i=0; i<recordCount; i++){
          playNote(recorded[i]);
          delay(50);
        }
        break;
      case '1': case '2': case '3':
      case '4': case '5': case '6': case '7': {
        int v = k - '0';
        if(recording && v != lastReceived && recordCount<MAX_EVENTS){
          recorded[recordCount++] = v;
          lastReceived = v;
        }
        playNote(v);
        break;
      }
      default:
        break;
    }
  }

  // —— Auto-stop 30 s recording ——
  if(recording && millis() - recordStart >= 30000){
    recording = false;
    beepShort();
  }
}
