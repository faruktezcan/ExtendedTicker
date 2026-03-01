#pragma once
#include <Arduino.h>
#include <Ticker.h>
#include <functional>
#include <memory>

class ExtendedTicker : public Ticker {
public:
  ExtendedTicker() = default;

  using Ticker::attach;
  using Ticker::attach_ms;
  using Ticker::attach_ms_scheduled;
  using Ticker::attach_ms_scheduled_accurate;
  using Ticker::attach_scheduled;
  using Ticker::once;
  using Ticker::once_ms;
  using Ticker::once_ms_scheduled;
  using Ticker::once_scheduled;
  using Ticker::detach;
  using Ticker::active;

  // ═══════════════════════════════════════════════════════════════════════════
  // repeatN  –  fire exactly N times, then stop
  //   optional 4th arg: onComplete callback fired after the last tick
  // ═══════════════════════════════════════════════════════════════════════════

  template<typename T>
  typename std::enable_if<std::is_integral<T>::value>::type
  repeatN(T ms, int n, std::function<void()> func,
          std::function<void()> onComplete = nullptr) {
    _repeatN_impl(static_cast<uint32_t>(ms), n,
                  std::move(func), std::move(onComplete));
  }

  template<typename T>
  typename std::enable_if<std::is_floating_point<T>::value>::type
  repeatN(T seconds, int n, std::function<void()> func,
          std::function<void()> onComplete = nullptr) {
    _repeatN_impl(static_cast<uint32_t>(seconds * T(1000)), n,
                  std::move(func), std::move(onComplete));
  }

  // ═══════════════════════════════════════════════════════════════════════════
  // repeatUntil  –  fire every interval until condition() returns true
  //   condition is checked BEFORE each tick; when true the tick is skipped
  //   and onComplete (if provided) is called instead
  // ═══════════════════════════════════════════════════════════════════════════

  template<typename T>
  typename std::enable_if<std::is_integral<T>::value>::type
  repeatUntil(T ms, std::function<bool()> condition,
              std::function<void()> func,
              std::function<void()> onComplete = nullptr) {
    _repeatUntil_impl(static_cast<uint32_t>(ms),
                      std::move(condition), std::move(func), std::move(onComplete));
  }

  template<typename T>
  typename std::enable_if<std::is_floating_point<T>::value>::type
  repeatUntil(T seconds, std::function<bool()> condition,
              std::function<void()> func,
              std::function<void()> onComplete = nullptr) {
    _repeatUntil_impl(static_cast<uint32_t>(seconds * T(1000)),
                      std::move(condition), std::move(func), std::move(onComplete));
  }

  // ═══════════════════════════════════════════════════════════════════════════
  // once_after  –  wait delayMs, then repeat every intervalMs indefinitely
  //   call detach() to stop the repeating phase
  // ═══════════════════════════════════════════════════════════════════════════

  void once_after(uint32_t delayMs, uint32_t intervalMs,
                  std::function<void()> func) {
    this->detach();
    _reset();
    _func        = func;
    _interval_ms = intervalMs;
    auto alive   = _alive;

    this->once_ms(delayMs, [this, intervalMs, func, alive]() {
      if (!(*alive)) return;
      _startContinuous(intervalMs, func);
    });
  }

  // ═══════════════════════════════════════════════════════════════════════════
  // repeatFor  –  repeat every intervalMs for a total wall-clock duration
  // ═══════════════════════════════════════════════════════════════════════════

  template<typename T>
  typename std::enable_if<std::is_integral<T>::value>::type
  repeatFor(T durationMs, T intervalMs, std::function<void()> func,
            std::function<void()> onComplete = nullptr) {
    _repeatFor_impl(static_cast<uint32_t>(durationMs),
                    static_cast<uint32_t>(intervalMs),
                    std::move(func), std::move(onComplete));
  }

  template<typename T>
  typename std::enable_if<std::is_floating_point<T>::value>::type
  repeatFor(T durationSec, T intervalSec, std::function<void()> func,
            std::function<void()> onComplete = nullptr) {
    _repeatFor_impl(static_cast<uint32_t>(durationSec  * T(1000)),
                    static_cast<uint32_t>(intervalSec   * T(1000)),
                    std::move(func), std::move(onComplete));
  }

  // ═══════════════════════════════════════════════════════════════════════════
  // pause / resume  –  temporarily halt and continue the current series
  //   state (remaining, elapsed, interval, callback) is fully preserved
  // ═══════════════════════════════════════════════════════════════════════════

  void pause() {
    if (!active() || _paused) return;
    this->detach();
    _paused = true;
  }

  void resume() {
    if (!_paused || !_func) return;
    _paused = false;

    auto alive      = _alive;
    auto remaining  = _remaining;
    auto elapsed    = _elapsed;
    auto func       = _func;
    auto onComplete = _onComplete;
    uint32_t ms     = _interval_ms;
    bool unlimited  = (*remaining == -1);

    this->attach_ms(ms, [this, remaining, elapsed, func, onComplete, alive, unlimited]() {
      if (!(*alive)) return;
      func();
      ++(*elapsed);
      if (!unlimited && --(*remaining) == 0) {
        this->detach();
        _func = nullptr;
        if (onComplete) onComplete();
      }
    });
  }

  // ═══════════════════════════════════════════════════════════════════════════
  // status queries
  // ═══════════════════════════════════════════════════════════════════════════

  /**
   * Ticks remaining in the current repeatN / repeatFor series.
   * Returns -1 for repeatUntil / once_after (unlimited), 0 if not active.
   */
  int remaining() const {
    if (!_remaining) return 0;
    return *_remaining;
  }

  /**
   * Ticks fired since the last series started.
   */
  int elapsed() const {
    if (!_elapsed) return 0;
    return *_elapsed;
  }

  bool paused() const { return _paused; }

  // ═══════════════════════════════════════════════════════════════════════════
  // destructor
  // ═══════════════════════════════════════════════════════════════════════════

  ~ExtendedTicker() {
    this->detach();
    *_alive = false;
  }

private:
  // ── internal state ──────────────────────────────────────────────────────────
  std::shared_ptr<bool>  _alive    { std::make_shared<bool>(true) };
  std::shared_ptr<int>   _remaining{ std::make_shared<int>(0)    };
  std::shared_ptr<int>   _elapsed  { std::make_shared<int>(0)    };
  std::function<void()>  _func;
  std::function<void()>  _onComplete;
  uint32_t               _interval_ms{ 0 };
  bool                   _paused     { false };

  // ── helpers ─────────────────────────────────────────────────────────────────

  void _reset() {
    _paused      = false;
    _interval_ms = 0;
    _func        = nullptr;
    _onComplete  = nullptr;
    _remaining   = std::make_shared<int>(0);
    _elapsed     = std::make_shared<int>(0);
  }

  // ── _repeatN_impl ────────────────────────────────────────────────────────────

  void _repeatN_impl(uint32_t ms, int n,
                     std::function<void()> func,
                     std::function<void()> onComplete) {
    if (n <= 0) return;

    this->detach();
    _paused      = false;
    _interval_ms = ms;
    _func        = func;
    _onComplete  = onComplete;
    _remaining   = std::make_shared<int>(n);
    _elapsed     = std::make_shared<int>(0);

    auto alive     = _alive;
    auto remaining = _remaining;
    auto elapsed   = _elapsed;

    this->attach_ms(ms, [this, remaining, elapsed, func, onComplete, alive]() {
      if (!(*alive)) return;
      func();
      ++(*elapsed);
      if (--(*remaining) == 0) {
        this->detach();
        _func = nullptr;
        if (onComplete) onComplete();
      }
    });
  }

  // ── _repeatUntil_impl ────────────────────────────────────────────────────────

  void _repeatUntil_impl(uint32_t ms,
                         std::function<bool()> condition,
                         std::function<void()> func,
                         std::function<void()> onComplete) {
    this->detach();
    _paused      = false;
    _interval_ms = ms;
    _func        = func;
    _onComplete  = onComplete;
    _remaining   = std::make_shared<int>(-1);   // -1 = unlimited
    _elapsed     = std::make_shared<int>(0);

    auto alive   = _alive;
    auto elapsed = _elapsed;

    this->attach_ms(ms, [this, elapsed, func, condition, onComplete, alive]() {
      if (!(*alive)) return;
      if (condition()) {
        this->detach();
        _func = nullptr;
        if (onComplete) onComplete();
        return;
      }
      func();
      ++(*elapsed);
    });
  }

  // ── _repeatFor_impl ──────────────────────────────────────────────────────────

  void _repeatFor_impl(uint32_t durationMs, uint32_t intervalMs,
                       std::function<void()> func,
                       std::function<void()> onComplete) {
    if (intervalMs == 0) return;
    int n = static_cast<int>(durationMs / intervalMs);
    _repeatN_impl(intervalMs, n, std::move(func), std::move(onComplete));
  }

  // ── _startContinuous ─────────────────────────────────────────────────────────

  void _startContinuous(uint32_t ms, std::function<void()> func) {
    _interval_ms = ms;
    _remaining   = std::make_shared<int>(-1);   // -1 = unlimited
    _elapsed     = std::make_shared<int>(0);

    auto alive   = _alive;
    auto elapsed = _elapsed;

    this->attach_ms(ms, [this, func, elapsed, alive]() {
      if (!(*alive)) return;
      func();
      ++(*elapsed);
    });
  }
};
