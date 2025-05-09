# Stage 1: Build
FROM golang:1.24.1-alpine AS builder
WORKDIR /app

# Ignore files via .dockerignore (see below)

# 1) Dependency download
COPY go.mod go.sum ./
RUN go mod download

# 2) Copy full source and compile
COPY . .
RUN CGO_ENABLED=0 \
    GOOS=linux GOARCH=amd64 \
    go build -ldflags="-s -w" -o server .

# Stage 2: Runtime
FROM alpine:3.18
# 1) Install only what we need in one layer
RUN apk add --no-cache ca-certificates && \
    addgroup -S appgroup && adduser -S appuser -G appgroup

USER appuser
WORKDIR /home/appuser

# 2) Copy binary
COPY --from=builder /app/server .

# 3) Expose ports
EXPOSE 1234/udp
EXPOSE 8080

# 4) Healthcheck (WebSocket readiness)
HEALTHCHECK --interval=30s --timeout=5s \
  CMD wget --spider --quiet http://localhost:8080/health || exit 1

ENTRYPOINT ["./server"]
