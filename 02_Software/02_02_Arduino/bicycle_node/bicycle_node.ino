
//pins
const int8_t LEFT_LED_PIN  = 2;
const int8_t RIGHT_LED_PIN = 4;
const int8_t BUZZER_PIN = 10;
const int8_t RSSI_PIN = 3;

//variables
const int16_t DELAY_TIME = 100;
const int16_t BUZZER_VOL = 110;
const int32_t PWM_PERIOD_US = 2000;


void setup() {
  pinMode(LEFT_LED_PIN,  OUTPUT);
  pinMode(RIGHT_LED_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);

  Serial.begin(9600);
   
}

//gets the duty cylce of the pwm signal on the rssi pin
static int16_t get_PWM_duty_cycle(){
  //init
  int32_t pwm_reading = 0;
  int32_t pwm_duty_cycle = 0;
  
  //read the pwm duty cycle
  pwm_reading = pulseIn(RSSI_PIN,HIGH, 2000);  

  //Serial.println(pwm_reading);
  //handle no rising/falling edge case
  if(pwm_reading == 0){
    if(digitalRead(RSSI_PIN) == HIGH){
      pwm_reading = PWM_PERIOD_US;
    }
    else{
      pwm_reading = 0;
    }
  }
  
  //convert to percentage
  pwm_duty_cycle = (pwm_reading * 100) / PWM_PERIOD_US;
  return pwm_duty_cycle;
}

static void test_bicycle(){
  //turn ON status leds
  digitalWrite(LEFT_LED_PIN,  HIGH);
  digitalWrite(RIGHT_LED_PIN, LOW);
  analogWrite(BUZZER_PIN, BUZZER_VOL);
  delay(DELAY_TIME);  

  //turn OFF status leds
  digitalWrite(LEFT_LED_PIN,  LOW);
  digitalWrite(RIGHT_LED_PIN, HIGH);
  analogWrite(BUZZER_PIN, BUZZER_VOL);
  
  delay(DELAY_TIME);
}

void loop() {
  
  //get rssi data:
//  Serial.print("PWM Duty Cycle: ");
//  Serial.print(get_PWM_duty_cycle());
//  Serial.println("%");
//  test_bicycle();

//  if(Serial.available()){
//    Serial.print((char)Serial.read());
//  }
  Serial.write("+++");
  delay(100);
    while(Serial.available()){
    Serial.print((char)Serial.read());
  }
  delay(1000);
  Serial.write("ATDB\r");
  delay(100);
    while(Serial.available()){
    Serial.print((char)Serial.read());
  }
  Serial.write("ATCN\r");
  delay(100);
  while(Serial.available()){
    Serial.print((char)Serial.read());
  }
  delay(1000);
  
  
}
 
