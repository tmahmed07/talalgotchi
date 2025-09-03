#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

// Display values and States
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define MAX_BAR_WIDTH 30
#define BTN_RIGHT 2
#define BTN_SELECT 3
#define BTN_LEFT 4
#define HOME 0
#define FEED_SELECT 1
#define FEED 2
#define HAPPINESS_SELECT 3
#define HAPPINESS 4
#define ENERGY_SELECT 5
#define ENERGY 6
#define AGE 7
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, -1);

// Pet bitmap (32x32)
const unsigned char petBitmap[] PROGMEM = {
  0x03,0x00,0x00,0xc0,0x07,0x80,0x01,0xe0,0x0f,0xff,0xff,0xf0,0x1f,0xff,0xff,0xf8,
  0x3f,0xff,0xff,0xfc,0x27,0xf8,0x1f,0xe4,0x23,0xf0,0x0f,0xc4,0x20,0x00,0x00,0x04,
  0x20,0x30,0x0c,0x04,0x20,0x00,0x00,0x04,0x20,0x70,0x0e,0x04,0x20,0xf8,0x1f,0x04,
  0x20,0xc8,0x13,0x04,0x20,0xc8,0x13,0x04,0x20,0x70,0x0e,0x04,0x26,0x01,0x80,0x64,
  0x20,0x05,0xa0,0x04,0x10,0x02,0x40,0x08,0x08,0x00,0x00,0x10,0x04,0x00,0x00,0x20,
  0x08,0x00,0x00,0x10,0x0a,0x00,0x00,0x50,0x0a,0x00,0x00,0x50,0x0e,0x00,0x00,0x70,
  0x04,0x00,0x00,0x20,0x04,0x00,0x00,0x20,0x04,0x00,0x00,0x20,0x04,0x00,0x00,0x20,
  0x04,0x00,0x00,0x20,0x05,0xff,0xff,0xa0,0x05,0x00,0x00,0xa0,0x07,0x00,0x00,0xe0
};

int state;
int feedSelect = 0;
int hunger = 30;
int happiness = 30;
int energy = 30;
int age_stat = 0;

void setup() {
  state = HOME;
  pinMode(BTN_RIGHT, INPUT_PULLUP);
  pinMode(BTN_SELECT, INPUT_PULLUP);
  pinMode(BTN_LEFT, INPUT_PULLUP);

  Serial.begin(9600);
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("SSD1306 allocation failed");
    for (;;);
  }

  display.clearDisplay();
  drawAllBars();
  display.drawBitmap(50, 28, petBitmap, 32, 32, SSD1306_WHITE);
  display.display();
}

// main loop
void loop() {
  switch(state) {
    case HOME:
      animatePet();
      readingButtonsMain();
      break;

    case FEED_SELECT:
      showFeedSelect();
      readingButtonsMain();
      break;

    case FEED:
      showFeed();
      break;

    case HAPPINESS_SELECT:
      showHappinessSelect();
      readingButtonsMain();
      break;

    case HAPPINESS:
      showHappiness();
      break;
    
    case ENERGY_SELECT:
      showEnergySelect();
      readingButtonsMain();
      break;
    
    case ENERGY:
      showEnergy();
      break;
    
    case AGE:
      showAge();
      readingButtonsMain();
      break;
  }
}

// Display Functions

void drawAllBars() {
  drawHungerBar(hunger);
  drawHappinessBar(happiness);
  drawEnergyBar(energy);
}

void drawHungerBar(int value) {
  int barWidth = map(value, 0, 30, 0, MAX_BAR_WIDTH);
  display.fillRect(10, 0, MAX_BAR_WIDTH, 7, BLACK);
  display.fillRect(10, 0, barWidth, 7, WHITE);
  display.setCursor(0, 0);
  display.setTextColor(WHITE);
  display.print("H");
}

void drawHappinessBar(int value) {
  int barWidth = map(value, 0, 30, 0, MAX_BAR_WIDTH);
  display.fillRect(50, 0, MAX_BAR_WIDTH, 7, BLACK);
  display.fillRect(50, 0, barWidth, 7, WHITE);
  display.setCursor(42, 0);
  display.setTextColor(WHITE);
  display.print("F");
}

void drawEnergyBar(int value) {
  int barWidth = map(value, 0, 30, 0, MAX_BAR_WIDTH);
  display.fillRect(95, 0, MAX_BAR_WIDTH, 7, BLACK);
  display.fillRect(95, 0, barWidth, 7, WHITE);
  display.setCursor(85, 0);
  display.setTextColor(WHITE);
  display.print("E");
}

void animatePet() {
  for (int i = 50; i > 10; i--) {
    if (state != HOME) break;
    display.clearDisplay();
    drawAllBars();
    display.drawBitmap(i, 28, petBitmap, 32, 32, SSD1306_WHITE);
    display.display();
    readingButtonsMain();
  }
  for (int i = 11; i < 98; i++) {
    if (state != HOME) break;
    display.clearDisplay();
    drawAllBars();
    display.drawBitmap(i, 28, petBitmap, 32, 32, SSD1306_WHITE);
    display.display();
    readingButtonsMain();
  }
  for (int i = 97; i > 49; i--) {
    if (state != HOME) break;
    display.clearDisplay();
    drawAllBars();
    display.drawBitmap(i, 28, petBitmap, 32, 32, SSD1306_WHITE);
    display.display();
    readingButtonsMain();
  }
}

void showFeedSelect() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("-> Feed your TalalGotchi!");
  display.setCursor(0, 55);
  display.print("Current Hunger: ");
  display.print((hunger * 100) / 30);
  display.print("%");
  display.display();
}

void showFeed() {
  display.clearDisplay();
  display.setCursor(0, 20);
  bool fed = false;
  if (feedSelect == 0) {
    display.println("-> Feed Snack");
    display.print("   Feed Food");
  } else {
    display.println("   Feed Snack");
    display.print("-> Feed Food");
  }

  if (digitalRead(BTN_SELECT) == LOW) {
    if (feedSelect == 0) hunger = min(hunger + 5, 30);
    else hunger = 30;
    fed = true;
    delay(200);
  }

  if (digitalRead(BTN_RIGHT) == LOW) {
    feedSelect = (feedSelect + 1) % 2;
    delay(200);
  }

  if (digitalRead(BTN_LEFT) == LOW) {
    state = FEED_SELECT;
    delay(200);
    return;
  }

  if (fed) {
    display.clearDisplay();
    display.setCursor(0, 20);
    display.println("Your TamaGotchi");
    display.println("was Fed!");
    display.display();
    delay(2000);
    state = HOME;
  }

  display.display();
}

void showHappinessSelect() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("-> Pet your TalalGotchi!");
  display.setCursor(0, 30);
  display.print("Current Fulfillment: ");
  display.print((happiness * 100) / 30);
  display.print("%");
  display.display();
}

void showHappiness() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.println(F("You pet your TalalGotchi!!"));
  display.display();
  happiness = min(happiness + 5, 30); // Increase happiness
  delay(1000);
  state = HOME;
}

void showEnergySelect() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print(F("-> Put your TalalGotchi to Sleep!"));
  display.setCursor(0, 30);
  display.print(F("Current Energy: "));
  display.print((energy * 10) / 3);
  display.print("%");
  display.display();
}

void showEnergy() {
  display.clearDisplay();
  display.setCursor(0, 0);
  display.print(F("Your TalalGotchi is now sleeping!"));
  display.display();
  delay(1000);
}

void showAge() {
  display.clearDisplay();
  display.setCursor(0, 16);
  display.print(F("Your TalalGotchi's current age is: "));
  display.print(age_stat);
  display.println(F(" years."));
  display.print(F("TalalGotchi's survive until 12 years old."));
  display.display();
}

// Button Input Functions

void readingButtonsMain() {
  switch(state) {
    case HOME:
      if (digitalRead(BTN_RIGHT) == LOW) { state = FEED_SELECT; delay(200); }
      break;
    case FEED_SELECT:
      if (digitalRead(BTN_LEFT) == LOW) { state = HOME; delay(200); }
      else if (digitalRead(BTN_SELECT) == LOW) { state = FEED; delay(200); }
      else if (digitalRead(BTN_RIGHT) == LOW) { state = HAPPINESS_SELECT; delay(200); }
      break;
    case HAPPINESS_SELECT:
      if (digitalRead(BTN_LEFT) == LOW) { state = FEED_SELECT; delay(200); }
      else if (digitalRead(BTN_SELECT) == LOW) { state = HAPPINESS; delay(200); }
      else if (digitalRead(BTN_RIGHT) == LOW) { state = ENERGY_SELECT; delay(200); }
      break;
    case ENERGY_SELECT:
      if (digitalRead(BTN_LEFT) == LOW) { state = HAPPINESS_SELECT; delay(200); }
      else if (digitalRead(BTN_SELECT) == LOW) { state = ENERGY; delay(200); }
      else if (digitalRead(BTN_RIGHT) == LOW) { state = AGE; delay(200); }
      break;
    case AGE:
      if (digitalRead(BTN_LEFT) == LOW) {
        state = ENERGY_SELECT;
        delay(200);
      } else if (digitalRead(BTN_RIGHT) == LOW) {
        state = HOME;
        delay(200);
      }
      break;
  }
}
