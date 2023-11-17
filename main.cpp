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
const int BULLET_SPEED = 10;         // default player speed

// position converter
void windowToGameFrame(int *x, int *y) {
  *x -= WIDTH / 2;
  *y = HEIGHT - *y;
}

void GameFrameTowindow(int *x, int *y) {
  *x += WIDTH / 2;
  *y = HEIGHT - *y;
}


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

  void getPosition(int *x, int *y) {
    // get bullet position
    *x = positionX;
    *y = positionY;
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
  Player() {};
  Player(int positionX, int positionY, int health, int length, int width) {
    this->positionX = positionX;
    this->positionY = positionY;
    this->health = health;
    this->length = length;
    this->width = width;
    this->alive = true;
  }
  ~Player() {}

  void getPosition(int *x, int *y) {
    // get player position
    *x = positionX;
    *y = positionY;
  }

  void setPosition(int x, int y) {
    // set player position
    positionX = x;
    positionY = y;
  }

  bool isAlive() {
    // check if player is alive
    return alive;
  }

  int getHealth() {
    // get player health
    return health;
  }

  void setHealth(int health) {
    // set player health
    this->health = health;
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
    Bullet bullet(positionX, positionY, 1);
    return bullet;
  }

  void draw() {
    // draw player object
    glColor3ub(0, 0, 0);
    glBegin(GL_QUADS);
    glVertex2i(positionX - length / 2, positionY - width / 2);
    glVertex2i(positionX + length / 2, positionY - width / 2);
    glVertex2i(positionX + length / 2, positionY + width / 2);
    glVertex2i(positionX - length / 2, positionY + width / 2);
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

  bool isAlive() {
    // check if enemy is alive
    return alive;
  }

  bool checkHit(Bullet bullet) {
    // check if enemy is hit by player's bullet
    int bulletX, bulletY;
    bullet.getPosition(&bulletX, &bulletY);
    int damageFromPlayer = bullet.getDamage();
    if (bulletX >= positionX - length / 2 && bulletX <= positionX + length / 2 && bulletY >= positionY - width / 2 && bulletY <= positionY + width / 2) {
      health -= damageFromPlayer;
      if (health <= 0) {
        alive = false;
      }
      return true;
    }
    return false;
  }

  bool chechHitPlayer(Player player) {
    // check if enemy is hit by player
    int playerX, playerY;
    player.getPosition(&playerX, &playerY);
    if (playerX >= positionX - length / 2 && playerX <= positionX + length / 2 && playerY >= positionY - width / 2 && playerY <= positionY + width / 2) {
      alive = false;
      return true;
    }
    return false;
  }

  void draw() {
    // draw enemy object
    glColor3ub(0, 0, 0);
    glBegin(GL_QUADS);
    glVertex2i(positionX - length / 2, positionY - width / 2);
    glVertex2i(positionX + length / 2, positionY - width / 2);
    glVertex2i(positionX + length / 2, positionY + width / 2);
    glVertex2i(positionX - length / 2, positionY + width / 2);
    glEnd();
  }
};

class Game {
private:
  int score;
  int time;

  Player player;
  vector<Bullet> bullets;
  vector<Enemy> enemies;
  // You might want to use a data structure for storing enemies, e.g., std::vector<Enemy> enemies;
  // Consider adding other necessary attributes for your game

  void displaySplashScreen() {
      // Implementation for displaying the splash screen transition
  }

  void handleInput() {
      // Implementation for handling user input
  }


public:
  Game() {
    // Initialize game state
    score = 0;
    time = 0;
    player = Player(WIDTH / 2, HEIGHT - 50, 100, 20, 20);
    initEnemy();
    // Initialize other attributes for your game
  }
  ~Game() {}

  void initEnemy() {
    // Initialize enemies
    for (int i = 0; i < 10; i++) {
      Enemy enemy(rand() % WIDTH, rand() % HEIGHT, 5, 3, 20, 20);
      enemies.push_back(enemy);
    }
  }

  void update(int playerX, int playerY) {

    // Update player position
    player.setPosition(playerX, playerY);
    // Update bullet positions
    for (int i = 0; i < bullets.size(); i++) {
      bullets[i].move(-10);
    }
    // Update enemy positions
    // for (int i = 0; i < enemies.size(); i++) {
    //   enemies[i].move(5);
    // }
    // Check for collisions
    for (int i = 0; i < bullets.size(); i++) {
      for (int j = 0; j < enemies.size(); j++) {
        if (enemies[j].checkHit(bullets[i])) {
          bullets.erase(bullets.begin() + i);
        }
        if (!enemies[j].isAlive()) {
          enemies.erase(enemies.begin() + j);
          score += 10;
        }
      }
    }
    // Check for player being hit
    for (int i = 0; i < enemies.size(); i++) {
      if (enemies[i].chechHitPlayer(player)) {
        player.setHealth(player.getHealth() - 1);
      }
      if (!enemies[i].isAlive()) {
        enemies.erase(enemies.begin() + i);
        score += 10;
      }
    }

    if (enemies.size() == 0) {
      initEnemy();
    }

    // Update game state
    handleInput();  // Check for user input
    // Update score, time, enemies, and other game elements
  }

  void draw() {
    // Draw game elements
    // Draw score, time, enemies, and other game elements
    player.draw();
    for (int i = 0; i < bullets.size(); i++) {
      bullets[i].draw();
    }
    for (int i = 0; i < enemies.size(); i++) {
      enemies[i].draw();
    }
  }

  void shootNewBullet() {
    // Shoot a new bullet
    Bullet bullet = player.shoot();
    bullets.push_back(bullet);
  }

};

int main(void) {
  srand(time(0));
  FsOpenWindow(0, 0, WIDTH, HEIGHT, 1);
  Game app;
  int timer = 0;
  while (true) { // main app loop
    FsPollDevice();
    if (FSKEY_ESC == FsInkey()) {
      break;
    }
    int lb, mb, rb, mx, my;
		int evt = FsGetMouseEvent(lb, mb, rb, mx, my);
    int playerX = mx;
    int playerY = my;
    // windowToGameFrame(&playerX, &playerY);
   
    // create a timer to shoot bullet
    timer++;
    if (timer == 10) {
      app.shootNewBullet();
      timer = 0;
    }

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    app.update(playerX, playerY);
    app.draw();
    FsSwapBuffers();
    FsSleep(10);
  }
  return 0;
}
