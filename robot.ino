const int PWMA=11; // Pololu drive A
const int AIN2=10;
const int AIN1 =9;
const int STDBY=8;
const int BIN1 =7; // Pololu drive B
const int BIN2 =6;
const int PWMB =5;

const int SL = A1;
const int SM = A2;
const int SR = A3;
int numWhiteLines = 0;
bool isWhite = false;
void setup()
{
  Serial.begin(9600);
pinMode(PWMA, OUTPUT);
pinMode(AIN1, OUTPUT);
pinMode(AIN2, OUTPUT);
pinMode(BIN1, OUTPUT);
pinMode(BIN2, OUTPUT);
pinMode(PWMB, OUTPUT);
pinMode(STDBY, OUTPUT);
digitalWrite(STDBY, HIGH);
}
void loop()
{
//drive(100,100); //drive forwards
int output_left = analogRead(A1);
int output_middle = analogRead(A2);
int output_right = analogRead(A3);
Serial.println("LEFT");
Serial.println(output_left);
Serial.println("MIDDLE");
Serial.println(output_middle);
Serial.println("RIGHT");
Serial.println(output_right);
//delay(1000);
control();
}

const int HIGH_VALUE = 750;
void motorWrite(int spd, int pin_IN1, int pin_IN2, int pin_PWM)
{
if (spd < 0)
{
digitalWrite(pin_IN1, HIGH); // go one way
digitalWrite(pin_IN2, LOW);
}
else
{
digitalWrite(pin_IN1, LOW); // go the other way
digitalWrite(pin_IN2, HIGH);
}
analogWrite(pin_PWM, abs(spd));
}
// your drive() function goes here!

void drive(int speed_left, int speed_right){
  motorWrite(speed_left,AIN1,AIN2,PWMA);
  motorWrite(speed_right,BIN1,BIN2,PWMB);
}

//const int TARGET_VALUE = 1000;
int target_value;
int left_diff_previous  = 0;
int left_diff_current = 0;
int right_diff_previous = 0;
int right_diff_current = 0;
int integral_left = 0;
int integral_right = 0;
const float Kp = 0.3;
const float Ki = 1;
const float Kd = 2;
const float delta_T = 0.01;
void control(){
  int output_left = analogRead(A1);
  int output_middle = analogRead(A2);
  int output_right = analogRead(A3);

    target_value = 1000;
    left_diff_previous = left_diff_current;
   
    right_diff_previous = left_diff_previous;
    left_diff_current = target_value - output_left;
    right_diff_current = target_value - output_right;

    int modified_speed_left_p = constrain(left_diff_current*Kp,0,255);
    int modified_speed_right_p = constrain(right_diff_current*Kp,0,255);
   
    integral_left = integral_left + ((left_diff_previous + left_diff_current)/2) * delta_T;
    integral_right = integral_right + ((right_diff_previous + right_diff_current)/2) * delta_T;
   
    if(output_left > HIGH_VALUE && output_right > HIGH_VALUE && output_middle > HIGH_VALUE){
        integral_left = 0;
        integral_right = 0;
    }
    int modified_speed_left_i = constrain(integral_left*Ki,0,255);
    int modified_speed_right_i= constrain(integral_right*Ki,0,255);
    int modified_speed_left_d = constrain(Kd*(left_diff_previous - left_diff_current)/delta_T,0,255);
    int modified_speed_right_d = constrain(Kd*(right_diff_previous - right_diff_current)/delta_T,0,255);
    if(output_left > HIGH_VALUE && output_right < HIGH_VALUE){
        modified_speed_left_p = 0;
        modified_speed_left_i = 0;
        modified_speed_left_d = 0;
        Serial.println(0);
    }
    else if(output_left < HIGH_VALUE && output_right > HIGH_VALUE){
      modified_speed_right_p = 0;
      modified_speed_right_i = 0;
      modified_speed_right_d = 0;
    }
    int speed_total_left = constrain(modified_speed_left_p + modified_speed_left_i + modified_speed_left_d,0,255);
    int speed_total_right = constrain(modified_speed_right_p + modified_speed_right_i + modified_speed_right_d,0,255);
 

  if (isWhite) {
    Serial.println("white line go");
    drive(200,200);
    if (output_left > HIGH_VALUE || output_right > HIGH_VALUE || output_middle > HIGH_VALUE){  
      numWhiteLines=0;
      isWhite = false;
    }
  }
  else if(output_left > HIGH_VALUE && output_right > HIGH_VALUE){
    //drive(60,60);
    Serial.println(speed_total_left + 70);
    Serial.println(speed_total_right  + 70);
    drive(speed_total_right+50,speed_total_left+50);
    Serial.println("Straight");
  }
  else if(output_left < HIGH_VALUE && output_right < HIGH_VALUE && output_middle < HIGH_VALUE){
    numWhiteLines++;
    drive(-50,-50);
    Serial.println("WHITE");
    Serial.println(numWhiteLines);
    if (numWhiteLines>=40) {
      isWhite=true;
    }
  }
  else if(output_left > HIGH_VALUE && output_right < HIGH_VALUE && !isWhite){
    Serial.println(speed_total_left);
    Serial.println(speed_total_right);
    drive(speed_total_right+40,speed_total_left + 20 );
  }
  else if(output_left < HIGH_VALUE && output_right > HIGH_VALUE && !isWhite){
    Serial.println(speed_total_left);
    Serial.println(speed_total_right);
    drive(speed_tot
    al_right + 20,speed_total_left + 40);
  }
  delay(1);
}
