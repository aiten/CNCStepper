void setup() 
{
  Serial.begin(250000);
  pinMode(2,INPUT_PULLUP);
  pinMode(3,INPUT_PULLUP);
  pinMode(4,INPUT_PULLUP);
  pinMode(5,INPUT_PULLUP);
  pinMode(6,INPUT_PULLUP);
  pinMode(7,INPUT_PULLUP);
  pinMode(8,INPUT_PULLUP);
  pinMode(9,INPUT_PULLUP);
  pinMode(10,INPUT_PULLUP);
  pinMode(11,INPUT_PULLUP);
  pinMode(12,INPUT_PULLUP);
  pinMode(13,INPUT_PULLUP);

  // A0 to turn on the LED
  // A1-A4 as GND

  pinMode(A0,OUTPUT);
  pinMode(A1,OUTPUT);
  pinMode(A2,OUTPUT);
  pinMode(A3,OUTPUT);
  pinMode(A4,OUTPUT);

  digitalWrite(A0,LOW);
  digitalWrite(A1,LOW);
  digitalWrite(A2,LOW);
  digitalWrite(A3,LOW);
  digitalWrite(A4,LOW);
}

int cnt;

#define CNT 12
#define X1 2
#define X10 3
#define X100 4

bool current[CNT] = { 0 };
bool old[CNT] = { 0 };

void loop() 
{
  for (int i = 0;i<CNT; i++)
  {
    current[i] = digitalRead(i+2)==LOW;
  }

  bool diff = false;
  for (int i = 0;i<CNT && !diff; i++)
  {
    diff = current[i] != old[i];
  }

  if (diff)
  {
    for (int i = 0;i<CNT; i++)
    {
      old[i] = current[i];
    }

    cnt++;

    digitalWrite(A0, current[X1] || current[X10] || current[X100]);

    Serial.print(cnt);
    Serial.print(" ");

    for (int i = 0;i<CNT; i++)
    {
      Serial.print(current[i]);
      if (i==1 || i==4 || i==5)
      {
        Serial.print(" ");
      }
    }

    Serial.println();
  }
}
