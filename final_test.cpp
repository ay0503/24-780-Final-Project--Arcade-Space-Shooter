#include "fssimplewindow.h"
#include "yspng.h"
#include "yssimplesound.h"
#include <cstdlib>
#include <iostream>
#include <math.h>
#include <queue>
#include <stdio.h>
#include <time.h>
#include <unordered_map>
#include <vector>
#include <chrono>
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

class Image
{
private:
    YsRawPngDecoder png;

public:
    Image() {};
    void LoadPNG(const char fn[])
    {
        if (YSOK == png.Decode(fn))
        {
            printf("Read Width=%d Height=%d\n", png.wid, png.hei);
            png.Flip();
        }
        else
        {
            printf("Read Error!\n");
            return;
        }
    }
    YsRawPngDecoder& GetPNG()
    {
        return png;
    }
    void Render(YsRawPngDecoder& png)
    {
        glDrawPixels(png.wid, png.hei, GL_RGBA, GL_UNSIGNED_BYTE, png.rgba);
        glFlush();
    }
};

class SplashScreen
{
public:
    bool gameStart = false;
    bool storyStart = false;

    Image splash;
    Image title;
    Image start;
    Image base;
    Image story;
    std::chrono::time_point<std::chrono::system_clock> timer1 = std::chrono::system_clock::now();
    std::chrono::time_point<std::chrono::system_clock> timer2 = std::chrono::system_clock::now();
    int scrollOffset = 0;
    int storyOffset = 764;

    SplashScreen() {
        splash.LoadPNG("splash_screen_2.png");
        title.LoadPNG("title.png");
        start.LoadPNG("start.png");
        base.LoadPNG("splash_screen_1.png");
        story.LoadPNG("story.png");
    };

    void RenderTitle(YsRawPngDecoder& png)
    {
        glRasterPos2i(20, 300);
        glDrawPixels(png.wid, png.hei, GL_RGBA, GL_UNSIGNED_BYTE, png.rgba);
    }

    void RenderStart(YsRawPngDecoder& png)
    {
        glRasterPos2i(174, 500);
        glDrawPixels(png.wid, png.hei, GL_RGBA, GL_UNSIGNED_BYTE, png.rgba);
    }

    void UpdateScroll(void)
    {
        timer2 = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(timer2 - timer1).count();
        if (elapsed >= 50)
        {
            if (scrollOffset != 5000 - 100) // png.hei - win.hei
            {
                scrollOffset = scrollOffset + 100;
                timer1 = timer2;
            }
            else
            {
                scrollOffset = 0;
                timer1 = timer2;
            }
        }
    }

    void RenderBase(YsRawPngDecoder& png)
    {
        glRasterPos2i(250, 899);
        UpdateScroll();
        unsigned char* visablePart = png.rgba + scrollOffset * png.wid * 4;
        glDrawPixels(100, 100, GL_RGBA, GL_UNSIGNED_BYTE, visablePart);
    }

    void keyPressed(const int key) {
        switch (key) {
        case FSKEY_ENTER:
            gameStart = true;
        }
    }

    void UpdateStory(void)
    {
        timer2 = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(timer2 - timer1).count();
        if (elapsed >= 5)
        {
            if (storyOffset != 0)
            {
                storyOffset = storyOffset - 1;
                timer1 = timer2;
            }
            else
            {
                FsSleep(3000);
                storyStart = true;
            }
        }
    }

    void RenderStory(YsRawPngDecoder& png)
    {
        glRasterPos2i(39, 842);
        UpdateStory();
        //unsigned char* visablePart = png.rgba - scrollOffset * png.wid * 4;
        glDrawPixels(png.wid, png.hei, GL_RGBA, GL_UNSIGNED_BYTE, png.rgba);
    }
};

class Particle {
//private:
//    Image power;

public:
    int x, y;   // position
    int vx, vy; // velocity
    int powerLevel = 0; // powerups

    //Particle() {
    //    if (powerLevel < 2)
    //    {
    //        power.LoadPNG("power_1.png");
    //    }
    //    else if (powerLevel >= 2 )
    //    {
    //        power.LoadPNG("power_2.png");
    //    }
    //}

    Particle(int x, int y, int vx, int vy, int powerLevel) : x(x), y(y), vx(vx), vy(vy), powerLevel(powerLevel) {};

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
    //void draw() {
    //    glRasterPos2i(x, y);
    //    power.Render(power.GetPNG());
    //}
};

class Player {
private:
    int health;
    const int speed = 5;
    Image player_ship_left;
    Image player_ship_center;
    Image player_ship_right;
    Image player_ship;
    int powerLevel = 0;

public:
    int x, y;
    vector<Particle> bullets;

    // for different move
    int move = 0;
    std::chrono::time_point<std::chrono::system_clock> timer1 = std::chrono::system_clock::now();  
    std::chrono::time_point<std::chrono::system_clock> timer2 = std::chrono::system_clock::now(); 

    Player() {
        // initialize other attributes
        x = WIDTH / 2;
        y = HEIGHT - 100;
        player_ship_left.LoadPNG("player_ship_left.png");   // 32x32 left center right each
        player_ship_center.LoadPNG("player_ship_center.png");
        player_ship_right.LoadPNG("player_ship_right.png");
        player_ship.LoadPNG("player_ship_center.png");
    };

    void update() {
        for (auto& bullet : bullets) {
            bullet.update();
        }
        // update position, velocity, acceleration
    }

    void keyPressed(const int key) {
        switch (key) {
        case FSKEY_UP:
            y -= speed;
            move = 0;
            break;
        case FSKEY_DOWN:
            y += speed;
            move = 0;
            break;
        case FSKEY_LEFT:
            x -= speed;
            move = 1;
            break;
        case FSKEY_RIGHT:
            x += speed;
            move = 2;
            break;
        case FSKEY_SPACE:
            shoot();
            break;
        case FSKEY_Z: // for testing purpose
            powerup();
            break;
        default:
            timer2 = std::chrono::system_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(timer2 - timer1).count();
            if (elapsed >= 150)
            {
                move = 0;
                timer1 = timer2;
            }
        }
    }

    void powerup() { powerLevel++; } // for testing purpose

    void shoot() { 
        if (powerLevel == 0)
        {
            bullets.push_back(Particle(x, y, 0, -15, powerLevel));
        }
        else if (powerLevel == 1)
        {
            bullets.push_back(Particle(x + 10, y, 0, -15, powerLevel));
            bullets.push_back(Particle(x - 10, y, 0, -15, powerLevel));
        }
        else if (powerLevel == 2)
        {
            bullets.push_back(Particle(x, y, 0, -15, powerLevel));
        }
        else if (powerLevel >= 3)
        {
            bullets.push_back(Particle(x + 10, y, 0, -15, powerLevel));
            bullets.push_back(Particle(x - 10, y, 0, -15, powerLevel));
        }
    }

    void draw() {
        //// draw the player image
        //glColor3f(1.0, 1.0, 1.0);
        //glBegin(GL_TRIANGLES);
        //glVertex2i(x - 10, y);
        //glVertex2i(x, y - 20);
        //glVertex2i(x + 10, y);
        //glEnd();

        glRasterPos2i(x - player_ship.GetPNG().wid / 2, y);
        if (move == 0) 
        {
            player_ship.Render(player_ship.GetPNG());
        }
        else if (move == 1)
        {
            player_ship.Render(player_ship_left.GetPNG());
            //move = 0;
        }
        else if (move == 2)
        {
            player_ship.Render(player_ship_right.GetPNG());
            //move = 0;
        }

        for (auto& bullet : bullets) {
            bullet.draw();
        }
    }
};

class EnemyFigureTemplate {
private:
    Image enemy_figure_1;
    Image enemy_figure_2;
    Image enemy_figure_3;
    Image enemy_figure_4;
    Image enemy_figure_5;

public:
    EnemyFigureTemplate() {
        enemy_figure_1.LoadPNG("enemy_1.png");
        enemy_figure_2.LoadPNG("enemy_2.png");
        enemy_figure_3.LoadPNG("enemy_3.png");
        enemy_figure_4.LoadPNG("enemy_4.png");
        enemy_figure_5.LoadPNG("enemy_5.png");
    };

    void draw() {
        glRasterPos2i(50, 300);
        enemy_figure_1.Render(enemy_figure_1.GetPNG());
        glRasterPos2i(150, 300);
        enemy_figure_2.Render(enemy_figure_2.GetPNG());
        glRasterPos2i(250, 300);
        enemy_figure_3.Render(enemy_figure_3.GetPNG());
        glRasterPos2i(350, 300);
        enemy_figure_4.Render(enemy_figure_4.GetPNG());
        glRasterPos2i(450, 300);
        enemy_figure_5.Render(enemy_figure_5.GetPNG());
    }
};

class PowerupsFigureTemplate {
private:
    Image powerups_figure_1;
    Image powerups_figure_2;
    Image powerups_figure_3;

public:
    PowerupsFigureTemplate() {
        powerups_figure_1.LoadPNG("powerups_1.png");
        powerups_figure_2.LoadPNG("powerups_2.png");
        powerups_figure_3.LoadPNG("powerups_3.png");
    };

    void draw() {
        glRasterPos2i(50, 600);
        powerups_figure_1.Render(powerups_figure_1.GetPNG());
        glRasterPos2i(150, 600);
        powerups_figure_2.Render(powerups_figure_2.GetPNG());
        glRasterPos2i(250, 600);
        powerups_figure_3.Render(powerups_figure_3.GetPNG());
    }
};

class Enemy {
private:
    int health;

    int moveDirection = 1;
    int moveSpeed = 2;
    int moveRange = 50;
    vector<Particle> bullets; // bullets fired by the enemy
    int moveCounter = 0;
    int shootTimer = 0;
    int shootInterval = 60;           // frames between shots
    YsRawPngDecoder* spaceshipDesign; // pointer to the image

public:
    int x, y; // position
    Enemy(const string& enemyType, YsRawPngDecoder* img, int x, int y)
        : x(x), y(y) {
        // initialize other attributes
        spaceshipDesign = img;
    }

    void update() {
        // update position, velocity, acceleration
        if (shootTimer++ >= shootInterval) {
            shootTimer = 0;
            Particle bullet(x, y, 0, 5, 0); //
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

    YsRawPngDecoder* getEnemyImage(const string& enemyType) {
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
    UIManager() {};
    void initializeButtons() {}

    void draw() {
        for (auto& button : buttons) {
            button.draw();
        }
    }

    void mouseClicked(int mouseX, int mouseY) {
        for (auto& button : buttons) {
            button.handleClick(mouseX, mouseY);
        }
    }
};

class Scroll {
public:
    int level = 1; // 3 levels?
    std::chrono::time_point<std::chrono::system_clock> timer1 = std::chrono::system_clock::now(); // level 1 starts
    std::chrono::time_point<std::chrono::system_clock> timer2 = std::chrono::system_clock::now(); // time passed in level1
    int scrollOffset = 0;

    Scroll() {};
    void UpdateScroll(void)
    {
        timer2 = std::chrono::system_clock::now();
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(timer2 - timer1).count();
        if (elapsed >= 50)
        {
            if (scrollOffset != 3000 - 900) // png.hei - win.hei
            {
                scrollOffset = scrollOffset + 1;
                timer1 = timer2;
            }
        }
    }
    void Render(YsRawPngDecoder& png)
    {
        //glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

        int winWid, winHei;
        FsGetWindowSize(winWid, winHei);
        glRasterPos2d(0.0, (double)(winHei - 1));
        UpdateScroll();
        unsigned char* visablePart = png.rgba + scrollOffset * png.wid * 4;
        glDrawPixels(winWid, winHei, GL_RGBA, GL_UNSIGNED_BYTE, visablePart);

        //FsSwapBuffers();
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
    EnemyFigureTemplate enemyshowcase; // for showcase different enermies
    PowerupsFigureTemplate powerups; // for showcase different powerups
    SplashScreen splashScreen;

public:
    Game() {
        score = 0;
        time = 0;
        ui.initializeButtons();
        map.LoadPNG("Space_Background.png");
    }

    void keyPressed(const int key) { 
        player.keyPressed(key); 
        splashScreen.keyPressed(key);
    }

    bool checkCollision(const Particle& bullet, const Enemy& enemy) {
        int enemyLeft = enemy.x - 10;
        int enemyRight = enemy.x + 10;
        int enemyTop = enemy.y - 10;
        int enemyBottom = enemy.y + 10;
        return bullet.x >= enemyLeft && bullet.x <= enemyRight &&
            bullet.y >= enemyTop && bullet.y <= enemyBottom;
    }

    void update() {
        player.update();

        // update enemies
        for (Enemy enemy : enemies.all) {
            enemy.update();
        }

        // update particles
        auto& bullets = player.bullets; // Get reference to player's bullets

        for (auto it = bullets.begin(); it != bullets.end();) {
            bool bulletRemoved = false;
            for (auto& bullet : bullets) {
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
        background.Render(map.GetPNG());
    }

    void draw() {
        drawBackground();
        ui.draw();
        player.draw();
        enemyshowcase.draw(); // for showcase different enermies
        powerups.draw(); // for showcase different enermies
        for (Enemy enemy : enemies.all) {
            enemy.draw();
        }
    }

    void mouseClicked() {
        int lb, mb, rb, mx, my;
        mx = -1;
        int evt = FsGetMouseEvent(lb, mb, rb, mx, my);
    }

    bool gameStart() {
        return splashScreen.gameStart;
    }

    bool storyStart() {
        return splashScreen.storyStart;
    }

    void drawSplashScreen() {
        glRasterPos2i(0, 899);
        splashScreen.splash.Render(splashScreen.splash.GetPNG());
        splashScreen.RenderTitle(splashScreen.title.GetPNG());
        splashScreen.RenderStart(splashScreen.start.GetPNG());
        splashScreen.RenderBase(splashScreen.base.GetPNG());
    }

    void drawStory() {
        glRasterPos2i(0, 899);
        splashScreen.splash.Render(splashScreen.splash.GetPNG());
        splashScreen.RenderStory(splashScreen.story.GetPNG());
    }
};

int main(void) {
    srand(time(0));
    FsOpenWindow(0, 0, WIDTH, HEIGHT, 1);
    Game app;

    // enable alpha blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    while (true) { // main app loop
        FsPollDevice();
        int key = FsInkey();
        if (FSKEY_ESC == key) {
            break;
        }
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //app.keyPressed(key);
        //app.update();
        //app.draw();

        if (app.gameStart() && app.storyStart()) {
            app.keyPressed(key);
            app.update();
            app.draw();
        }
        else if (app.gameStart() && !app.storyStart()) {
            app.drawStory();
        }
        else {
            app.drawSplashScreen();
            app.keyPressed(key);
        }
        FsSwapBuffers();
        FsSleep(10);
    }
    return 0;
}
