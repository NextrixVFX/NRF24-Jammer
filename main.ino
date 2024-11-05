#include <RF24.h>
#include <nRF24L01.h>

// Pinout; Digital IO : Inland ESP32 Dev32
#define LED 13
#define CSN 0
#define CE 2

RF24 radio(CE, CSN);

// noise (can be just 0xFF aswell ig but might be less powerful)
const char payload[] = "00000000000";
// this will attack 2.400ghz - 2.485ghz [min - max]
// yes this also effects Bluetooth lol
const uint8_t MAX_CHANNELS = 85;
const uint8_t MIN_CHANNELS = 0;
// total loop amnt clamped
uint32_t amnt = 0 % 2;

void setup() {
  Serial.begin(115200);
  pinMode(LED, OUTPUT);

  // incase ur transmitter(s) aren't setup right.
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
    // Transmitting Mode
    module.stopListening();
    // Doesnt wait for a response from another transmitter
    module.setAutoAck(false);
    // Highest DBm
    module.setPALevel(RF24_PA_MAX);
    // 2MBPS is good for looping through all the channels FAST but you loose a lot of range / power
    module.setDataRate(RF24_2MBPS);
  }
}

void loop() {
  // jam every channel via channel hopping
  for (uint8_t channel = MIN_CHANNELS; channel <= MAX_CHANNELS; channel++) {
    radio.setChannel(channel);
    radio.write(&payload, sizeof(payload));
  }

  // led visual feedback
  digitalWrite(LED, amnt % 2 == 0 ? HIGH : LOW);
  amnt++;
}
