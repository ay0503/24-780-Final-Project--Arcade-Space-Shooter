

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

class Player;
class Enemy;
class UIManager;
class Particle;

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

  void keyPressed(const int key) {
    // handle key press
  }

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
};

class Player {
private:
  int health;
  YsRawPngDecoder spaceshipDesign;
  vector<Particle> bullets; // bullets fired by the player

public:
  Player();

  void update() {
    for (Particle bullet : bullets) {
      bullet.update();
    }
    // update position, velocity, acceleration
  }

  void draw() {
    for (Particle bullet : bullets) {
      bullet.draw();
    }
    // draw the player image
  }
};

class Enemy {
private:
  int health;
  YsRawPngDecoder *spaceshipDesign; // pointer to the image

public:
  Enemy(const string &enemyType, Game *game) {
    // initialize other attributes
    spaceshipDesign = game->getEnemyImage(enemyType);
  }

  void update() {
    // update position, velocity, acceleration
  }
  void draw() {
    // draw the enemy
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

class UIManager {
public:
  UIManager();

  // draw UI elements
  void draw() {}
};

class Particle {
private:
  int x, y;   // position
  int vx, vy; // velocity

public:
  Particle(int x, int y, int vx, int vy);

  // update particle position
  void update() {}

  // draw the particle
  void draw() {}
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
