

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

class Particle {
private:
  int x, y;   // position
  int vx, vy; // velocity

public:
  Particle(int x, int y, int vx, int vy) : x(x), y(y), vx(vx), vy(vy){};

  // update particle position
  void update() {}

  // draw the particle
  void draw() {
    glColor3f(0.0, 1.0, 0.0);
    glBegin(GL_POINTS);
    glVertex2i(x, y);
    glEnd();
  }
};
class Player {
private:
  int health;
  int x, y;
  const int speed = 5;
  YsRawPngDecoder spaceshipDesign;
  vector<Particle> bullets;

public:
  Player();

  void update() {
    for (Particle bullet : bullets) {
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
    }
  }

  void draw() {
    for (Particle bullet : bullets) {
      bullet.draw();
    }
    // draw the player image
    glColor3f(1.0, 1.0, 1.0);
    glBegin(GL_TRIANGLES);
    glVertex2i(x - 10, y);
    glVertex2i(x, y - 20);
    glVertex2i(x + 10, y);
    glEnd();
  }
};

class Enemy {
private:
  int health;
  int x, y; //

  int moveDirection = 1;
  const int moveSpeed = 2;
  const int moveRange = 50;
  vector<Particle> bullets; // bullets fired by the enemy
  int moveCounter = 0;
  int shootTimer = 0;
  const int shootInterval = 60;     // frames between shots
  YsRawPngDecoder *spaceshipDesign; // pointer to the image

public:
  Enemy(const string &enemyType, YsRawPngDecoder *img) {
    // initialize other attributes
    spaceshipDesign = img;
  }

  void update() {
    // update position, velocity, acceleration
    if (shootTimer++ >= shootInterval) {
      shootTimer = 0;
      Particle bullet(x, y, 0, 5); //
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
private:
  vector<Enemy> enemies;

public:
  EnemyController(){
      // initialize enemies
  };

  // update all enemies
  void update() {
    for (Enemy enemy : enemies) {
      enemy.update();
    }
  }
};

class Button {
private:
  int x, y, width, height;
  string text;
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
  vector<Enemy> enemies;
  Player player;
  UIManager ui;
  unordered_map<string, YsRawPngDecoder> enemyImages; // store enemy images

public:
  Game() {
    score = 0;
    time = 0;
    loadEnemyImages();
    ui.initializeButtons();
  }

  void loadEnemyImages() {
    // load images and add them to enemyImages map
    // e.g. enemyImages["enemy_1"] = loadPng("path_to_type1_image.png");
  } // load images

  YsRawPngDecoder *getEnemyImage(const string &enemyType) {
    auto it = enemyImages.find(enemyType);
    if (it != enemyImages.end()) {
      return &it->second;
    }
    return nullptr;
  }

  void keyPressed(const int key) { player.keyPressed(key); }

  void update() {
    player.update();
    for (Enemy enemy : enemies) {
      enemy.update();
    }
  };

  void draw() {
    player.draw();
    for (Enemy enemy : enemies) {
      enemy.draw();
    }
    ui.draw();
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
