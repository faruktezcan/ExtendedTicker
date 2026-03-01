/**
 * ExtendedTicker – Example 02: repeatUntil
 *
 * Polls every 500 ms until a flag is set (simulated after 4 s),
 * then fires an onComplete callback.
 */

#include <ExtendedTicker.h>

ExtendedTicker ticker;
bool deviceReady = false;

void setup() {
  Serial.begin(115200);

  Serial.println("=== repeatUntil example ===");
  Serial.println("Device will become ready after ~4 seconds...");

  // Simulate the device becoming ready after 4 seconds using a one-shot timer.
  static Ticker readyTimer;
  readyTimer.once_ms(4000, []() {
    deviceReady = true;
    Serial.println("[sim] deviceReady = true");
  });

  // Poll every 500 ms until deviceReady == true.
  ticker.repeatUntil(500,
    []() -> bool {
      return deviceReady;           // stop condition
    },
    []() {
      Serial.printf("Waiting... (elapsed: %d polls)\n", ticker.elapsed());
    },
    []() {
      Serial.println("Device is ready! Stopping poll.");
    }
  );
}

void loop() {}
