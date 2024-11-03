#include <SPI.h>
#include <RF24.h>
#include <nRF24L01.h>

// Pinout; Digital IO
#define LED 13
#define CSN 0
#define CE 2

RF24 radio(2, 0);  // First transmitter on pins CE = 2, CSN = 0
// RF24 radio2(4, 16);  // Second transmitter on pins CE = 4, CSN = 16

const char payload[] = "00000000000";
const uint8_t MAX_CHANNELS = 85;
const uint8_t MIN_CHANNELS = 0;
uint32_t amnt = 0 % 2;

uint8_t writeRegister(uint8_t reg, uint8_t value){
  uint8_t status;
  digitalWrite(CSN, LOW);
  status = SPI.transfer( W_REGISTER | ( REGISTER_MASK & reg ) );
  SPI.transfer(value);
  digitalWrite(CSN, HIGH);
  return status;
}

void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);

  if (!radio.begin()) {
    while (true) {
      Serial.println("Initialization failed");
      digitalWrite(LED, HIGH);
      delay(250);
      digitalWrite(LED, LOW);
      delay(250);
    }
  }

  // incase you want to have more transmitters on different pinouts to cover more channels continuously
  for (RF24 module : {radio}) {
    module.stopListening();
    module.setAutoAck(false);
    
    module.setPALevel(RF24_PA_MAX);
    module.setDataRate(RF24_2MBPS);
  }

  SPI.begin();
  SPI.setDataMode(SPI_MODE0);
  SPI.setClockDivider(SPI_CLOCK_DIV2);
  SPI.setBitOrder(MSBFIRST);

  writeRegister(EN_AA, 0x0);
  writeRegister(RF_SETUP, 0x0F);
}

void loop() {
  for (uint8_t channel = MIN_CHANNELS; channel <= MAX_CHANNELS; channel++) {
    radio.setChannel(channel);
    radio.write(&payload, sizeof(payload));
  }

  digitalWrite(LED, amnt % 2 == 0 ? HIGH : LOW);

  amnt++;
}
