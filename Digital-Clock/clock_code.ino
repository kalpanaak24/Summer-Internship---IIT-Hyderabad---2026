const byte segmentPins[7] = {2, 3, 4, 5, 6, 7, 8};
const byte displayPins[4] = {9, 10, 11, 12};

const byte btnHour   = A0;
const byte btnMinute = A1;
const byte btnMode   = A2;

const byte segmentMap[10][7] =
{
  {0,0,0,0,0,0,1},
  {1,0,0,1,1,1,1},
  {0,0,1,0,0,1,0},
  {0,0,0,0,1,1,0},
  {1,0,0,1,1,0,0},
  {0,1,0,0,1,0,0},
  {0,1,0,0,0,0,0},
  {0,0,0,1,1,1,1},
  {0,0,0,0,0,0,0},
  {0,0,0,0,1,0,0}
};

int hh = 0;
int mm = 0;
int ss = 0;

bool clockStarted = false;
bool secView = false;

unsigned long lastTick = 0;
unsigned long holdStart = 0;
unsigned long secViewStart = 0;

bool holdFlag = false;

bool oldHourBtn = HIGH;
bool oldMinuteBtn = HIGH;
bool oldModeBtn = HIGH;

void setup()
{
  for(byte i = 0; i < 7; i++)
    pinMode(segmentPins[i], OUTPUT);

  for(byte i = 0; i < 4; i++)
    pinMode(displayPins[i], OUTPUT);

  pinMode(btnHour, INPUT_PULLUP);
  pinMode(btnMinute, INPUT_PULLUP);
  pinMode(btnMode, INPUT_PULLUP);
}

void loop()
{
  readButtons();

  if(clockStarted)
  {
    updateClock();
    handleSecondDisplay();
  }

  refreshDisplay();
}

void readButtons()
{
  bool hState = digitalRead(btnHour);
  bool mState = digitalRead(btnMinute);
  bool sState = digitalRead(btnMode);

  if(!clockStarted)
  {
    if(oldHourBtn == HIGH && hState == LOW)
      hh = (hh + 1) % 24;

    if(oldMinuteBtn == HIGH && mState == LOW)
      mm = (mm + 1) % 60;

    if(oldModeBtn == HIGH && sState == LOW)
    {
      clockStarted = true;
      lastTick = millis();
    }
  }

  oldHourBtn = hState;
  oldMinuteBtn = mState;
  oldModeBtn = sState;
}

void updateClock()
{
  while(millis() - lastTick >= 1000)
  {
    lastTick += 1000;

    ss++;

    if(ss == 60)
    {
      ss = 0;
      mm++;

      if(mm == 60)
      {
        mm = 0;
        hh++;
      }

      if(hh == 24)
        hh = 0;
    }
  }
}

void handleSecondDisplay()
{
  if(digitalRead(btnMode) == LOW)
  {
    if(!holdFlag)
    {
      holdStart = millis();
      holdFlag = true;
    }

    if(millis() - holdStart >= 1500)
    {
      secView = true;
      secViewStart = millis();
    }
  }
  else
  {
    holdFlag = false;
  }

  if(secView && millis() - secViewStart >= 5000)
  {
    secView = false;
  }
}

void loadSegments(byte num)
{
  for(byte i = 0; i < 7; i++)
  {
    digitalWrite(segmentPins[i], segmentMap[num][i]);
  }
}

void refreshDisplay()
{
  int data[4];

  if(secView)
  {
    data[0] = mm / 10;
    data[1] = mm % 10;
    data[2] = ss / 10;
    data[3] = ss % 10;
  }
  else
  {
    data[0] = hh / 10;
    data[1] = hh % 10;
    data[2] = mm / 10;
    data[3] = mm % 10;
  }

  for(byte digit = 0; digit < 4; digit++)
  {
    for(byte i = 0; i < 4; i++)
      digitalWrite(displayPins[i], LOW);

    loadSegments(data[digit]);

    digitalWrite(displayPins[digit], HIGH);

    delay(3);
  }
}
