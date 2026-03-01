/**
 * ExtendedTicker – Example 03: once_after
 *
 * Waits 2 seconds (e.g. for WiFi to stabilise), then polls every 1 second.
 * Automatically stops after 5 polls using elapsed().
 */

#include <ExtendedTicker.h>

ExtendedTicker ticker;

void setup() {
  Serial.begin(115200);

  Serial.println("=== once_after example ===");
  Serial.println("Waiting 2 s before first poll...");

  // Wait 2 s, then fire every 1 s. Stop after 5 polls.
  ticker.once_after(2000, 1000, [&]() {
    Serial.printf("Poll #%d\n", ticker.elapsed());

    if (ticker.elapsed() >= 5) {
      ticker.detach();
      Serial.println("Reached 5 polls – stopped.");
    }
  });
}

void loop() {}
