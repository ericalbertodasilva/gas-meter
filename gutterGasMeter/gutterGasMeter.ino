struct gasMeter {
    String numberSerial;
    int pin;
    int count;
    float volumeMeasurement = 15.0;
    bool measurement[4];
    int errorCode = 0;
};

gasMeter sensor1;
/*serial communication*/
const byte NUMCHARS = 64;
char _receivedChars[NUMCHARS];
boolean _newMsgReceived = false;

void setZeroSensor1() {
    sensor1.count = 0;
    Serial.println("<b>");
}

void measurementSensor1(){
    sensor1.measurement[3] = sensor1.measurement[2];
    sensor1.measurement[2] = sensor1.measurement[1];
    sensor1.measurement[1] = sensor1.measurement[0];
    sensor1.measurement[0] = digitalRead(sensor1.pin);
    if((sensor1.measurement[3]==0)&&(sensor1.measurement[2]==0)&&(sensor1.measurement[1]==1)&&(sensor1.measurement[0]==1)){
        sensor1.count = sensor1.count + 1;
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
        default:
        Serial.println("<?>");
        break;
    }    
}

// the setup routine runs once when you press reset:
void setup() {
  sensor1.pin = 5;
  Serial.begin(115200);
  pinMode(sensor1.pin,INPUT);
}

void loop() {
    getSerialMessage();
    if (_newMsgReceived == true) {      
        decodeRequest();    
        _newMsgReceived = false;
    }
    measurementSensor1();
}
