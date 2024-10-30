#include <Servo.h>

//#define DBUG
#define SwitchPin         5
#define AileronInPin      10
#define AileronMasterOut  11
#define AileronSlaveOut   12
#define FlapsOut          9
#define FlapDeflection    800
#define AileronDeflection 200

enum MODE {
  S_FLY = 0,
  S_SET = 1
};

enum SWITCH_POS {
  S_HI = 2,
  S_MED = 1,
  S_LOW = 0
};

Servo aileron_master_srv;
Servo aileron_slave_srv;
Servo flaps_srv;
unsigned long toggles[3] = {
  0, 0, 0                // keeps track of switch flips
};
SWITCH_POS cur_pos        = S_HI;
MODE       cur_mode       = S_FLY;
int        aileron_offset = AileronDeflection;

// 2040  - top
// 562.5 - center
// 915   - bottom




int limit_duty_cycle(int duty_cycle) {
  if (duty_cycle > 2040) return 2040;
  if (duty_cycle < 915)  return 915;
  return duty_cycle;
}

SWITCH_POS get_switch_pos(int duty_cycle) {
  if (duty_cycle > 1800 && duty_cycle < 2200) return S_HI;
  if (duty_cycle > 1300 && duty_cycle < 1700) return S_MED;
  if (duty_cycle > 700 && duty_cycle < 1200) return S_LOW;
}

void setup() {
#ifdef DBUG
  Serial.begin(9600);
#endif
  pinMode(SwitchPin, INPUT);
  pinMode(AileronInPin, INPUT);

  aileron_master_srv.attach(AileronMasterOut); 
  aileron_slave_srv.attach(AileronSlaveOut); 
  flaps_srv.attach(FlapsOut); 
}

void loop() {
  if (cur_mode == S_FLY) {
    int s_in = pulseIn(SwitchPin, HIGH);
    SWITCH_POS md = get_switch_pos(s_in);
    if (md != cur_pos) {
#ifdef DBUG
      Serial.println("Toggle");
      Serial.println(String(toggles[2]) +"," + String(toggles[1]) + "," + String(toggles[0]));
#endif

      cur_pos=md;
      toggles[2] = toggles[1];
      toggles[1] = toggles[0];
      toggles[0] = millis();
      if ( (toggles[2] != 0 && toggles[1] != 0 && toggles[0] != 0)
        && (toggles[0] - toggles[2] < 300)) {
        toggles[0] = 0;
        toggles[1] = 0;
        toggles[2] = 0;
        cur_mode = S_SET;
      }
    }
    int a_in = pulseIn(AileronInPin, HIGH);
#ifdef DBUG
    Serial.println(a_in);
#endif
  
    switch(md) {
      case S_LOW:
        flaps_srv.writeMicroseconds(limit_duty_cycle(FlapDeflection));
        aileron_master_srv.writeMicroseconds(limit_duty_cycle(a_in + aileron_offset));
        aileron_slave_srv.writeMicroseconds(limit_duty_cycle(a_in - aileron_offset));
        break;
      case S_MED:
        flaps_srv.writeMicroseconds(limit_duty_cycle(FlapDeflection));
        aileron_master_srv.writeMicroseconds(limit_duty_cycle(a_in));
        aileron_slave_srv.writeMicroseconds(limit_duty_cycle(a_in));
        break;
      case S_HI:
        flaps_srv.writeMicroseconds(1900);
        aileron_master_srv.writeMicroseconds(limit_duty_cycle(a_in));
        aileron_slave_srv.writeMicroseconds(limit_duty_cycle(a_in));
        break;
    }
  } else if (cur_mode == S_SET) {
    int s_in = pulseIn(SwitchPin, HIGH);
    SWITCH_POS md = get_switch_pos(s_in);
    int a_in = pulseIn(AileronInPin, HIGH);
    if (md == S_LOW) {
      cur_mode = S_FLY;
      aileron_offset = a_in - 1482;
      return;
    }
#ifdef DBUG
    Serial.println(a_in);
#endif
    // 1482 - centered
    aileron_master_srv.writeMicroseconds(limit_duty_cycle(a_in));
    aileron_slave_srv.writeMicroseconds(limit_duty_cycle(1482 + (1482 - a_in)));
  }
}
