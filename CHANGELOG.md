# Changelog

## Unreleased

### Added
- Centralized pattern dispatch for easier maintenance and extension.
- Safer request handling and JSON/text response helpers for the web API.
- Better startup diagnostics and WiFi connection handling for ESP8266 stability.

### Improved
- Replaced hard-coded pattern switches with a pattern registry.
- Reduced blocking behavior in frame rendering by using `FastLED.delay()` and `yield()`.
- Added clearer validation for color and palette endpoint inputs.

### Documentation
- Added a contributor guide for future open-source collaboration.
