#include <Servo.h>

#define DBUG
#define SwitchPin         5
#define AileronInPin      10
#define AileronMasterOut  11
#define AileronSlaveOut   12
#define FlapsOut          9
#define FlapDeflection    800
#define AileronDeflection 450
Servo aileron_master_srv;
Servo aileron_slave_srv;
Servo flaps_srv;

// 2040 - top

// 915 - bottom

enum SWITCH_MODE {
  S_HI = 2,
  S_MED = 1,
  S_LOW = 0
};

int limit_duty_cycle(int duty_cycle) {
  
}

SWITCH_MODE get_switch_mode(int duty_cycle) {
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
  int s_in = pulseIn(SwitchPin, HIGH);
  SWITCH_MODE md = get_switch_mode(s_in);
  int a_in = pulseIn(AileronInPin, HIGH);
#ifdef DBUG
  Serial.println(a_in);
#endif

  switch(md) {
    case S_LOW:
      flaps_srv.writeMicroseconds(FlapDeflection);
      aileron_master_srv.writeMicroseconds(a_in + AileronDeflection);
      aileron_slave_srv.writeMicroseconds(a_in - AileronDeflection);
      break;
    case S_MED:
      flaps_srv.writeMicroseconds(FlapDeflection);
      aileron_master_srv.writeMicroseconds(a_in);
      aileron_slave_srv.writeMicroseconds(a_in);
      break;
    case S_HI:
      flaps_srv.writeMicroseconds(1900);
      aileron_master_srv.writeMicroseconds(a_in);
      aileron_slave_srv.writeMicroseconds(a_in);
      break;
  }
}
