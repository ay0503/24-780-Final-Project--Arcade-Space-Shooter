

#include "libraries/fssimplewindow.h"
#include "libraries/yspng.h"
#include "libraries/yssimplesound.h"
#include <chrono>
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

class Image {
private:
  YsRawPngDecoder png;

public:
  Image(){};
  void LoadPNG(const char fn[]) {
    if (YSOK == png.Decode(fn)) {
      printf("Read Width=%d Height=%d\n", png.wid, png.hei);
      png.Flip();
    } else {
      printf("Read Error!\n");
      return;
    }
  }
  YsRawPngDecoder &GetPNG() { return png; }
  void Render(YsRawPngDecoder &png) {

    glDrawPixels(png.wid, png.hei, GL_RGBA, GL_UNSIGNED_BYTE, png.rgba);
  }
};

class Bullet {
public:
  int x, y;   // position
  int vx, vy; // velocity
  int damage;
  int powerLevel = 0; // powerups

  Bullet(int x, int y, int vx, int vy, int powerLevel)
      : x(x), y(y), vx(vx), vy(vy), powerLevel(powerLevel){};

  // update particle position
  void update() {
    x += vx;
    y += vy * (powerLevel + 1);
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
  const int speed = 10;
  Image player_ship_left;
  Image player_ship_center;
  Image player_ship_right;
  Image player_ship;

public:
  int x, y;
  bool isAlive = true;
  int powerLevel = 0;
  vector<Bullet> bullets;
  int move = 0;
  chrono::time_point<chrono::system_clock> timer1 = chrono::system_clock::now();
  chrono::time_point<chrono::system_clock> timer2 = chrono::system_clock::now();

  Player() {
    x = WIDTH / 2;
    y = HEIGHT - 100;
    player_ship_left.LoadPNG(
        "player_ship_left.png"); // 32x32 left center right each
    player_ship_center.LoadPNG("player_ship_center.png");
    player_ship_right.LoadPNG("player_ship_right.png");
    player_ship.LoadPNG("player_ship_center.png");
  };

  void update() {
    for (auto it = bullets.begin(); it != bullets.end();) {
      it->update();

      if (it->y < 0) {
        it = bullets.erase(it);
      } else {
        ++it;
      }
    }

    // update position, velocity, acceleration
  }

  void keyPressed(const unordered_map<int, bool> &keyStatus) {
    if (keyStatus.at(FSKEY_UP)) {
      y -= speed;
      move = 0;
    }
    if (keyStatus.at(FSKEY_DOWN)) {
      y += speed;
      move = 0;
    }
    if (keyStatus.at(FSKEY_LEFT)) {
      x -= speed;
      move = 1;
    }
    if (keyStatus.at(FSKEY_RIGHT)) {
      x += speed;
      move = 2;
    }
    if (keyStatus.at(FSKEY_SPACE)) {
      shoot();
    }
    if (keyStatus.at(FSKEY_Z)) {
      powerup();
    }
  }

  void powerup() { powerLevel++; } // for testing purpose

  void shoot() {
    if (powerLevel == 0) {
      bullets.push_back(Bullet(x, y, 0, -15, powerLevel));
    } else if (powerLevel == 1) {
      bullets.push_back(Bullet(x + 10, y, 0, -15, powerLevel));
      bullets.push_back(Bullet(x - 10, y, 0, -15, powerLevel));
    } else if (powerLevel == 2) {
      bullets.push_back(Bullet(x, y, 0, -15, powerLevel));
    } else if (powerLevel >= 3) {
      bullets.push_back(Bullet(x + 10, y, 0, -15, powerLevel));
      bullets.push_back(Bullet(x - 10, y, 0, -15, powerLevel));
    }
  }

  void draw() {
    // draw the player image
    glRasterPos2i(x - player_ship.GetPNG().wid / 2, y);
    if (move == 0) {
      player_ship.Render(player_ship.GetPNG());
    } else if (move == 1) {
      player_ship.Render(player_ship_left.GetPNG());
    } else if (move == 2) {
      player_ship.Render(player_ship_right.GetPNG());
    }
    for (Bullet bullet : bullets) {
      bullet.draw();
    }
  }
};

class Enemy {
private:
  int health;

  int moveDirection = 1;
  int moveSpeed = 1;
  int moveRange = 50;
  vector<Bullet> bullets; // bullets fired by the enemy
  int moveCounter = 0;
  int shootTimer = 0;
  int shootInterval = 60; // frames between shots

public:
  int x, y; // position
  bool isAlive = true;
  Enemy(int x, int y) : x(x), y(y) {
    // initialize other attributes
  }

  void moveDown(int rate) { y += rate; }

  void update() {
    moveDown(moveSpeed);
    // update position, velocity, acceleration
    x += moveDirection * moveSpeed;
    if (++moveCounter >= moveRange) {
      moveDirection *= -1;
      moveCounter = 0;
    }
  }
};

class EnemyFigureTemplate {
private:
  Image enemy_figure_1, enemy_figure_2, enemy_figure_3, enemy_figure_4,
      enemy_figure_5;

public:
  Enemy enemy1, enemy2, enemy3, enemy4, enemy5;
  EnemyFigureTemplate()
      : enemy1(50, 300), enemy2(150, 300), enemy3(250, 300), enemy4(350, 300),
        enemy5(450, 300) {
    enemy_figure_1.LoadPNG("enemy_1.png");
    enemy_figure_2.LoadPNG("enemy_2.png");
    enemy_figure_3.LoadPNG("enemy_3.png");
    enemy_figure_4.LoadPNG("enemy_4.png");
    enemy_figure_5.LoadPNG("enemy_5.png");
  };

  bool allEnemiesGone() const {
    return !enemy1.isAlive && !enemy2.isAlive && !enemy3.isAlive &&
           !enemy4.isAlive && !enemy5.isAlive;
  }

  void respawnEnemies() {
    // Reset the positions and set isAlive to true for all enemies
    enemy1 = Enemy(50, 300);
    enemy1.isAlive = true;
    enemy2 = Enemy(150, 300);
    enemy2.isAlive = true;
    enemy3 = Enemy(250, 300);
    enemy3.isAlive = true;
    enemy4 = Enemy(350, 300);
    enemy4.isAlive = true;
    enemy5 = Enemy(450, 300);
    enemy5.isAlive = true;
  }

  void draw() {
    if (enemy1.isAlive) {
      glRasterPos2i(enemy1.x, enemy1.y);
      enemy_figure_1.Render(enemy_figure_1.GetPNG());
    }
    if (enemy2.isAlive) {
      glRasterPos2i(enemy2.x, enemy2.y);
      enemy_figure_2.Render(enemy_figure_2.GetPNG());
    }
    if (enemy3.isAlive) {
      glRasterPos2i(enemy3.x, enemy3.y);
      enemy_figure_3.Render(enemy_figure_3.GetPNG());
    }
    if (enemy4.isAlive) {
      glRasterPos2i(enemy4.x, enemy4.y);
      enemy_figure_4.Render(enemy_figure_4.GetPNG());
    }
    if (enemy5.isAlive) {
      glRasterPos2i(enemy5.x, enemy5.y);
      enemy_figure_5.Render(enemy_figure_5.GetPNG());
    }
  }
};

class Powerup {
public:
  int x, y; // position
  bool isAlive = true;
  Powerup(int x, int y) : x(x), y(y) {
    // initialize other attributes
  }

  void update() {
    // update position, velocity, acceleration
  }
};

class PowerupsFigureTemplate {
public:
  Image powerups_figure_1, powerups_figure_2, powerups_figure_3;
  Powerup powerup1 = Powerup(50, 600);
  Powerup powerup2 = Powerup(150, 600);
  Powerup powerup3 = Powerup(250, 600);

  PowerupsFigureTemplate() {
    powerups_figure_1.LoadPNG("powerups_1.png");
    powerups_figure_2.LoadPNG("powerups_2.png");
    powerups_figure_3.LoadPNG("powerups_3.png");
  };

  void draw() {
    if (powerup1.isAlive) {
      glRasterPos2i(50, 600);
      powerups_figure_1.Render(powerups_figure_1.GetPNG());
    }
    if (powerup2.isAlive) {
      glRasterPos2i(150, 600);
      powerups_figure_2.Render(powerups_figure_2.GetPNG());
    }
    if (powerup3.isAlive) {
      glRasterPos2i(250, 600);
      powerups_figure_3.Render(powerups_figure_3.GetPNG());
    }
  }
};

class EnemyController {
public:
  vector<Enemy> all;
  unordered_map<string, YsRawPngDecoder> enemyImages; // store enemy images
  EnemyController() {
    for (int i = 0; i < 10; i++) {
      int x = 50 + i * (WIDTH - 100) / 9;
      int y = 100;
      Enemy enemy(x, y);
      all.push_back(enemy);
    }
  };

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

class Scroll {
public:
  int level = 1; // 3 levels?
  std::chrono::time_point<std::chrono::system_clock> timer1 =
      std::chrono::system_clock::now(); // level 1 starts
  std::chrono::time_point<std::chrono::system_clock> timer2 =
      std::chrono::system_clock::now(); // time passed in level1
  int scrollOffset = 0;

  Scroll(){};
  void updateScroll(void) {
    timer2 = std::chrono::system_clock::now();
    auto elapsed =
        std::chrono::duration_cast<std::chrono::milliseconds>(timer2 - timer1)
            .count();
    if (elapsed >= 50) {
      if (scrollOffset != 3000 - 900) // png.hei - win.hei
      {
        scrollOffset = scrollOffset + 1;
        timer1 = timer2;
      }
    }
  }
  void Render(YsRawPngDecoder &png) {
    int winWid, winHei;
    FsGetWindowSize(winWid, winHei);
    glRasterPos2d(0.0, (double)(winHei - 1));
    updateScroll();
    unsigned char *visablePart = png.rgba + scrollOffset * png.wid * 4;
    glDrawPixels(winWid, winHei, GL_RGBA, GL_UNSIGNED_BYTE, visablePart);
  }
};

class Game {
private:
  int score;
  int time;
  Player player;
  EnemyController enemies;
  UIManager ui;
  Scroll background;
  Image map;
  EnemyFigureTemplate enemy;       // for showcase different enermies
  PowerupsFigureTemplate powerups; // for showcase different powerups
  unordered_map<int, bool> keyStatus;

public:
  bool isOver = false;
  Game() {
    score = 0;
    time = 0;
    ui.initializeButtons();
    map.LoadPNG("Space_Background.png");
    keyStatus[FSKEY_UP] = false;
    keyStatus[FSKEY_DOWN] = false;
    keyStatus[FSKEY_LEFT] = false;
    keyStatus[FSKEY_RIGHT] = false;
    keyStatus[FSKEY_SPACE] = false;
    keyStatus[FSKEY_Z] = false;
  }

  void keyPressed(int key, bool isDown) { keyStatus[key] = isDown; }

  bool checkCollision(const Bullet &bullet, const Enemy &enemy) {
    return bullet.x >= enemy.x - 10 && bullet.x <= enemy.x + 10 &&
           bullet.y >= enemy.y - 10 && bullet.y <= enemy.y + 10;
  }

  void checkBulletCollisions(Enemy &enemy) {
    for (auto it = player.bullets.begin(); it != player.bullets.end();) {
      if (isCollidingBullet(*it, enemy)) {
        enemy.isAlive = false;
        cout << "Bullet collided with enemy" << endl;
        it = player.bullets.erase(it); // Remove the bullet
      } else {
        ++it;
      }
    }
  }

  bool isCollidingBullet(const Bullet &bullet, const Enemy &enemy) {
    return bullet.x >= enemy.x - 32 / 2 && bullet.x <= enemy.x + 32 / 2 &&
           bullet.y >= enemy.y - 32 / 2 && bullet.y <= enemy.y + 32 / 2;
  }

  void checkCollisionsWithPowerups(Player &player) {
    if (isColliding(player, powerups.powerup1)) {
      cout << "colliding" << endl;
      powerups.powerup1.isAlive = false;
      player.powerLevel = 1;
    }
    if (isColliding(player, powerups.powerup2)) {
      cout << "colliding" << endl;
      powerups.powerup2.isAlive = false;
      player.powerLevel = 2;
    }
    if (isColliding(player, powerups.powerup3)) {
      cout << "colliding" << endl;
      powerups.powerup3.isAlive = false;
      player.powerLevel = 3;
    }
  }

  bool isColliding(const Player &player, const Powerup &powerup) {
    cout << "enemy x: " << player.x << " player y: " << player.y << endl;
    cout << "powerup x: " << powerup.x << " powerup y: " << powerup.y << endl;
    return player.x < powerup.x + 35 && player.x + 32 > powerup.x &&
           player.y < powerup.y + 35 && player.y + 32 > powerup.y;
  }

  void updateEnemy() {
    enemy.enemy1.update();
    enemy.enemy2.update();
    enemy.enemy3.update();
    enemy.enemy4.update();
    enemy.enemy5.update();

    checkBulletCollisions(enemy.enemy1);
    checkBulletCollisions(enemy.enemy2);
    checkBulletCollisions(enemy.enemy3);
    checkBulletCollisions(enemy.enemy4);
    checkBulletCollisions(enemy.enemy5);
  }

  void update() {
    player.keyPressed(keyStatus);
    player.update();
    checkCollisionsWithPowerups(player);

    // update enemies
    updateEnemy();

    // check if all enemies are gone and respawn them
    if (enemy.allEnemiesGone()) {
      enemy.respawnEnemies();
    }
    if (!player.isAlive) {
      cout << "Game Over" << endl;
      isOver = true;
    }

    // update particles
    auto &bullets = player.bullets;

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

  void drawBackground() { background.Render(map.GetPNG()); }

  void draw() {
    drawBackground();
    ui.draw();
    player.draw();
    enemy.draw();
    powerups.draw();
    enemy.draw();
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
    app.keyPressed(FSKEY_UP, FsGetKeyState(FSKEY_UP));
    app.keyPressed(FSKEY_DOWN, FsGetKeyState(FSKEY_DOWN));
    app.keyPressed(FSKEY_LEFT, FsGetKeyState(FSKEY_LEFT));
    app.keyPressed(FSKEY_RIGHT, FsGetKeyState(FSKEY_RIGHT));
    app.keyPressed(FSKEY_SPACE, FsGetKeyState(FSKEY_SPACE));
    app.keyPressed(FSKEY_Z, FsGetKeyState(FSKEY_Z));
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    app.update();
    if (app.isOver) {
      break;
    }
    app.draw();
    FsSwapBuffers();
    FsSleep(10);
  }
  return 0;
}
