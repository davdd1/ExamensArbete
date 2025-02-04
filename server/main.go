package main

import (
	"encoding/binary"
	"image/color"
	"log"
	"math"
	"net"
	"sync"
	"time"

	"github.com/hajimehoshi/ebiten/v2"
)

// --- Globala variabler & konstanter ---

// Fönsterstorlek
const (
	windowWidth  = 800
	windowHeight = 600
)

// Förväntad storlek på inkommande UDP-paket (20 byte: 1+3 (padding) +4+4+4)
const expectedSize = 20

// Variabler för bollens position (bör starta någonstans mitt i fönstret)
var (
	ballX float64 = windowWidth / 2
	ballY float64 = windowHeight / 2
	// Mutex för säker samtidighetsåtkomst till positionen
	ballMu sync.Mutex
	// Bollbilden skapas i main()
	ballImg *ebiten.Image
)

// --- Hjälpfunktion: Skapa en bollbild ---
//
// Skapar en cirkel (fylld med en given färg) med given radie.
func createBallImage(radius int, clr color.Color) *ebiten.Image {
	diameter := radius * 2
	img := ebiten.NewImage(diameter, diameter)
	// Gå igenom varje pixel och om den ligger inom cirkeln, sätt färgen.
	for y := 0; y < diameter; y++ {
		for x := 0; x < diameter; x++ {
			dx := float64(x - radius)
			dy := float64(y - radius)
			if dx*dx+dy*dy <= float64(radius*radius) {
				img.Set(x, y, clr)
			} else {
				// Gör pixeln transparent
				img.Set(x, y, color.RGBA{0, 0, 0, 0})
			}
		}
	}
	return img
}

// --- UDP-mottagare ---
//
// Lyssnar på UDP-port 1234, tar emot paket, avkodar gyrovärdena och uppdaterar bollens position.
func udpReceiver() {
	udpAddr, err := net.ResolveUDPAddr("udp", ":1234")
	if err != nil {
		log.Fatalf("Kunde inte lösa UDP-adress: %v", err)
	}
	conn, err := net.ListenUDP("udp", udpAddr)
	if err != nil {
		log.Fatalf("Kunde inte starta UDP-lyssnare: %v", err)
	}
	defer conn.Close()
	log.Println("UDP-mottagare körs på :1234")

	// Buffer för inkommande data
	buf := make([]byte, 1024)
	for {
		n, _, err := conn.ReadFromUDP(buf)
		if err != nil {
			log.Println("UDP läsfel:", err)
			continue
		}
		if n < expectedSize {
			log.Printf("För kort paket: %d byte (förväntat %d)", n, expectedSize)
			continue
		}

		// Avkoda den binära strukturen:
		// Byte 0: type (vi ignorerar)
		// Byte 1-3: padding (ignoreras)
		// Byte 4-7: player_id (ignoreras här)
		// Byte 8-11: gyro_x (float32)
		// Byte 12-15: gyro_y (float32)
		// Byte 16-19: gyro_z (float32, ignoreras)
		gyroX := math.Float32frombits(binary.LittleEndian.Uint32(buf[8:12]))
		gyroY := math.Float32frombits(binary.LittleEndian.Uint32(buf[12:16]))

		// Justera rörelsen med en skalfaktor och implementera en dead zone
		const factor = 0.5
		deltaX := float64(gyroX) * factor
		deltaY := float64(gyroY) * factor
		if math.Abs(deltaX) < 0.05 {
			deltaX = 0
		}
		if math.Abs(deltaY) < 0.05 {
			deltaY = 0
		}

		// Uppdatera bollens position på ett trådsäkert sätt
		ballMu.Lock()
		ballX += deltaX
		ballY += deltaY

		// Håll positionen inom fönstrets gränser (räknat med bollens diameter)
		if ballX < 0 {
			ballX = 0
		}
		if ballX > windowWidth-20 { // 20 = diameter (om radie=10)
			ballX = windowWidth - 20
		}
		if ballY < 0 {
			ballY = 0
		}
		if ballY > windowHeight-20 {
			ballY = windowHeight - 20
		}
		ballMu.Unlock()

		// Valfritt: logga rörelsen
		// log.Printf("gyroX=%.2f, gyroY=%.2f, ny position: (%.2f, %.2f)", gyroX, gyroY, ballX, ballY)
		// Liten paus (valfritt) för att undvika hög CPU-belastning
		time.Sleep(10 * time.Millisecond)
	}
}

// --- Ebiten Game ---
//
// Vi implementerar ett enkelt spel där vi ritar bakgrund och vår boll i fönstret.
type Game struct{}

func (g *Game) Update() error {
	// All uppdatering av bollpositionen sker via UDP-mottagaren
	return nil
}

func (g *Game) Draw(screen *ebiten.Image) {
	// Fyll bakgrunden med en mörk färg
	screen.Fill(color.RGBA{R: 32, G: 32, B: 32, A: 255})

	// Hämta bollens position på ett trådsäkert sätt
	ballMu.Lock()
	x, y := ballX, ballY
	ballMu.Unlock()

	// Rita bollbilden
	op := &ebiten.DrawImageOptions{}
	op.GeoM.Translate(x, y)
	screen.DrawImage(ballImg, op)
}

func (g *Game) Layout(outsideWidth, outsideHeight int) (int, int) {
	return windowWidth, windowHeight
}

// --- main ---
//
// Startar UDP-mottagaren (i en gorutin), skapar bollbilden och kör Ebiten-spelet.
func main() {
	// Skapa en bollbild med radie 10, färg röd
	ballImg = createBallImage(10, color.RGBA{R: 255, G: 0, B: 0, A: 255})

	// Starta UDP-mottagaren i bakgrunden
	go udpReceiver()

	// Konfigurera fönstret
	ebiten.SetWindowTitle("UDP Ball Movement Demo")
	ebiten.SetWindowSize(windowWidth, windowHeight)

	// Kör spelet
	if err := ebiten.RunGame(&Game{}); err != nil {
		log.Fatal(err)
	}
}
