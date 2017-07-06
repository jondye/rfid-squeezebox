#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <FS.h>
#include <MFRC522.h>
#include <SPI.h>
#include <WiFiManager.h>

#define RST_PIN 2
#define SS_PIN  15
#define SPEAKER D2

const auto NOTE_C4 = 262u;
const auto NOTE_D4 = 294u;
const auto NOTE_E4 = 330u;
const auto NOTE_F4 = 349u;
const auto NOTE_G4 = 392u;
const auto NOTE_A4 = 440u;
const auto NOTE_B4 = 494u;
const auto NOTE_C5 = 523u;

MFRC522 mfrc522(SS_PIN, RST_PIN);

#define PLAYER_ID "00%3A04%3A20%3A17%3A81%3Ad5";
// Kitchen 00%3A04%3A20%3A26%3Ac6%3A4d
// Loft 00%3A04%3A20%3A07%3Acc%3Af1
// Living%20room 00%3A04%3A20%3A17%3A81%3Ad5
const int server_port = 9090;
const char server_host[] = "squeezebox";

void setup() {
	Serial.begin(115200);

	if (SPIFFS.begin()) {
		Serial.println(F("Filesystem mounted"));
	}
	FSInfo fs_info;
	if (SPIFFS.info(fs_info)) {
		Serial.println(F("Filesystem info:"));
		Serial.print(F("  totalBytes:"));
		Serial.println(fs_info.totalBytes);
		Serial.print(F("  usedBytes"));
		Serial.println(fs_info.usedBytes);
		Serial.print(F("  blockSize"));
		Serial.println(fs_info.blockSize);
		Serial.print(F("  pageSize"));
		Serial.println(fs_info.pageSize);
		Serial.print(F("  maxOpenFiles"));
		Serial.println(fs_info.maxOpenFiles);
		Serial.print(F("  maxPathLength"));
		Serial.println(fs_info.maxPathLength);
	}
	SPI.begin();
  SPI.setFrequency(1000000);
	mfrc522.PCD_Init();
	mfrc522.PCD_DumpVersionToSerial();
	Serial.println(F("Ready"));
}

void getUid(char * uid) {
  for (auto i = 0; i != mfrc522.uid.size; ++i) {
		sprintf(uid + 2*i, "%02X", mfrc522.uid.uidByte[i]);
  }
  Serial.print(F("Scanned UID: "));
  Serial.println(uid);
}

void ackSound() {
  tone(SPEAKER, NOTE_C4);
	delay(100);
  noTone(SPEAKER);
}

void successSound() {
  tone(SPEAKER, NOTE_C4);
  delay(100);
  tone(SPEAKER, NOTE_F4);
  delay(100);
  tone(SPEAKER, NOTE_C5);
  delay(100);
  noTone(SPEAKER);
}

void failSound() {
  tone(SPEAKER, NOTE_C4);
  delay(200);
  tone(SPEAKER, NOTE_C4);
	delay(200);
  noTone(SPEAKER);
}

void loop() {
	// Look for new cards
	if (!mfrc522.PICC_IsNewCardPresent()) {
		return;
	}

	// Select one of the cards
	if (!mfrc522.PICC_ReadCardSerial()) {
		return;
	}

	ackSound();

	char file_name[12] = "/";
	getUid(file_name + 1);

  mfrc522.PICC_HaltA();

	if (!SPIFFS.exists(file_name)) {
		Serial.println(F("No file found"));
		failSound();
		return;
	}

	File f = SPIFFS.open(file_name, "r");
	if (!f) {
		Serial.println(F("Unable to read file"));
		failSound();
		return;
	}

	const String tracks = f.readString();
	Serial.print(F("Tracks: "));
	Serial.println(tracks);

	f.close();

	successSound();

	WiFiClient client;
	if (!client.connect(server_host, server_port)) {
		Serial.println("squeezebox server connection failed");
		failSound();
		return;
	}

	// <player id> playlistcontrol cmd:load track_id:<comma separated list>
	char cmd[255] = PLAYER_ID;
	strncat(cmd, " playlistcontrol cmd:load track_id:", sizeof(cmd));
	strncat(cmd, tracks.c_str(), sizeof(cmd));

	Serial.print("Sending command: ");
	Serial.println(cmd);

	// This will send the request to the server
	client.print(cmd);
	unsigned long timeout = millis();
	while (client.available() == 0) {
		if (millis() - timeout > 5000) {
			Serial.println(F(">>> Client Timeout !"));
			client.stop();
			failSound();
			return;
		}
	}

	// Read all the lines of the reply from server and print them to Serial
	while(client.available()) {
		String line = client.readStringUntil('\r');
		Serial.print(line);
	}
}
