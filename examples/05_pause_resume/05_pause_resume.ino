/**
 * ExtendedTicker – Example 05: pause / resume
 *
 * Starts a 10-tick series. Every 3 seconds the series is toggled
 * between paused and running so you can observe remaining/elapsed.
 */

#include <ExtendedTicker.h>

ExtendedTicker ticker;

bool     isPaused    = false;
uint32_t lastToggle  = 0;

void setup() {
  Serial.begin(115200);

  Serial.println("=== pause / resume example ===");
  Serial.println("Series will pause/resume every 3 seconds.\n");

  ticker.repeatN(600, 10,
    []() {
      Serial.printf("[tick]  elapsed=%-2d  remaining=%-2d\n",
                    ticker.elapsed(), ticker.remaining());
    },
    []() {
      Serial.println("[done]  All 10 ticks complete.");
    }
  );
}

void loop() {
  if (millis() - lastToggle >= 3000) {
    lastToggle = millis();
    isPaused   = !isPaused;

    if (isPaused) {
      ticker.pause();
      Serial.printf("\n--- PAUSED  (elapsed=%d  remaining=%d) ---\n\n",
                    ticker.elapsed(), ticker.remaining());
    } else {
      ticker.resume();
      Serial.printf("\n--- RESUMED (elapsed=%d  remaining=%d) ---\n\n",
                    ticker.elapsed(), ticker.remaining());
    }
  }
}
