/**
 * ExtendedTicker – Example 06: retry pattern
 *
 * A real-world pattern: attempt to connect to WiFi up to 5 times,
 * every 3 seconds. Stop early on success; report failure after all
 * attempts are exhausted via onComplete.
 */

#include <ExtendedTicker.h>
// #include <ESP8266WiFi.h>   // uncomment for real WiFi use

ExtendedTicker ticker;
bool connected = false;
int  simSuccessOnAttempt = 3;   // simulate success on the 3rd attempt

void setup() {
  Serial.begin(115200);

  Serial.println("=== retry pattern example ===");
  Serial.println("Attempting connection (max 5 tries, every 3 s)...\n");

  // ── Replace the simulated block below with real WiFi code ─────────────────
  // WiFi.begin("ssid", "password");

  ticker.repeatN(3000, 5,
    []() {
      int attempt = ticker.elapsed() + 1;
      Serial.printf("Attempt %d / 5 ...\n", attempt);

      // ── Simulated connect ────────────────────────────────────────────────
      if (attempt == simSuccessOnAttempt) {
        connected = true;
      }

      // ── Real WiFi check would be: ────────────────────────────────────────
      // connected = (WiFi.status() == WL_CONNECTED);

      if (connected) {
        Serial.printf("Connected on attempt %d!\n", attempt);
        ticker.detach();   // stop early – don't wait for the remaining tries
      }
    },
    []() {
      // onComplete fires only when all 5 attempts were exhausted without
      // an early detach(), i.e. all failed.
      if (!connected) {
        Serial.println("Failed to connect after 5 attempts. Giving up.");
      }
    }
  );
}

void loop() {}
