

#include "libraries/fssimplewindow.h"
#include "libraries/yspng.h"
#include "libraries/yssimplesound.h"
#include <cstdlib>
#include <iostream>
#include <math.h>
#include <queue>
#include <stdio.h>
#include <time.h>
#include <unordered_map>
#include <vector>
using namespace std;

// constants
const int WIDTH = 600;  // window width
const int HEIGHT = 900; // window height

//----------------------------------------------------------------
//                        HELPERS
//----------------------------------------------------------------

//----------------------------------------------------------------
//                        CLASSES
//----------------------------------------------------------------

class Bullet {
public:
  int x, y;   // position
  int vx, vy; // velocity
  int damage;

  Bullet(int x, int y, int vx, int vy) : x(x), y(y), vx(vx), vy(vy){};

  // update particle position
  void update() {
    x += vx;
    y += vy;
  }

  // draw the particle
  void draw() {
    glColor3f(0.0, 1.0, 0.0);
    glPointSize(6);
    glBegin(GL_POINTS);
    glVertex2i(x, y);
    glEnd();
  }
};
class Player {
private:
  int health;
  const int speed = 5;
  YsRawPngDecoder spaceshipDesign;

public:
  int x, y;
  vector<Bullet> bullets;

  Player() {
    // initialize other attributes
    x = WIDTH / 2;
    y = HEIGHT - 100;
  };

  void update() {
    for (auto &bullet : bullets) {
      bullet.update();
    }
    // update position, velocity, acceleration
  }

  void keyPressed(const int key) {
    switch (key) {
    case FSKEY_UP:
      y -= speed;
      break;
    case FSKEY_DOWN:
      y += speed;
      break;
    case FSKEY_LEFT:
      x -= speed;
      break;
    case FSKEY_RIGHT:
      x += speed;
      break;
    case FSKEY_SPACE:
      shoot();
      break;
    }
  }

  void shoot() { bullets.push_back(Bullet(x, y, 0, -15)); }

  void draw() {
    // draw the player image
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_TRIANGLES);
    glVertex2i(x - 10, y);
    glVertex2i(x, y - 20);
    glVertex2i(x + 10, y);
    glEnd();
    for (Bullet bullet : bullets) {
      bullet.draw();
    }
  }
};

class Enemy {
private:
  int health;

  int moveDirection = 1;
  int moveSpeed = 2;
  int moveRange = 50;
  vector<Bullet> bullets; // bullets fired by the enemy
  int moveCounter = 0;
  int shootTimer = 0;
  int shootInterval = 60;           // frames between shots
  YsRawPngDecoder *spaceshipDesign; // pointer to the image

public:
  int x, y; // position
  Enemy(const string &enemyType, YsRawPngDecoder *img, int x, int y)
      : x(x), y(y) {
    // initialize other attributes
    spaceshipDesign = img;
  }

  void update() {
    // update position, velocity, acceleration
    if (shootTimer++ >= shootInterval) {
      shootTimer = 0;
      Bullet bullet(x, y, 0, 5); //
      bullets.push_back(bullet);
    }
    x += moveDirection * moveSpeed;
    if (++moveCounter >= moveRange) {
      moveDirection *= -1;
      moveCounter = 0;
    }
  }
  void draw() {
    // draw the enemy
    glColor3f(1.0, 0.0, 0.0); // Red color
    glBegin(GL_QUADS);
    glVertex2i(x - 10, y - 10);
    glVertex2i(x + 10, y - 10);
    glVertex2i(x + 10, y + 10);
    glVertex2i(x - 10, y + 10);
    glEnd();
  }
};

class EnemyController {
public:
  vector<Enemy> all;
  unordered_map<string, YsRawPngDecoder> enemyImages; // store enemy images
  EnemyController() {
    loadEnemyImages();
    for (int i = 0; i < 10; i++) {
      int x = 50 + i * (WIDTH - 100) / 9;
      int y = 100;
      Enemy enemy("enemy_1", getEnemyImage("enemy_1"), x, y);
      all.push_back(enemy);
    }
  };

  void loadEnemyImages() {} // load images

  YsRawPngDecoder *getEnemyImage(const string &enemyType) {
    auto it = enemyImages.find(enemyType);
    if (it != enemyImages.end()) {
      return &it->second;
    }
    return nullptr;
  }

  // update all enemies
  void update() {
    for (Enemy enemy : all) {
      enemy.update();
    }
  }
};

class Button {
private:
  int x, y, width, height;
  string text;
  int health = 100;
  // function<void()> onClick;

public:
  Button(int x, int y, int width, int height, string text)
      : x(x), y(y), width(width), height(height), text(text) {}

  void draw() const {
    glColor3f(0.7, 0.7, 0.7);
    glBegin(GL_QUADS);
    glVertex2i(x, y);
    glVertex2i(x + width, y);
    glVertex2i(x + width, y + height);
    glVertex2i(x, y + height);
    glEnd();

    // draw text for button
  }

  bool checkClick(int mouseX, int mouseY) const {
    if (mouseX >= x && mouseX <= x + width && mouseY >= y &&
        mouseY <= y + height) {
      return true;
    }
    return false;
  }

  bool isAlive() const { return health > 0; }

  void handleClick(int mouseX, int mouseY) {
    if (checkClick(mouseX, mouseY)) {
      // onClick();
    }
  }
};

class UIManager {
private:
  vector<Button> buttons;

public:
  UIManager(){};
  void initializeButtons() {}

  void draw() {
    for (auto &button : buttons) {
      button.draw();
    }
  }

  void mouseClicked(int mouseX, int mouseY) {
    for (auto &button : buttons) {
      button.handleClick(mouseX, mouseY);
    }
  }
};

class Game {
private:
  int score;
  int time;
  Player player;
  EnemyController enemies;
  UIManager ui;

public:
  Game() {
    score = 0;
    time = 0;
    ui.initializeButtons();
  }

  void keyPressed(const int key) { player.keyPressed(key); }

  bool checkCollision(const Bullet &bullet, const Enemy &enemy) {
    return bullet.x >= enemy.x - 10 && bullet.x <= enemy.x + 10 &&
           bullet.y >= enemy.y - 10 && bullet.y <= enemy.y + 10;
  }

  void update() {
    player.update();

    // update enemies
    for (Enemy enemy : enemies.all) {
      enemy.update();
    }

    // update particles
    auto &bullets = player.bullets; // Get reference to player's bullets

    for (auto it = bullets.begin(); it != bullets.end();) {
      bool bulletRemoved = false;
      for (Bullet bullet : bullets) {
        bullet.draw();
      }

      for (auto enemyIt = enemies.all.begin(); enemyIt != enemies.all.end();
           ++enemyIt) {
        if (checkCollision(*it, *enemyIt)) {
          // Remove enemy and bullet
          enemyIt = enemies.all.erase(enemyIt);
          it = bullets.erase(it);
          bulletRemoved = true;
          break;
        }
      }

      if (!bulletRemoved) {
        ++it;
      }
    }
  };

  void drawBackground() {
    glColor3f(0.0, 0.0, 0.0);
    glBegin(GL_QUADS);
    glVertex2i(0, 0);
    glVertex2i(WIDTH, 0);
    glVertex2i(WIDTH, HEIGHT);
    glVertex2i(0, HEIGHT);
    glEnd();
  }

  void draw() {
    drawBackground();
    ui.draw();
    player.draw();
    for (Enemy enemy : enemies.all) {
      enemy.draw();
    }
  }

  void mouseClicked() {
    int lb, mb, rb, mx, my;
    mx = -1;
    int evt = FsGetMouseEvent(lb, mb, rb, mx, my);
  }
};

int main(void) {
  srand(time(0));
  FsOpenWindow(0, 0, WIDTH, HEIGHT, 1);
  Game app;
  while (true) { // main app loop
    FsPollDevice();
    int key = FsInkey();
    if (FSKEY_ESC == key) {
      break;
    }
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    app.keyPressed(key);
    app.update();
    app.draw();
    FsSwapBuffers();
    FsSleep(10);
  }
  return 0;
}
