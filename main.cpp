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

class Bullet {
private:
  int positionX;
  int positionY;
  int damage;

public:
  Bullet(int positionX, int positionY, int damage) {
    this->positionX = positionX;
    this->positionY = positionY;
    this->damage = damage;
  }
  ~Bullet() {}

  void getPosition(int &x, int &y) {
    // get bullet position
    x = positionX;
    y = positionY;
  }

  void setPosition(int x, int y) {
    // set bullet position
    positionX = x;
    positionY = y;
  }

  int getDamage() {
    // get bullet damage
    return damage;
  }

  void move(int speed) {
    // move bullet
    positionY += speed;
  }

  void draw() {
    // draw bullet object
    glColor3ub(0, 0, 0);
    glBegin(GL_QUADS);
    glVertex2i(positionX, positionY);
    glVertex2i(positionX + 2, positionY);
    glVertex2i(positionX + 2, positionY + 5);
    glVertex2i(positionX, positionY + 5);
    glEnd();
  }
};

class Player {
private:
  int positionX;
  int positionY;
  int health;

  int length;
  int width;
  bool alive;

public:
  Player(int positionX, int positionY, int health, int length, int width) {
    this->positionX = positionX;
    this->positionY = positionY;
    this->health = health;
    this->length = length;
    this->width = width;
    this->alive = true;
  }
  ~Player() {}

  void getPosition(int &x, int &y) {
    // get player position
    x = positionX;
    y = positionY;
  }

  void setPosition(int x, int y) {
    // set player position
    positionX = x;
    positionY = y;
  }

  bool checkHit(int x, int y, int damageFromPlayer) {
    // check if player is hit by player's bullet
    if (x >= positionX && x <= positionX + length && y >= positionY && y <= positionY + width) {
      health -= damageFromPlayer;
      if (health <= 0) {
        alive = false;
      }
      return true;
    }
    return false;
  }

  // shoot a bullet
  Bullet shoot() {
    Bullet bullet(positionX + length / 2, positionY, 1);
    return bullet;
  }

  void draw() {
    // draw player object
    glColor3ub(0, 0, 0);
    glBegin(GL_QUADS);
    glVertex2i(positionX, positionY);
    glVertex2i(positionX + length, positionY);
    glVertex2i(positionX + length, positionY + width);
    glVertex2i(positionX, positionY + width);
    glEnd();
  }
};

class Enemy {
private:
  int positionX;
  int positionY;
  int speed;
  int health;

  // self-defined attributes
  int length;
  int width;
  bool alive;

public:
  Enemy(int positionX, int positionY, int speed, int health, int length, int width) {
    this->positionX = positionX;
    this->positionY = positionY;
    this->speed = speed;
    this->health = health;
    this->length = length;
    this->width = width;
    this->alive = true;
  }
  ~Enemy() {}

  void getPosition(int &x, int &y) {
    // get enemy position
    x = positionX;
    y = positionY;
  }

  void setPosition(int x, int y) {
    // set enemy position
    positionX = x;
    positionY = y;
  }

  bool checkHit(int x, int y, int damageFromPlayer) {
    // check if enemy is hit by player's bullet
    if (x >= positionX && x <= positionX + length && y >= positionY && y <= positionY + width) {
      health -= damageFromPlayer;
      if (health <= 0) {
        alive = false;
      }
      return true;
    }
    return false;
  }

  void draw() {
    // draw enemy object
    glColor3ub(0, 0, 0);
    glBegin(GL_QUADS);
    glVertex2i(positionX, positionY);
    glVertex2i(positionX + length, positionY);
    glVertex2i(positionX + length, positionY + width);
    glVertex2i(positionX, positionY + width);
    glEnd();
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
