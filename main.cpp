#include <iostream>
#include <cmath>
#include <ctime>
#include <string>
#include <vector>
#include <fstream>
#include <cstdlib>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>

using namespace sf;
using namespace std;

bool isCollision(Sprite& sprite1, Sprite& sprite2) {
    return sprite1.getGlobalBounds().intersects(sprite2.getGlobalBounds());
}
const int MAX_SCORES = 20;
int scores[MAX_SCORES];
char usernames[MAX_SCORES][50]; // Assuming max username length of 50
int scoreCount = 0;
void readHighScores() {
    ifstream file("highscores.txt");
    scoreCount = 0;
    while (file >> usernames[scoreCount] >> scores[scoreCount]) {
        scoreCount++;
        if (scoreCount == MAX_SCORES) break;
    }
    file.close();
}
void sortHighScores() {
    for (int i = 0; i < scoreCount - 1; ++i) {
        for (int j = 0; j < scoreCount - i - 1; ++j) {
            if (scores[j] < scores[j + 1]) {
                // Swap scores
                int tempScore = scores[j];
                scores[j] = scores[j + 1];
                scores[j + 1] = tempScore;

                // Swap corresponding usernames
                char tempUsername[50];
                strcpy_s(tempUsername, usernames[j]);
                strcpy_s(usernames[j], usernames[j + 1]);
                strcpy_s(usernames[j + 1], tempUsername);
            }
        }
    }
}
void addHighScore(const char* username, int score) {
    if (scoreCount < MAX_SCORES) {
        scores[scoreCount] = score;
        strcpy_s(usernames[scoreCount], username);
        scoreCount++;
    }
    else {
        // Optional: Replace the lowest score if the new score is higher
    }
    sortHighScores();
}
void writeHighScores() {
    ofstream file("highscores.txt", ios::app); // Open the file in append mode
    for (int i = 0; i < scoreCount; ++i) {
        file << usernames[i] << " " << scores[i] << endl;
    }
    file.close();
}
int main()
{
    const int maxUsernameLength = 50; // Maximum username length
    char username[maxUsernameLength];

    std::cout << "Enter your username : ";
    std::cin.getline(username, maxUsernameLength);

    std::cout << "Hello, " << username << "!\n";
    //MENU 
    int selectedItemIndex = 0;
    bool showMenu = true;  // Flag to show menu
    bool soundOn = true;   // Flag for sound
    bool REPLAY = false;

    // Initialize menu items
    Font font;
    font.loadFromFile("Cyborg Punk.ttf"); // Make sure to handle font loading errors

    Text startGame("START GAME", font, 30);
    startGame.setPosition(320, 200);

    Text highScores("SCORES", font, 30);
    highScores.setPosition(320, 300);

    Text exitGame("EXIT", font, 30);
    exitGame.setPosition(320, 400);

    Text soundToggle("SOUND ON", font, 30);
    soundToggle.setPosition(320, 500);


    int lives = 3;
    float mvspeed = 1.f;
    double xpos = 790 / 2.0, ypos = 790 - 100;
    srand(time(0));
    int score = 0;

    // Adjustments for 1080p window
    RenderWindow window(VideoMode(790, 790), "Space Shooter");
    window.setFramerateLimit(240);

    // Load and scale background
    Sprite background;
    Texture bg_texture;
    bg_texture.loadFromFile("background1.png");
    background.setTexture(bg_texture);
    sf::Text scoreText;
    scoreText.setFont(font);
    scoreText.setCharacterSize(24);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(790 / 2 - 110, 42);
    sf::Text livesText;
    livesText.setFont(font);
    livesText.setCharacterSize(24);
    livesText.setFillColor(sf::Color::Red);
    livesText.setPosition(790 - 200, 750);

    sf::Text timeText;
    timeText.setFont(font);
    timeText.setCharacterSize(24);
    timeText.setFillColor(sf::Color::White);
    timeText.setPosition(600, 10);
    sf::Texture spaceshipTexture;
    spaceshipTexture.loadFromFile("ship.png");
    sf::Sprite spaceshipSprite;
    spaceshipSprite.setTexture(spaceshipTexture);
    spaceshipSprite.setScale(0.5, 0.5);
    Clock clock;
    float timer = 0;
    int timer2 = 0;

    sf::Texture bulletTexture;
    if (!bulletTexture.loadFromFile("bullet.png")) {
        std::cout << "Error: Failed to load bullet texture!" << std::endl;
        return -1;
    }

    sf::Sprite bulletSprite;
    bulletSprite.setTexture(bulletTexture);
    bool bulletActive = false;


    sf::Texture stoneTexture;
    if (!stoneTexture.loadFromFile("smallstone.png")) {
        std::cout << "Error: Failed to load stone texture!" << std::endl;
        return -1;
    }
    sf::SoundBuffer playerDestroyedBuffer;
    if (!playerDestroyedBuffer.loadFromFile("asteroid_hit.wav")) {
        cout << "Error: Failed to load player destroyed sound!" << endl;
        return -1;
    }
    sf::Sound playerDestroyedSound;
    playerDestroyedSound.setBuffer(playerDestroyedBuffer);

    // Load stone destroyed sound
    sf::SoundBuffer stoneDestroyedBuffer;
    if (!stoneDestroyedBuffer.loadFromFile("asteroid_explosion.wav")) {
        cout << "Error: Failed to load stone destroyed sound!" << endl;
        return -1;
    }
    sf::Sound stoneDestroyedSound;
    stoneDestroyedSound.setBuffer(stoneDestroyedBuffer);

    sf::SoundBuffer firebuffer;
    if (!firebuffer.loadFromFile("player_shoot.wav")) {
        cout << "Error: Failed to load stone destroyed sound!" << endl;
        return -1;
    }
    sf::Sound firesound;
    firesound.setBuffer(firebuffer);

    sf::SoundBuffer music;
    if (!music.loadFromFile("MUSIC.wav")) {
        cout << "Error: Failed to load background sound!" << endl;
        return -1;
    }
    sf::Sound music1;
    music1.setBuffer(music);

    int maxStones = 1;
    sf::Sprite* stones = new sf::Sprite[maxStones];
    int stoneCount = 0;
    float spawnTimer = 0.0f;
    float spawnInterval = 1.f; // Time in seconds between spawning stones
    int score1 = 0;
    sf::Sprite spaceshipShadow;
    spaceshipShadow.setTexture(spaceshipTexture);
    spaceshipShadow.setScale(0.5, 0.5);
    spaceshipShadow.setColor(sf::Color(0, 0, 0, 100)); // Semi-transparent black

    bool isPaused = false; // Pause state variable
    while (window.isOpen())
    {
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape)) {
            window.close();
        }

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::P)) {
            isPaused = !isPaused; // Toggle pause state
            sf::sleep(sf::milliseconds(150)); // Add a small delay to prevent rapid toggling
        }

        if (!isPaused) {
            spaceshipSprite.setPosition(xpos, ypos);
            spaceshipShadow.setPosition(xpos + 10, ypos + 50);
            if (spaceshipSprite.getPosition().x >= 790)
                xpos = 0;
            if (spaceshipSprite.getPosition().x <= 0)
                xpos = 790;
            if (spaceshipSprite.getPosition().y >= 790)
                ypos = 0;
            if (spaceshipSprite.getPosition().y <= 0)
                ypos = 790;

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space) && !bulletActive) {
                bulletSprite.setPosition(spaceshipSprite.getPosition().x - 0, spaceshipSprite.getPosition().y - 60);
                bulletActive = true;
                if (soundOn)
                    firesound.play();
            }
            if (bulletActive) {
                bulletSprite.move(0, -5); // Move the bullet upward

                if (bulletSprite.getPosition().y < 0) {
                    bulletActive = false; // Deactivate the bullet if it goes off-screen
                }
                for (int i = 0; i < stoneCount; i++) {
                    if (isCollision(bulletSprite, stones[i])) {
                        if (stones[i].getScale().x == 1) {
                            bulletActive = false;
                            continue;
                        }
                        if (soundOn)
                            stoneDestroyedSound.play();
                        bulletActive = false;
                        score += 1;
                        score1 += 1;
                        for (int k = i; k < stoneCount - 1; ++k) {
                            stones[k] = stones[k + 1];
                        }
                        stoneCount--;
                    }
                }
            }
            /* if (score == 15) {
                 spawnInterval = 0.5f;
                 mvspeed = 0.5f;
             }
             else if (score == 30) {
                 spawnInterval = 0.2f;
             }*/
            float time = clock.getElapsedTime().asSeconds();
            clock.restart();
            timer += time;
            timer2 += time;
            scoreText.setString("SCORE : " + to_string(score));
            livesText.setString("LIVES : " + to_string(lives));
            spawnTimer += time;
            if (!showMenu) {
                if (spawnTimer >= spawnInterval) {
                    spawnTimer = 0.0f;

                    if (stoneCount + 3 >= maxStones) {
                        maxStones++;
                        sf::Sprite* newStones = new sf::Sprite[maxStones];
                        for (int i = 0; i < stoneCount; ++i) {
                            newStones[i] = stones[i];
                        }
                        delete[] stones;
                        stones = newStones;
                    }
                    if (score1 < 7) {
                        sf::Sprite stone(stoneTexture);
                        stone.setPosition(rand() % window.getSize().x - 100, 20);
                        stone.setScale(0.75, 0.75);
                        stones[stoneCount++] = stone;
                    }
                    else {
                        sf::Sprite stone(stoneTexture);
                        stone.setScale(1, 1.35);
                        stone.setPosition(rand() % window.getSize().x - 100, 30);
                        stones[stoneCount++] = stone;
                        score1 = 0;
                    }
                }
                for (int i = 0; i < stoneCount;) {
                    stones[i].move(0, mvspeed);

                    if (stones[i].getPosition().y > window.getSize().y) {
                        for (int j = i; j < stoneCount - 1; ++j) {
                            stones[j] = stones[j + 1];
                        }
                        stoneCount--;
                    }
                    else {
                        ++i;
                    }
                }
            }
            Event e;
            while (window.pollEvent(e))
            {
                if (e.type == Event::Closed) // If cross/close is clicked/pressed
                    window.close(); // close the game
                if (showMenu) {
                    if (e.type == Event::KeyPressed) {
                        if (e.key.code == Keyboard::Up) {
                            if (selectedItemIndex > 0) selectedItemIndex--;
                        }
                        else if (e.key.code == Keyboard::Down) {
                            if (selectedItemIndex < 3) selectedItemIndex++;
                        }
                        else if (e.key.code == Keyboard::Enter) {
                            switch (selectedItemIndex) {
                            case 0: // Start Game
                                showMenu = false;
                                break;
                            case 1: // High Scores
                                window.clear();
                                readHighScores();
                                for (int i = 0; i < scoreCount; ++i) {
                                    Text scoreText;
                                    scoreText.setFont(font);
                                    scoreText.setCharacterSize(30);
                                    scoreText.setFillColor(Color::White);
                                    string displayText = string(usernames[i]) + ": " + to_string(scores[i]);
                                    scoreText.setString(displayText);
                                    scoreText.setPosition(100, 100 + i * 40);
                                    window.draw(scoreText);
                                }
                                window.display();
                                sleep(sf::seconds(4));
                                showMenu = true;
                                break;
                            case 2: // Exit
                                window.close();
                                break;
                            case 3: // Sound Toggle
                                soundOn = !soundOn;
                                soundToggle.setString(soundOn ? "SOUND ON" : "SOUND OFF");
                                break;
                            }
                        }
                    }

                    if (!soundOn) {
                        music1.play();
                    }

                    startGame.setFillColor(Color::White);
                    highScores.setFillColor(Color::White);
                    soundToggle.setFillColor(Color::White);
                    exitGame.setFillColor(Color::White);

                    switch (selectedItemIndex) {
                    case 0:
                        startGame.setFillColor(Color::Red);
                        break;
                    case 1:
                        highScores.setFillColor(Color::Red);
                        break;
                    case 2:
                        exitGame.setFillColor(Color::Red);
                        break;
                    case 3:
                        soundToggle.setFillColor(Color::Red);
                        break;
                    }
                }
            }
            if (!showMenu) {
                if (Keyboard::isKeyPressed(Keyboard::Left)) // If left key is pressed
                    xpos -= 2;
                if (Keyboard::isKeyPressed(Keyboard::Right)) // If right key is pressed
                {
                    xpos += 2;
                }
                if (Keyboard::isKeyPressed(Keyboard::Up)) // If up key is pressed
                    ypos -= 2;
                if (Keyboard::isKeyPressed(Keyboard::Down)) // If down key is pressed
                    ypos += 2;


                for (int i = 0; i < stoneCount; i++) {
                    if (isCollision(spaceshipSprite, stones[i])) {
                        if (stones[i].getScale().x != 1) {
                            lives--;
                        }
                        else {
                            lives -= 3;
                        }
                        if (soundOn)
                            playerDestroyedSound.play();
                        for (int k = i; k < stoneCount - 1; ++k) {
                            stones[k] = stones[k + 1];
                        }
                        stoneCount--;
                    }
                }
                if (lives <= 0) {
                    addHighScore(username, score);
                    writeHighScores();
                    sortHighScores();
                    window.clear();
                    window.draw(background);
                    Font font1;
                    font1.loadFromFile("Deep Shadow.ttf");
                    sf::Text gameover;
                    gameover.setFont(font1);
                    gameover.setCharacterSize(70);
                    gameover.setFillColor(sf::Color::Red);
                    gameover.setPosition(790 / 2 - 300, 790 / 2 - 100);
                    gameover.setString("GAME OVER");
                    window.draw(gameover);
                    window.display();
                    sf::sleep(sf::seconds(3));
                    bool REPLAY = true;
                    lives = 3;
                    score = 0;
                    showMenu = true;
                    continue;
                }
            }

            window.clear(); // Clear the window before drawing
            if (showMenu) {
                // Draw menu items
                window.draw(startGame);
                window.draw(highScores);
                window.draw(exitGame);
                window.draw(soundToggle);
            }
            else {
                window.draw(background);
                window.draw(spaceshipSprite); // Draw the sprite
                window.draw(scoreText);
                window.draw(livesText);
                window.draw(timeText);
                window.draw(spaceshipShadow);
                if (bulletActive) {
                    window.draw(bulletSprite);
                }
                // Draw stones
                for (int i = 0; i < stoneCount; ++i) {
                    window.draw(stones[i]);
                }
            }
            window.display();
        }

        else {
            // Render Pause Screen
            sf::Text pauseMessage;
            pauseMessage.setFont(font);
            pauseMessage.setCharacterSize(50);
            pauseMessage.setFillColor(sf::Color::White);
            pauseMessage.setString("GAME PAUSED\nPRESS P TO RESUME");
            pauseMessage.setPosition(790 / 2 - 350, 790 / 2 - 100);
            window.draw(pauseMessage);
            window.display();
        }
    }
    return 0;
}