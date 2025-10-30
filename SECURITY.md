# Security

## Important: This is a Prototype

⚠️ **This is a student thesis project** - not production-ready. Use on local networks only for testing/learning.

## What's Missing

- No encryption (UDP and WebSocket are plaintext)
- No authentication (anyone on the network can connect)
- No input validation
- Not tested against attacks

## Found a Security Issue?

If you find a vulnerability:
- Don't open a public issue
- Create a [security advisory](https://github.com/davdd1/ExamensArbete/security/advisories/new)
- Or contact the authors directly

## If You Want to Use This Seriously

**Network:**
- Use a separate network for IoT devices
- Set up firewall rules
- Use a VPN for remote access

**Encryption:**
- Add TLS/SSL to WebSocket
- Consider DTLS for UDP if needed

**Authentication:**
- Add API keys or tokens
- Whitelist device MAC addresses

**Code:**
- Validate all incoming data
- Add rate limiting
- Keep dependencies updated

## Dependencies to Monitor

- `github.com/gorilla/websocket` (Go)
- ESP-IDF framework
- Godot Engine

Check for updates regularly.

## Future Plans

Maybe we'll add:
- TLS/SSL support
- Device authentication
- Encrypted payloads
- Better input validation

Contributions welcome! See [CONTRIBUTING.md](CONTRIBUTING.md).
