struct gasMeter 
{
    String numberSerial;
    int pinHigh;
    int pinLow;
    int pinValve;
    int count = 0;
    float volumeMeasurement = 15.0;
    bool measurementHigh[4];
    bool measurementLow[4];
    bool statusHigh = 0;
    bool statusLow = 0;
    int statusCalibrating = 0;
    int errorCode = 0;
    int stateMachine = 0;
};
struct gasMeter sensor1;



/*serial communication*/
const byte NUMCHARS = 64;
char _receivedChars[NUMCHARS];
boolean _newMsgReceived = false;

void setZeroSensor1() {
  sensor1.count = 0;
  Serial.println("<b>");
}

void setCalibratingSensor1(int s){
  sensor1.statusCalibrating = s;
}

void measurementHighSensor1(){
  sensor1.measurementHigh[3] = sensor1.measurementHigh[2];
  sensor1.measurementHigh[2] = sensor1.measurementHigh[1];
  sensor1.measurementHigh[1] = sensor1.measurementHigh[0];
  sensor1.measurementHigh[0] = digitalRead(sensor1.pinHigh);
  if((sensor1.measurementHigh[3]==0)&&(sensor1.measurementHigh[2]==0)&&(sensor1.measurementHigh[1]==1)&&(sensor1.measurementHigh[0]==1)){
    sensor1.statusHigh = true;
  }
  sensor1.statusHigh = false;
}

void measurementLowSensor1(){
  sensor1.measurementLow[3] = sensor1.measurementLow[2];
  sensor1.measurementLow[2] = sensor1.measurementLow[1];
  sensor1.measurementLow[1] = sensor1.measurementLow[0];
  sensor1.measurementLow[0] = digitalRead(sensor1.pinHigh);
  if((sensor1.measurementLow[3]==0)&&(sensor1.measurementLow[2]==0)&&(sensor1.measurementLow[1]==1)&&(sensor1.measurementLow[0]==1)){
    sensor1.statusLow = true;
  }
  sensor1.statusLow = false;
}

void stateMachineSensor1(){
  if (sensor1.statusCalibrating == 0){
    if ((sensor1.stateMachine == 0) && (digitalRead(sensor1.pinValve) == false)){
      sensor1.stateMachine = 1;
    }
    else if ((sensor1.stateMachine == 1) && (sensor1.statusHigh == true)){
      digitalWrite(sensor1.pinValve, true);
      sensor1.stateMachine = 2;
      sensor1.count = sensor1.count + 1;
    }
    else if ((sensor1.stateMachine == 2) && (sensor1.statusLow == false)){
      digitalWrite(sensor1.pinValve, false);
      sensor1.stateMachine = 1;
    }
  } else {
    digitalWrite(sensor1.pinValve, false);
    sensor1.stateMachine = 0;
  }
}

/*
* Attention: Two static variables ... sometimes not all messages arrive at once. 
* It is possible to compose the message in more than one cycle of the loop
*/
void getSerialMessage() {
  static boolean recvInProgress = false;
  static byte j = 0;
  char startMarker = '<';
  char endMarker = '>';
  char c;

  while (Serial.available() > 0 && _newMsgReceived == false) {
    c = Serial.read();

    if (recvInProgress == true) {
      if (c != endMarker) {
        _receivedChars[j] = c;
        j++;
        if (j >= NUMCHARS) {
            j = NUMCHARS - 1;
        }
      } else {
        _receivedChars[j] = '\0'; // terminate the string
        recvInProgress = false;
        j = 0;
        _newMsgReceived = true;
      }
    }

    else if (c == startMarker) {
      recvInProgress = true;
    }
  }
}

void sendAllData(){
  String s = "<a;"+String(sensor1.count)+">";
  Serial.println(s);
}

void decodeRequest()
{    
  String s;

  char cmd = _receivedChars[0];

  switch(cmd)
  {
    case '?': // echo
      Serial.println("<?>");
      break;

    case 'a': // sends string with various information for system monitoring
      sendAllData();
      break;
    case 'b': // sends string with various information for system monitoring
      setZeroSensor1();
      break;
    case 'c': // sends string with various information for system monitoring
      setCalibratingSensor1(1);
      break;
    case 'd': // sends string with various information for system monitoring
      setCalibratingSensor1(0);
      break;
    default:
      Serial.println("<?>");
      break;
  }    
}

// the setup routine runs once when you press reset:
void setup() {
  sensor1.pinHigh = 2;
  sensor1.pinLow = 3;
  sensor1.pinValve = 4;
  Serial.begin(115200);
  pinMode(sensor1.pinHigh,INPUT);
  pinMode(sensor1.pinLow,INPUT);
  pinMode(sensor1.pinValve,OUTPUT);

}

void loop() {
  getSerialMessage();
  if (_newMsgReceived == true) {      
      decodeRequest();    
      _newMsgReceived = false;
  }

  measurementHighSensor1();
  measurementLowSensor1();
  stateMachineSensor1();
}
