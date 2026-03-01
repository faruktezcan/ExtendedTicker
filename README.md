# ExtendedTicker

An extended Arduino `Ticker` library for **ESP8266** and **ESP32** that adds powerful scheduling features on top of the built-in `Ticker` class.

---

## Features

| Method | Description |
|---|---|
| `repeatN()` | Fire a callback exactly N times, then stop |
| `repeatUntil()` | Repeat until a condition function returns `true` |
| `once_after()` | Wait a delay, then repeat indefinitely |
| `repeatFor()` | Repeat for a fixed wall-clock duration |
| `pause()` / `resume()` | Pause and resume any running series |
| `remaining()` | How many ticks are left in the current series |
| `elapsed()` | How many ticks have fired since the series started |
| `paused()` | Whether the ticker is currently paused |

All repeat methods accept an optional `onComplete` callback.  
All interval arguments accept either **integers (ms)** or **floats (seconds)**.

---

## Installation

### Arduino Library Manager
1. Open Arduino IDE → **Sketch → Include Library → Manage Libraries**
2. Search for `ExtendedTicker`
3. Click **Install**

### Manual
1. Download the ZIP from [GitHub Releases](https://github.com/faruk/ExtendedTicker/releases)
2. Arduino IDE → **Sketch → Include Library → Add .ZIP Library**

---

## Requirements

- ESP8266 or ESP32 board package installed
- Built-in `Ticker` library (included with both board packages)

---

## API Reference

### `repeatN(interval, count, callback [, onComplete])`

Fire `callback` every `interval`, exactly `count` times, then stop.

```cpp
// Integers → interval in milliseconds
ticker.repeatN(500, 10, []() { Serial.println("tick"); });

// Floats → interval in seconds
ticker.repeatN(0.5f, 10, []() { Serial.println("tick"); });

// With onComplete
ticker.repeatN(500, 5,
  []() { Serial.println("tick"); },
  []() { Serial.println("done!"); }
);
```

---

### `repeatUntil(interval, condition, callback [, onComplete])`

Fire `callback` every `interval` until `condition()` returns `true`.  
The condition is checked **before** each tick — when `true`, the tick is skipped and `onComplete` is called.

```cpp
bool ready = false;

ticker.repeatUntil(500,
  []() -> bool { return ready; },
  []() { Serial.println("still waiting..."); },
  []() { Serial.println("ready!"); }
);
```

---

### `once_after(delayMs, intervalMs, callback)`

Wait `delayMs`, then repeat `callback` every `intervalMs` indefinitely.  
Call `detach()` to stop.

```cpp
// Wait 2 s for WiFi, then start polling every 1 s
ticker.once_after(2000, 1000, []() {
  Serial.println("polling...");
});

// Stop after 5 polls using elapsed()
ticker.once_after(2000, 1000, [&]() {
  Serial.println("polling...");
  if (ticker.elapsed() >= 5) ticker.detach();
});
```

---

### `repeatFor(duration, interval, callback [, onComplete])`

Repeat `callback` every `interval` for a total `duration`, then stop.

```cpp
// Integers → milliseconds
ticker.repeatFor(10000, 200, []() { Serial.println("sample"); });

// Floats → seconds
ticker.repeatFor(10.0f, 0.2f, []() { Serial.println("sample"); });

// With onComplete
ticker.repeatFor(5000, 500,
  []() { Serial.println("tick"); },
  []() { Serial.println("5 s window done"); }
);
```

---

### `pause()` / `resume()`

Temporarily halt and continue a running series. All state is fully preserved:
remaining count, elapsed count, interval, and callback.

```cpp
ticker.repeatN(500, 20, []() {
  Serial.printf("elapsed=%d remaining=%d\n",
                ticker.elapsed(), ticker.remaining());
});

// Later — in loop() or a button handler:
ticker.pause();   // stops firing, remembers where it was
ticker.resume();  // continues from where it paused
```

---

### `remaining()` / `elapsed()` / `paused()`

Query the state of the current series at any time.

```cpp
ticker.remaining();  // ticks left  (-1 = unlimited, 0 = not active)
ticker.elapsed();    // ticks fired so far
ticker.paused();     // true if currently paused
```

---

## Examples

All examples are included under `examples/` and appear in  
**File → Examples → ExtendedTicker** in the Arduino IDE.

| Example | Demonstrates |
|---|---|
| `01_repeatN` | Basic N-shot callback + onComplete |
| `02_repeatUntil` | Condition-based polling |
| `03_once_after` | Delayed start then continuous repeat |
| `04_repeatFor` | Fixed-duration sampling window |
| `05_pause_resume` | Pause/resume with status queries |
| `06_retry_pattern` | Real-world WiFi reconnect retry |

---

## Compatibility

| Board | Status |
|---|---|
| ESP8266 (all variants) | ✅ Tested |
| ESP32 | ✅ Compatible |
| AVR (Uno, Mega, etc.) | ❌ Not supported (requires `Ticker`) |

---

## License

MIT — see [LICENSE](LICENSE)
