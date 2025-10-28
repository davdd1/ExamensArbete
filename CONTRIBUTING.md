# Contributing

Thanks for checking this out! This is a student thesis project, but we're happy to accept contributions.

## Reporting Bugs

Found a bug? Open an issue with:
- What happened
- How to reproduce it
- What you expected
- Your setup (hardware/software versions)

## Suggesting Features

Got an idea? Open an issue and describe:
- What you want to add
- Why it would be useful
- How you think it could work

## Submitting Code

1. Fork the repo
2. Create a branch: `git checkout -b fix-something`
3. Make your changes
4. Test them
5. Commit: `git commit -m "Fix something"`
6. Push and open a PR

## Code Style

### C (ESP32)
- Use ESP-IDF conventions
- `snake_case` for functions/variables
- `UPPER_CASE` for constants
- Use ESP_LOG macros for logging

### Go (Server)
- Run `gofmt` on your code
- `camelCase` for private, `PascalCase` for exported
- Handle errors explicitly

### GDScript (Godot)
- Follow the [GDScript style guide](https://docs.godotengine.org/en/stable/tutorials/scripting/gdscript/gdscript_styleguide.html)
- `snake_case` for functions/variables
- `PascalCase` for classes
- Use type hints

## What We'd Love Help With

- PCB design (KiCad)
- 3D printable case
- Better sensor integration
- Deep sleep modes
- TLS/SSL support
- Authentication
- Game demos in Godot
- Documentation improvements

## Questions?

Open an issue or check existing discussions.

## License

By contributing, you agree your code will be under the MIT License.
