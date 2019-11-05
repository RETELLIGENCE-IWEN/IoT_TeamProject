/* Example taking test values from node.js server as input as well... */

#include "AESLib.h"

AESLib aesLib;

String plaintext = "12345678;";

char cleartext[256];
char ciphertext[512];

// AES Encryption Key
byte aes_key[] = { 0x2B, 0x7E, 0x15, 0x16, 0x28, 0xAE, 0xD2, 0xA6, 0xAB, 0xF7, 0x15, 0x88, 0x09, 0xCF, 0x4F, 0x3C };

// General initialization vector (you must use your own IV's in production for full security!!!)
byte aes_iv[N_BLOCK] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };


String encrypt(char * msg, byte iv[]) {
  int msgLen = strlen(msg);
  char encrypted[2 * msgLen];
  aesLib.encrypt64(msg, encrypted, aes_key, iv);
  return String(encrypted);
}

String decrypt(char * msg, byte iv[]) {
  unsigned long ms = micros();
  int msgLen = strlen(msg);
  char decrypted[msgLen]; // half may be enough
  aesLib.decrypt64(msg, decrypted, aes_key, iv);
  return String(decrypted);
}


void setup() {
  Serial.begin(115200);
}

void loop() {

  Serial.println("ENCRYPTING :::: m001-t001-00:11:22:33:FF:EE--44:55:66:77:FF:AA-k");
  // Encrypt
  byte enc_iv[N_BLOCK] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // iv_block gets written to, provide own fresh copy...
  String encrypted = encrypt("m001-t001-00:11:22:33:FF:EE--44:55:66:77:FF:AA-k", enc_iv);
  sprintf(ciphertext, "%s", encrypted.c_str());
  Serial.print("Ciphertext: ");
  Serial.println(encrypted);

  // Decrypt
  byte dec_iv[N_BLOCK] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 }; // iv_block gets written to, provide own fresh copy...
  String decrypted = decrypt(ciphertext, dec_iv);
  Serial.print("Cleartext: ");
  Serial.println(decrypted);

  delay(5000);
}
