

#include "libraries/fssimplewindow.h"
#include "libraries/yspng.h"
#include "libraries/yssimplesound.h"
#include <cstdlib>
#include <iostream>
#include <math.h>
#include <queue>
#include <stdio.h>
#include <time.h>
#include <vector>
using namespace std;

// other constants
const int WIDTH = 600;               // default window width
const int HEIGHT = 900;               // default window height

class Player {
private:
  int health;

public:
  Player() {}
  ~Player() {}

  void update() {
    // update player movement

    // update player status (health)
  }

  void draw() {
    // draw player object
  }
};

class Enemy {
private:
  int positionX;
  int positionY;
  int speed;
  int health;
  int damage;

public:
  Enemy() {}
  ~Enemy() {}

  void update() {   
    // update enemy movement

    // update enemy state (health, damage)
  }

  void draw() {
    // draw enemy object
  }
};

class Game {
private:
  int score;
  int time;
  // You might want to use a data structure for storing enemies, e.g., std::vector<Enemy> enemies;
  // Consider adding other necessary attributes for your game

  void displaySplashScreen() {
      // Implementation for displaying the splash screen transition
  }

  void handleInput() {
      // Implementation for handling user input
  }


public:
  Game() {}
  ~Game() {}

  void update() {
    // Update game state
    handleInput();  // Check for user input
    // Update score, time, enemies, and other game elements
  }

  void draw() {
    // Draw game elements
    // Draw score, time, enemies, and other game elements
  }
};

int main(void) {
  srand(time(0));
  FsOpenWindow(0, 0, WIDTH, HEIGHT, 1);
  Game app;
  while (true) { // main app loop
    FsPollDevice();
    if (FSKEY_ESC == FsInkey()) {
      break;
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    app.update();
    app.draw();
    FsSwapBuffers();
    FsSleep(10);
  }
  return 0;
}
