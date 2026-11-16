#include "esp_dsp.h"

const uint32_t timeout_cycles = microsecondsToClockCycles(1);


void setup() 
{
  Serial.begin(115200);
  pinMode(LED_BUILTIN,OUTPUT);
}

void delayNanoSeconds(uint32_t nanoseconds)
{
  const uint32_t start_cycle_count = esp_cpu_get_cycle_count();
  const uint32_t until_cycle_count = start_cycle_count + ((nanoseconds*timeout_cycles)/1000);
  uint32_t  nowcount;

  while ((nowcount = esp_cpu_get_cycle_count())>=start_cycle_count && nowcount <= until_cycle_count)
  {

  }
}

void myDelay(uint nanoseconds)
{
  auto mymillis = millis();
  auto until = mymillis + nanoseconds; 
  int count = 0;

  for (count=0; millis() < until; count++)
  {
    delayNanoSeconds(1000000);
  }

  Serial.print(count);
  Serial.print(" => ");
}


void loop() 
{
  Serial.print("start  => ");
  myDelay(1000);
  digitalWrite(LED_BUILTIN,LOW);
  myDelay(500);
  digitalWrite(LED_BUILTIN,HIGH);
  Serial.println(" => done");
}
