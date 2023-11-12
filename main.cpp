

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

public:
  Player() {}
  ~Player() {}

  void update() {
  }

  void draw() {
  }
};

class Enemy {
private:

public:
  Enemy() {}
  ~Enemy() {}

  void update() {
  }

  void draw() {
  }
};

class Game {
private:

public:
  Game() {}
  ~Game() {}

  void update() {
  }

  void draw() {
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
