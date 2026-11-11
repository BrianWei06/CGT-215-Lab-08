#include <iostream>
#include <SFML/Graphics.hpp>
#include <SFPhysics.h>
#include <vector>

using namespace std;
using namespace sf;
using namespace sfp;

const int WINDOW_WIDTH = 800;
const int WINDOW_HEIGHT = 600;
const int STARTING_ARROWS = 5;
const float DUCK_SPEED = 0.22f;  // Pixels per millisecond (like Balloon Buster)
const float ARROW_SPEED = -1.0f;


bool LoadTex(Texture& tex, string filename) {
    if (tex.loadFromFile("images/" + filename)) return true;
    if (tex.loadFromFile("Lab08/images/" + filename)) return true;
    cout << "Could not load images/" << filename << endl;
    return false;
}

bool LoadGameFont(Font& font) {
    // The provided file is named 'arial.ttf' (not 'arial.ttf').
    if (font.loadFromFile("arial.ttf")) return true;
    if (font.loadFromFile("Lab08/arial.ttf")) return true;
    cout << "Could not load arial.ttf." << endl;
    return false;
}

Vector2f GetTextSize(Text text) {
    FloatRect r = text.getGlobalBounds();
    return Vector2f(r.width, r.height);
}

int main() {
    RenderWindow window(VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Duck Crossbow");
    World world(Vector2f(0, 0));

    int score = 0;
    int arrowsLeft = STARTING_ARROWS;
    int duckTimerMS = 0;  // First duck appears immediately.
    bool drawingArrow = false;
    bool gameOver = false;
    bool spaceWasDown = false;

    Texture crossbowTex;
    Texture arrowTex;
    Texture duckTex;
    if (!LoadTex(crossbowTex, "crossbow.png") ||
        !LoadTex(arrowTex, "arrow.png") ||
        !LoadTex(duckTex, "duck.png")) {
        return 1;
    }

    Font fnt;
    if (!LoadGameFont(fnt)) return 2;


    const int duckDelayMS = static_cast<int>(
        (static_cast<float>(duckTex.getSize().x) + 90.0f) / DUCK_SPEED);

    PhysicsSprite crossBow;
    crossBow.setTexture(crossbowTex);
    Vector2f bowSize = crossBow.getSize();
    crossBow.setCenter(Vector2f(WINDOW_WIDTH / 2.0f,
                                WINDOW_HEIGHT - bowSize.y / 2.0f));

    PhysicsSprite arrow;
    arrow.setTexture(arrowTex);

    // A fired arrow is removed when it reaches the top of the screen.
    PhysicsRectangle top;
    top.setSize(Vector2f(WINDOW_WIDTH, 10));
    top.setCenter(Vector2f(WINDOW_WIDTH / 2.0f, 5));
    top.setStatic(true);
    world.AddPhysicsBody(top);

    // Ducks are removed when they reach the right edge of the screen.
    PhysicsRectangle right;
    right.setSize(Vector2f(10, WINDOW_HEIGHT));
    right.setCenter(Vector2f(WINDOW_WIDTH - 5.0f, WINDOW_HEIGHT / 2.0f));
    right.setStatic(true);
    world.AddPhysicsBody(right);

    PhysicsShapeList<PhysicsSprite> ducks;

    top.onCollision = [&drawingArrow, &world, &arrow]
        (PhysicsBodyCollisionResult result) {
        if (drawingArrow && result.object2 == arrow) {
            drawingArrow = false;
            world.RemovePhysicsBody(arrow);
        }
    };

    Text scoreText;
    scoreText.setFont(fnt);
    scoreText.setCharacterSize(26);
    scoreText.setFillColor(Color::White);

    Text arrowCountText;
    arrowCountText.setFont(fnt);
    arrowCountText.setCharacterSize(26);
    arrowCountText.setFillColor(Color::White);

    Text gameOverText;
    gameOverText.setFont(fnt);
    gameOverText.setCharacterSize(60);
    gameOverText.setFillColor(Color::Yellow);
    gameOverText.setString("GAME OVER");
    FloatRect overBounds = gameOverText.getLocalBounds();
    gameOverText.setOrigin(overBounds.left + overBounds.width / 2.0f,
                           overBounds.top + overBounds.height / 2.0f);
    gameOverText.setPosition(WINDOW_WIDTH / 2.0f, WINDOW_HEIGHT / 2.0f);

    Clock clock;
    Time lastTime = clock.getElapsedTime();

    while (window.isOpen()) {
        Event event;
        while (window.pollEvent(event)) {
            if (event.type == Event::Closed) window.close();
        }
        if (!window.isOpen()) break;

        Time currentTime = clock.getElapsedTime();
        int deltaMS = (currentTime - lastTime).asMilliseconds();
        if (deltaMS < 10) continue;
        lastTime = currentTime;
        if (deltaMS > 50) deltaMS = 50;


        bool spaceDown = Keyboard::isKeyPressed(Keyboard::Space);
        bool newSpacePress = spaceDown && !spaceWasDown;
        spaceWasDown = spaceDown;

        if (gameOver) {

            if (newSpacePress) {
                window.close();
                break;
            }
        } else {

            if (newSpacePress && !drawingArrow && arrowsLeft > 0) {
                drawingArrow = true;
                arrowsLeft--;
                arrow.setCenter(crossBow.getCenter());
                arrow.setVelocity(Vector2f(0, ARROW_SPEED));
                world.AddPhysicsBody(arrow);
            }


            duckTimerMS -= deltaMS;
            while (duckTimerMS <= 0) {
                duckTimerMS += duckDelayMS;
                PhysicsSprite& duck = ducks.Create();
                duck.setTexture(duckTex);
                Vector2f duckSize = duck.getSize();
                duck.setCenter(Vector2f(-duckSize.x / 2.0f - 10.0f, 130));
                duck.setVelocity(Vector2f(DUCK_SPEED, 0));
                world.AddPhysicsBody(duck);

                duck.onCollision =
                    [&drawingArrow, &world, &arrow, &duck, &ducks,
                     &score, &right, removed = false]
                    (PhysicsBodyCollisionResult result) mutable {
                    if (removed) return;

                    if (drawingArrow && result.object2 == arrow) {
                        removed = true;
                        drawingArrow = false;
                        world.RemovePhysicsBody(arrow);
                        world.RemovePhysicsBody(duck);
                        ducks.QueueRemove(duck);
                        score += 10;
                    } else if (result.object2 == right) {
                        removed = true;
                        world.RemovePhysicsBody(duck);
                        ducks.QueueRemove(duck);
                    }
                };
            }

            world.UpdatePhysics(deltaMS);
            ducks.DoRemovals();


            if (arrowsLeft == 0 && !drawingArrow) gameOver = true;
        }

        window.clear(Color(35, 60, 85));

        for (PhysicsShape& duck : ducks) {
            window.draw((PhysicsSprite&)duck);
        }
        window.draw(crossBow);
        if (drawingArrow) window.draw(arrow);

        arrowCountText.setString("Arrows: " + to_string(arrowsLeft));
        arrowCountText.setPosition(12, WINDOW_HEIGHT - 45.0f);
        window.draw(arrowCountText);

        scoreText.setString("Score: " + to_string(score));
        scoreText.setPosition(
            WINDOW_WIDTH - 12.0f - GetTextSize(scoreText).x,
            WINDOW_HEIGHT - 45.0f);
        window.draw(scoreText);

        if (gameOver) {
            RectangleShape panel(Vector2f(540, 130));
            panel.setPosition(130, 235);
            panel.setFillColor(Color(0, 0, 0, 190));
            window.draw(panel);
            window.draw(gameOverText);
        }

        window.display();
    }
    return 0;
}
