/**
 * ExtendedTicker – Example 04: repeatFor
 *
 * Samples a (simulated) sensor every 200 ms for a 3-second window,
 * then prints a summary via onComplete.
 */

#include <ExtendedTicker.h>

ExtendedTicker ticker;
float sampleSum = 0;

void setup() {
  Serial.begin(115200);

  Serial.println("=== repeatFor example ===");
  Serial.println("Sampling for 3 seconds at 200 ms intervals...");

  ticker.repeatFor(3000, 200,
    []() {
      // Simulate a sensor reading between 20.0 and 25.0
      float reading = 20.0f + (float)(millis() % 500) / 100.0f;
      sampleSum += reading;
      Serial.printf("Sample %2d: %.2f\n", ticker.elapsed(), reading);
    },
    []() {
      int n = ticker.elapsed();
      Serial.printf("Done. %d samples, average = %.2f\n",
                    n, n > 0 ? sampleSum / n : 0);
    }
  );
}

void loop() {}
