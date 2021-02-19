#define E_OK 0
#define E_NOT_OK -1
char buf[10];
typedef enum {
  SW_enStateMachineOff,
  SW_enStateMachineRead,
  SW_enStateMachineUpdate,
  SW_enStateMachineAcknowledge
}SW_StateMachineState_ten;

static uint8_t SW_SwitchToPinMap[] = {
  2,3,4,5,6,7,8,9
};
static SW_StateMachineState_ten eState = SW_enStateMachineOff;
static int32_t incomingByte = 0x0;
static uint8_t u8SwState = 0x0;

void SW_Off(uint8_t sw_nr) {
  pinMode(SW_SwitchToPinMap[sw_nr], INPUT);
  digitalWrite(SW_SwitchToPinMap[sw_nr], LOW);
}

void SW_On(uint8_t sw_nr) {
  pinMode(SW_SwitchToPinMap[sw_nr], OUTPUT);
}

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  switch(eState) {
    case SW_enStateMachineOff:
      if(Serial) {
        eState = SW_enStateMachineRead;
      } else {
        eState = SW_enStateMachineOff;
      }
      break;
    case SW_enStateMachineRead:
      if(SerialTask() == E_OK) {
        u8SwState = (uint8_t)(incomingByte & 0xFF);
        eState = SW_enStateMachineUpdate;
      } else {
        eState = SW_enStateMachineRead;
      }
      break;
    case SW_enStateMachineUpdate:
      GPIOTask();
      eState = SW_enStateMachineAcknowledge;
      break;
    case SW_enStateMachineAcknowledge:
      Serial.print("OK\r\n");
      eState = SW_enStateMachineRead;
      break;
    default:
      break;
  }
}

int32_t SerialTask(void) {
  int32_t result = E_NOT_OK;
  if(Serial.available() > 0) {
    incomingByte = Serial.read();
    if(incomingByte >=0) {
      result = E_OK;
    }
  }
  return result;
}

void GPIOTask(void) {
  for(uint8_t i = 0 ; i < 8; i++) {
    if(((u8SwState >> i) & 0x1)) {
      SW_On(i);
    } else {
      SW_Off(i);
    }
  }
}
