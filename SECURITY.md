# Security Policy

## Supported Versions

This project is currently in active development as a thesis project (Examensarbete). Security updates will be provided for the following:

| Version | Supported          |
| ------- | ------------------ |
| Latest (main branch) | :white_check_mark: |
| Development branches | :x: |

---

## Reporting a Vulnerability

We take security seriously and appreciate your efforts to responsibly disclose any security vulnerabilities.

### How to Report

If you discover a security vulnerability, please **do NOT** open a public issue. Instead:

1. **Email the maintainers privately:**
   - Contact: [Create a security advisory on GitHub](https://github.com/davdd1/ExamensArbete/security/advisories/new)
   - Or email the project authors (check GitHub profiles for contact info)

2. **Include the following information:**
   - Description of the vulnerability
   - Steps to reproduce
   - Potential impact
   - Suggested fix (if you have one)
   - Your contact information (if you'd like credit for the disclosure)

3. **Expected Response:**
   - We will acknowledge your report within 48 hours
   - We will investigate and provide updates on the fix timeline
   - We will notify you when the vulnerability is resolved

---

## Security Considerations

### Current Limitations

This project is a **prototype/proof-of-concept** and has the following known security limitations:

⚠️ **NOT PRODUCTION-READY** - This system is intended for research, education, and prototyping only.

1. **No Encryption:**
   - UDP communication is **unencrypted**
   - WebSocket communication is **not secured with TLS/SSL**
   - All sensor data is transmitted in **plaintext**

2. **No Authentication:**
   - No user authentication or authorization
   - Any device on the network can connect to the server
   - Any client can receive sensor data

3. **No Input Validation:**
   - Limited validation of incoming sensor data
   - Potential for malformed packets to cause unexpected behavior

4. **Network Security:**
   - Designed for **trusted local networks only**
   - Not suitable for use over public/untrusted networks
   - No protection against man-in-the-middle attacks

5. **Physical Security:**
   - No device pairing or authentication
   - No protection against unauthorized hardware access

---

## Recommendations for Secure Usage

If you plan to use this project in a real-world scenario:

### 1. Network Isolation
- Use a **dedicated, isolated network** for IoT devices
- Implement **firewall rules** to restrict access
- Use **VPN** if remote access is needed

### 2. Encryption
- Implement **TLS/SSL** for WebSocket connections
- Consider **DTLS** (Datagram TLS) for UDP if confidentiality is required
- Encrypt sensitive configuration data stored on devices

### 3. Authentication & Authorization
- Add **API keys** or **token-based authentication**
- Implement **device whitelisting** based on MAC address
- Use **role-based access control** for different user types

### 4. Input Validation
- Validate all incoming data on the server side
- Implement **rate limiting** to prevent abuse
- Add **checksums or signatures** to verify packet integrity

### 5. Firmware Security
- Use **secure boot** on ESP32 to prevent unauthorized firmware
- Implement **OTA (Over-The-Air) updates** with signature verification
- Protect sensitive credentials in firmware (use ESP32 secure storage)

### 6. Code Auditing
- Regularly review code for security vulnerabilities
- Use static analysis tools (e.g., `go vet`, `golangci-lint`)
- Perform penetration testing before deployment

---

## Future Security Improvements

We plan to address these security concerns in future versions:

- [ ] TLS/SSL support for WebSocket
- [ ] Device authentication (MAC whitelisting, certificates)
- [ ] Encrypted UDP payloads (DTLS or custom encryption)
- [ ] Input validation and sanitization
- [ ] Rate limiting and DDoS protection
- [ ] Secure credential storage
- [ ] Secure boot and signed OTA updates

Contributions in these areas are especially welcome! See [CONTRIBUTING.md](CONTRIBUTING.md).

---

## Third-Party Dependencies

This project uses the following third-party libraries. Please monitor them for security updates:

### Go Dependencies
- `github.com/gorilla/websocket` - WebSocket implementation
- Check `server/go.mod` for full list

### ESP-IDF
- ESP-IDF framework - https://github.com/espressif/esp-idf
- Regularly update to the latest stable version

### Godot Engine
- Godot Engine - https://godotengine.org/
- Keep updated to receive security patches

### Advisories
- Monitor GitHub Security Advisories for dependencies
- Use `go mod tidy` and `go get -u` to update Go packages
- Subscribe to ESP-IDF release notes

---

## Responsible Disclosure

We appreciate security researchers who responsibly disclose vulnerabilities. If you report a valid security issue:

1. We will acknowledge your contribution in the fix commit/release notes (if you wish)
2. We will work with you to understand and address the issue
3. We will not take legal action against good-faith security research

---

## Contact

For security-related inquiries:
- GitHub Security Advisories: https://github.com/davdd1/ExamensArbete/security/advisories
- Project Maintainers: Jesper Morais & David Stenman

---

## Disclaimer

This software is provided "as is" without warranty of any kind. See [LICENSE](LICENSE) for full terms.

**Use at your own risk.** This project is not intended for critical systems or environments where security is paramount.

---

*Last updated: May 2025*
