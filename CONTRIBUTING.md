# Contributing to IoT Wireless Controller

Thank you for your interest in contributing to this project! We welcome contributions from the community to help improve and expand this open-source IoT controller platform.

---

## How to Contribute

### Reporting Bugs

If you find a bug, please open an issue on GitHub with:

1. **Clear title**: Briefly describe the problem
2. **Description**: Detailed explanation of the issue
3. **Steps to reproduce**: How to recreate the bug
4. **Expected behavior**: What should happen
5. **Actual behavior**: What actually happens
6. **Environment**: Hardware/software versions, OS, etc.
7. **Logs/Screenshots**: Any relevant error messages or visuals

### Suggesting Features

We love new ideas! To suggest a feature:

1. **Check existing issues**: Ensure it hasn't been suggested already
2. **Open a new issue**: Use the "Feature Request" template
3. **Describe the feature**: What problem does it solve?
4. **Provide context**: Use cases, benefits, implementation ideas
5. **Be specific**: The more detail, the better

### Submitting Pull Requests

Follow these steps to contribute code:

1. **Fork the repository** on GitHub
2. **Clone your fork** locally:
   ```bash
   git clone https://github.com/YOUR_USERNAME/ExamensArbete.git
   cd ExamensArbete
   ```
3. **Create a new branch** for your feature/fix:
   ```bash
   git checkout -b feature/your-feature-name
   ```
4. **Make your changes** following our coding standards (see below)
5. **Test your changes** thoroughly
6. **Commit with clear messages**:
   ```bash
   git commit -m "Add feature: description of changes"
   ```
7. **Push to your fork**:
   ```bash
   git push origin feature/your-feature-name
   ```
8. **Open a Pull Request** on GitHub with:
   - Clear title and description
   - Reference to related issues
   - Summary of changes
   - Testing performed

---

## Coding Standards

### General Guidelines

- **Keep changes minimal**: Only modify what's necessary
- **Write clear code**: Use descriptive variable/function names
- **Comment when needed**: Explain complex logic, but don't over-comment
- **Follow existing style**: Match the coding style of the file you're editing
- **Test before submitting**: Ensure your changes don't break existing functionality

### ESP32 (C/ESP-IDF)

- Use ESP-IDF coding conventions
- Follow C99 standard
- Use `snake_case` for functions and variables
- Use `UPPER_CASE` for constants and macros
- Include header guards in all `.h` files
- Free allocated memory and close resources
- Use ESP_LOG macros for logging

Example:
```c
#include "esp_log.h"

static const char *TAG = "MODULE_NAME";

void initialize_module(void) {
    ESP_LOGI(TAG, "Initializing module...");
    // Implementation
}
```

### Go (Server)

- Follow standard Go conventions (use `gofmt`)
- Use `camelCase` for private functions/variables
- Use `PascalCase` for exported functions/variables
- Handle errors explicitly
- Use goroutines and channels appropriately
- Comment exported functions/types

Example:
```go
package main

import "log"

// ProcessSensorData handles incoming sensor data from UDP
func ProcessSensorData(data []byte) error {
    if len(data) == 0 {
        return fmt.Errorf("empty data received")
    }
    // Implementation
    return nil
}
```

### Godot (GDScript)

- Follow GDScript style guide: https://docs.godotengine.org/en/stable/tutorials/scripting/gdscript/gdscript_styleguide.html
- Use `snake_case` for functions and variables
- Use `PascalCase` for class names
- Use `UPPER_CASE` for constants
- Type hints are recommended
- Keep scripts modular and reusable

Example:
```gdscript
extends Node

const MAX_SPEED: float = 100.0

var current_speed: float = 0.0

func _ready() -> void:
    print("Node ready")

func update_speed(delta: float) -> void:
    current_speed += delta
```

### Documentation

- Update README files if you change functionality
- Add inline comments for complex logic
- Update references if you add new dependencies
- Keep documentation concise and accurate

---

## Development Workflow

### Setting Up Your Environment

**ESP32 Development:**
```bash
cd embedded
# Install ESP-IDF v5.x
# Configure and build
idf.py build
```

**Go Server:**
```bash
cd server
go mod download
go build
# Or use Docker
docker-compose up --build
```

**Godot Client:**
- Install Godot Engine 4.x
- Open project in Godot
- Run and test

### Testing

- **ESP32**: Test on actual hardware, monitor serial output
- **Go Server**: Test UDP reception and WebSocket broadcasting
- **Godot Client**: Test WebSocket connection and data visualization
- **End-to-End**: Test full pipeline from controller to visualization

### Code Review Process

1. Maintainers will review your PR
2. Address any feedback or requested changes
3. Once approved, your PR will be merged
4. Thank you for your contribution! 🎉

---

## Areas for Contribution

We especially welcome contributions in these areas:

### Hardware
- Custom PCB design (KiCad schematics)
- 3D-printable enclosure designs
- Alternative sensor integrations (IMU upgrades, buttons, triggers)
- Battery optimization and power management

### Software
- **ESP32**: Deep sleep modes, OTA updates, improved sensor fusion
- **Go Server**: TLS/SSL support, authentication, load balancing
- **Godot Client**: New game demos, UI improvements, mobile support
- **Cross-Platform**: Web client, mobile apps, other platforms

### Documentation
- Wiring diagrams and assembly guides
- Video tutorials
- Localization (translations)
- API documentation

### Testing
- Automated testing frameworks
- Performance benchmarking tools
- CI/CD pipeline improvements

---

## Community Guidelines

- **Be respectful**: Treat everyone with kindness and professionalism
- **Be constructive**: Provide helpful feedback
- **Be patient**: Maintainers and contributors are often volunteers
- **Be open**: Welcome new ideas and diverse perspectives
- **Follow the Code of Conduct**: (If applicable, link to CODE_OF_CONDUCT.md)

---

## Questions?

If you have questions about contributing:

1. Check existing issues and discussions
2. Open a new issue with the "Question" label
3. Reach out to maintainers: Jesper Morais & David Stenman

---

## License

By contributing to this project, you agree that your contributions will be licensed under the MIT License (see `LICENSE` file).

---

Thank you for making this project better! Your contributions help create a more accessible, open-source IoT ecosystem. 🚀
