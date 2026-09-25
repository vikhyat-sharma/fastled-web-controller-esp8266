# Contributing

Thanks for helping improve this project.

## Development workflow

1. Keep changes focused and well documented.
2. Prefer small, tested refactors over large rewrites.
3. Avoid introducing new dependencies unless they are necessary for ESP8266 compatibility.
4. Preserve backward-compatible behavior for existing API routes and UI flows.

## Style notes

- Keep code readable and conservative for embedded targets.
- Prefer `const`, `static`, and bounded loops where practical.
- Avoid heap-heavy `String` usage in hot paths.
- Use `yield()` in long-running loops and delay logic.

## Pull requests

- Describe the problem and the solution clearly.
- Note any hardware or configuration changes.
- Include screenshots or a short note when UI behavior changes.
