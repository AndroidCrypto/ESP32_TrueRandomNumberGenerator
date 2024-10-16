/*
  This example explains how to get True Random Numbers on an ESP32 device (True RNG).

  Why is it important to get these 'True' Random numbers ? For some cryptographic methods it is CRUCIAL
  that a value is random or unpredictable, meaning an unwanted listener ("hacker") cannot get this number
  when getting encrypted data. Some cryptography algorithms like "AES GCM" or "ASCON 128" require a "Nonce"
  (= number used just once) as parameter. If the same nonce is used twice on the same data origin the attacker
  is being abled to decrypt the complete encryption data from this device.
  As the regular getRandom() method may produce "pseudo-random numbers" the effect is horrible: you cannot 
  distinguish between the "true" or "pseudo" numbers but an attacker can. Especially when the RF subsystem on
  an ESP32 is completely off (meaning neither Wi-Fi nor Bluetooth is activated, e.g. for power saving reasons) 
  this may happen.
  
  I'm using the described example in my encrypted LoRa transmission examples, because the LoRa sender is just
  reading values from a sensor (e.g. a BME280 temperature, humditiy and air pressure sensor module) and transmitting
  the data from the sender via LoRa to the receiver - no Wi-Fi or Bluetooth is involved in this process.
*/

#include "bootloader_random.h" // for true random RNG
 
void setup() {
  /*
    https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf#rng

    Important: DO NOT RUN bootloader_random_enable() when you are planning to activate Wi-Fi or Bluetooth in your app
    This function is not safe to use if any other subsystem is accessing the RF subsystem or the ADC at the same time!
    see: https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-reference/system/random.html#_CPPv424bootloader_random_enablev

    Please do not connect any sensors to ADC1 or ADC2 pins
    for reference see https://i0.wp.com/randomnerdtutorials.com/wp-content/uploads/2018/08/ESP32-DOIT-DEVKIT-V1-Board-Pinout-36-GPIOs-updated.jpg?quality=100&strip=all&ssl=1
  */

  /*
    Enable an entropy source for RNG if RF subsystem is disabled.
    The exact internal entropy source mechanism depends on the chip in use but all SoCs use the SAR ADC to continuously mix 
    random bits (an internal noise reading) into the HWRNG. Consult the SoC Technical Reference Manual for more information.

    SAR ADC: ESP32 integrates two 12-bit SAR (“Successive Approximation Register”) ADCs (Analog to Digital Converters) and 
    supports measurements on 18 channels (analog enabled pins).

    Can also be called from app code, if true random numbers are required without initialized RF subsystem. This might be 
    the case in early startup code of the application when the RF subsystem has not started yet or if the RF subsystem should 
    not be enabled for power saving.

    Consult ESP-IDF Programming Guide "Random Number Generation" section for details.

    Warning
    This function is not safe to use if any other subsystem is accessing the RF subsystem or the ADC at the same time!

    use bootloader_random_disable() when you are going to run RF subsystem or the ADC

    Disable entropy source for RNG.
    Disables internal entropy source. Must be called after bootloader_random_enable() and before RF subsystem features, 
    ADC, or I2S (ESP32 only) are initialized.
    Consult the ESP-IDF Programming Guide "Random Number Generation" section for details.
  */

  Serial.begin(115200);
  delay(1000);
  Serial.println(F("ESP32 True Random Number Generator"));
  
  bootloader_random_enable();

  const int nonceLength = 16;
  uint8_t nonceRandom[nonceLength];

  for (int i = 0; i < 10; i++) {
    bootloader_fill_random(nonceRandom, nonceLength);
    //esp_fill_random(aesGcmNonce, ascon128NonceLength);
    Serial.print(i); Serial.print(": ");
    Serial.println(printData("Nonce",nonceRandom, nonceLength));
  }

}

void loop() {
}

String printData(String dataName, uint8_t *data, int dataLength) {
  return dataName + F(" length: ") + String(dataLength) + F(" data: ") + uint8ArrayToHexString(data, dataLength);
}

const char base36Chars[36] PROGMEM = { '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M', 'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z' };
String uint8ArrayToHexString(const uint8_t *uint8Array, const uint32_t arrayLength) {
  String hexString;
  if (!hexString.reserve(2 * arrayLength))  // Each uint8_t will become two characters (00 to FF)
  {
    return emptyString;
  }

  for (uint32_t i = 0; i < arrayLength; ++i) {
    hexString += (char)pgm_read_byte(base36Chars + (uint8Array[i] >> 4));
    hexString += (char)pgm_read_byte(base36Chars + uint8Array[i] % 16);
  }

  return hexString;
}