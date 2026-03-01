/**
 * ExtendedTicker – Example 01: repeatN
 *
 * Demonstrates firing a callback exactly N times, with and without
 * an onComplete callback, and using float (seconds) intervals.
 */

#include <ExtendedTicker.h>

ExtendedTicker ticker;

void setup() {
  Serial.begin(115200);
  pinMode(LED_BUILTIN, OUTPUT);

  Serial.println("=== repeatN examples ===");

  // ── 1. Basic: blink LED 6 times every 300 ms ─────────────────────────────
  ticker.repeatN(300, 6, []() {
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    Serial.printf("blink %d/6\n", ticker.elapsed());
  });

  // ── 2. With onComplete ─────────────────────────────────────────────────────
  // Uncomment to try:
  /*
  ticker.repeatN(1000, 3,
    []() { Serial.printf("tick %d/3\n", ticker.elapsed()); },
    []() { Serial.println("All 3 ticks done!"); }
  );
  */

  // ── 3. Float interval (seconds) ───────────────────────────────────────────
  // Uncomment to try:
  /*
  ticker.repeatN(0.5f, 4,
    []() { Serial.printf("tick %d/4 (0.5 s interval)\n", ticker.elapsed()); },
    []() { Serial.println("Done."); }
  );
  */
}

void loop() {}
