#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <SFML/System.hpp>
#include <iostream>

#include <fstream>
#include <sstream>
#include <string>

// Function to display difficulty menu and get user choice
int getDifficultyChoice(sf::RenderWindow& window, sf::Font& font) {
    sf::Text menuText("Select Difficulty:\n1. Easy\n2. Medium\n3. Hard", font, 30);
    menuText.setPosition(100, 200);

    window.clear();
    window.draw(menuText);
    window.display();

    int choice = 0;
    sf::Event event{};
    while (window.waitEvent(event)) {
        if (event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Num1) {
                choice = 1;
                break;
            } else if (event.key.code == sf::Keyboard::Num2) {
                choice = 2;
                break;
            } else if (event.key.code == sf::Keyboard::Num3) {
                choice = 3;
                break;
            }
        }
    }

    return choice;
}

void setDifficulty(int choice, float& paddleSpeed, sf::Vector2f& ballVelocity) {
    switch (choice) {
    case 1: // Easy
        paddleSpeed = 5.0f;
        ballVelocity = sf::Vector2f(5.0f, 5.0f);
        break;
    case 2: // Medium
        paddleSpeed = 7.0f;
        ballVelocity = sf::Vector2f(7.0f, 7.0f);
        break;
    case 3: // Hard
        paddleSpeed = 9.0f;
        ballVelocity = sf::Vector2f(10.0f, 10.0f);
        break;
    default:
        paddleSpeed = 7.0f; // Default to Medium if invalid choice
        ballVelocity = sf::Vector2f(5.0f, 5.0f);
        break;
    }
}

// Function to setup the window
void setupWindow(sf::RenderWindow & window) {
    window.setFramerateLimit(60); // 60 Frame / Sec
}

// Function to load textures
void loadTextures(sf::Texture& paddleTexture, sf::Texture& ballTexture, sf::Texture& backgroundTexture) {
    paddleTexture.loadFromFile("resources/MainPaddle.png");
    ballTexture.loadFromFile("resources/ball.png");
    backgroundTexture.loadFromFile("resources/backdrop00.png");
}

// Function to load audio
void loadAudio(sf::Sound& hitSound, sf::SoundBuffer& hitBuffer) {
    hitBuffer.loadFromFile("resources/hit.wav");
    hitSound.setBuffer(hitBuffer);
}

// Function to setup sprites
void setupSprites(sf::Sprite& paddle1Sprite, sf::Sprite& paddle2Sprite, sf::Sprite& ballSprite, sf::Sprite& backgroundSprite, sf::Texture& paddleTexture, sf::Texture& ballTexture, sf::Texture& backgroundTexture, float windowHeight, float windowWidth) {
    // Background
    backgroundSprite.setTexture(backgroundTexture);

    // Paddles
    float Scaling_Factor = 0.3f;
    paddle1Sprite.setTexture(paddleTexture);
    paddle1Sprite.setScale(Scaling_Factor, Scaling_Factor);
    paddle1Sprite.setPosition(50, (windowHeight - paddle1Sprite.getGlobalBounds().height) / 2 + 100);

    paddle2Sprite.setTexture(paddleTexture);
    paddle2Sprite.setScale(Scaling_Factor, Scaling_Factor);
    paddle2Sprite.setPosition(windowWidth - 50 - paddle2Sprite.getGlobalBounds().width, (windowHeight - paddle2Sprite.getGlobalBounds().height) / 2 + 100);

    // Ball
    ballSprite.setTexture(ballTexture);
    ballSprite.setScale(Scaling_Factor, Scaling_Factor);
    ballSprite.setPosition(windowWidth / 2, windowHeight / 2 + 70);
}


// Function to handle events
void handleEvents(sf::RenderWindow& window, bool& inMenu, bool& playing, bool& showHighScores, bool& ballPaused, bool& gamePaused, sf::Clock& gameClock, sf::Time& pausedTime) {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
            window.close();
        }

        if (inMenu && event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Num1) {
                playing = true;
                inMenu = false;
                gameClock.restart(); // Starts the clock from 0
            } else if (event.key.code == sf::Keyboard::Num2) {
                showHighScores = true;
                inMenu = false;
            } else if (event.key.code == sf::Keyboard::Num3) {
                window.close();
            }
        }

        if (playing && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
            ballPaused = !ballPaused;
        }

        if (playing && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::P) {
            gamePaused = !gamePaused;
            if (gamePaused) {
                pausedTime = gameClock.getElapsedTime();
            } else {
                gameClock.restart();
            }
        }
    }
}



// Function to update paddle positions
void updatePaddles(sf::Sprite& paddle1Sprite, sf::Sprite& paddle2Sprite, float paddleSpeed, float windowHeight) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && paddle1Sprite.getPosition().y > 105) {
        paddle1Sprite.move(0, -paddleSpeed);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && paddle1Sprite.getPosition().y + paddle1Sprite.getGlobalBounds().height < windowHeight) {
        paddle1Sprite.move(0, paddleSpeed);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && paddle2Sprite.getPosition().y > 105) {
        paddle2Sprite.move(0, -paddleSpeed);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && paddle2Sprite.getPosition().y + paddle2Sprite.getGlobalBounds().height < windowHeight) {
        paddle2Sprite.move(0, paddleSpeed);
    }
}

// Function to update ball position and handle collisions
void updateBall(sf::Sprite& ballSprite, sf::Vector2f& ballVelocity, sf::Sprite& paddle1Sprite, sf::Sprite& paddle2Sprite, float windowWidth, float windowHeight, int& score1, int& score2, sf::Sound& hitSound, bool ballPaused, bool gamePaused) {
    if (!ballPaused && !gamePaused) {
        ballSprite.move(ballVelocity);

        if (ballSprite.getPosition().y <= 105 || ballSprite.getPosition().y + ballSprite.getGlobalBounds().height >= windowHeight) {
            ballVelocity.y = -ballVelocity.y;
            hitSound.play();
        }

        if (ballSprite.getGlobalBounds().intersects(paddle1Sprite.getGlobalBounds()) ||
            ballSprite.getGlobalBounds().intersects(paddle2Sprite.getGlobalBounds())) {
            ballVelocity.x = -ballVelocity.x;
            hitSound.play();
            }

        if (ballSprite.getPosition().x <= 0) {
            score2++;
            ballSprite.setPosition(windowWidth / 2, windowHeight / 2 + 50);
        } else if (ballSprite.getPosition().x + ballSprite.getGlobalBounds().width >= windowWidth) {
            score1++;
            ballSprite.setPosition(windowWidth / 2, windowHeight / 2 + 50);
        }
    }
}


// Function to render game objects
void render(sf::RenderWindow& window, sf::Sprite& backgroundSprite, sf::Sprite& paddle1Sprite, sf::Sprite& paddle2Sprite, sf::Sprite& ballSprite, sf::Text& scoreText, sf::RectangleShape& boundary, sf::Text& timeText, bool gamePaused, sf::Font & font) {
    window.clear();
    window.draw(backgroundSprite);
    window.draw(boundary);
    window.draw(paddle1Sprite);
    window.draw(paddle2Sprite);
    window.draw(ballSprite);
    window.draw(scoreText);
    window.draw(timeText);

    if (gamePaused) {
        sf::Text pauseText("Game Paused", font, 50);
        pauseText.setPosition(window.getSize().x / 2 - 150, window.getSize().y / 2 - 50);
        window.draw(pauseText);
    }

    window.display();
}



// Function to get player names
void getPlayerNames(std::string& player1, std::string& player2, sf::RenderWindow& window, sf::Font& font) {
    sf::Text inputText1("Player 1 Name:", font, 30);
    inputText1.setPosition(50, 200);
    sf::Text inputText2("Player 2 Name:", font, 30);
    inputText2.setPosition(50, 300);

    sf::Text player1Name("", font, 30);
    player1Name.setPosition(300, 200);
    sf::Text player2Name("", font, 30);
    player2Name.setPosition(300, 300);

    std::string input1, input2;
    bool input1Complete = false, input2Complete = false;

    while (window.isOpen() && !(input1Complete && input2Complete)) {
        sf::Event event{};
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::TextEntered) {
                if (event.text.unicode < 128) { // Only ASCII
                    if (!input1Complete) {
                        if (event.text.unicode == '\n' || event.text.unicode == '\r') {
                            input1Complete = true;
                        } else {
                            input1 += static_cast<char>(event.text.unicode);
                            player1Name.setString(input1);
                        }
                    } else if (!input2Complete) {
                        if (event.text.unicode == '\n' || event.text.unicode == '\r') {
                            input2Complete = true;
                        } else {
                            input2 += static_cast<char>(event.text.unicode);
                            player2Name.setString(input2);
                        }
                    }
                }
            }
        }

        window.clear();
        window.draw(inputText1);
        window.draw(player1Name);
        window.draw(inputText2);
        window.draw(player2Name);
        window.display();
    }

    player1 = input1;
    player2 = input2;
}

// Function to save high scores to a file
void saveHighScore(const std::string& player, int score) {
    std::ofstream file("highscores.txt", std::ios::app);
    if (file.is_open()) {
        file << player << ": " << score << "\n";
        file.close();
    }
}

// Function to update score text
void updateScoreText(sf::Text& scoreText, int score1, int score2, const std::string& player1, const std::string& player2) {
    std::stringstream scoreStream;
    scoreStream << player1 << " " << score1 << " : " << score2 << " " << player2;
    scoreText.setString(scoreStream.str());
}

// Function to display the main menu
void displayMenu(sf::RenderWindow& window, sf::Font& font) {
    sf::Text menuText("1. Play\n2. High Scores\n3. Quit", font, 50);
    menuText.setPosition(100, 200);

    window.clear();
    window.draw(menuText);
    window.display();
}

// Function to display high scores
void displayHighScores(sf::RenderWindow& window, sf::Font& font) {
    sf::Text highScoreText("High Scores", font, 50);
    highScoreText.setPosition(100, 50);

    std::ifstream file("highscores.txt");
    std::string line;
    std::string topScore;
    int maxScore = 0;

    while (std::getline(file, line)) {
        std::stringstream iss(line);
        std::string playerName;
        int score;
        iss >> playerName >> score;

        if (score > maxScore) {
            maxScore = score;
            topScore = line;
        }
    }

    sf::Text scoreText(topScore, font, 40);
    scoreText.setPosition(100, 150);
    window.clear();
    window.draw(highScoreText);
    window.draw(scoreText);
    window.display();

    // If any key is prssed, it closes the high scores screen
    sf::Event event{};
    while (window.waitEvent(event)) {
        if (event.type == sf::Event::KeyPressed) {
            break;
        }
    }
}

int main() {
    bool gamePaused = false;
    bool ballPaused = true; // Ball starts in a paused state

    const float WindowHeight = 768, WindowWidth = 1000;


    sf::RenderWindow window(sf::VideoMode(WindowWidth, WindowHeight), "Pong", sf::Style::Titlebar | sf::Style::Close);
    setupWindow(window);


    sf::Texture PaddleTexture, BallTexture, BackgroundTexture;
    loadTextures(PaddleTexture, BallTexture, BackgroundTexture);

    sf::SoundBuffer hitBuffer;
    sf::Sound hitSound;
    loadAudio(hitSound, hitBuffer);

    sf::Font font;
    font.loadFromFile("resources/trebuc.ttf");

    std::string player1, player2;
    getPlayerNames(player1, player2, window, font);

    int difficultyChoice = getDifficultyChoice(window, font);

    // Set difficulty parameters
    float paddleSpeed;
    sf::Vector2f ballVelocity;
    setDifficulty(difficultyChoice, paddleSpeed, ballVelocity);

    sf::Sprite paddle1Sprite, paddle2Sprite, ballSprite, backgroundSprite;
    setupSprites(paddle1Sprite, paddle2Sprite, ballSprite, backgroundSprite, PaddleTexture, BallTexture, BackgroundTexture, WindowHeight, WindowWidth);

    sf::Text scoreText("", font, 30);
    scoreText.setPosition(WindowWidth / 2 - 100, 20);

    int score1 = 0, score2 = 0;

    sf::RectangleShape boundary(sf::Vector2f(WindowWidth, 100)); // The upper horizontal Boundary
    boundary.setFillColor(sf::Color::Black);

    //State
    bool inMenu = true;
    bool playing = false;
    bool showHighScores = false;

    // clock to measure the elapsed time
    sf::Clock gameClock;
    sf::Time gameTime = sf::seconds(60); // game duration = 60 seconds
    sf::Time pausedTime = sf::Time::Zero;

    sf::Text timeText("", font, 30);
    timeText.setPosition(WindowWidth / 2 - 50, 60);

    while (window.isOpen()) {
        if (inMenu) {
            displayMenu(window, font);
            handleEvents(window, inMenu, playing, showHighScores, ballPaused, gamePaused, gameClock, pausedTime);
            if (playing && !gamePaused) {
                gameClock.restart(); // Ensures that the clock starts only when the game has started and WE ARE NO MORE IN THE MENU
            }
        } else if (playing) {
            handleEvents(window, inMenu, playing, showHighScores, ballPaused, gamePaused, gameClock, pausedTime);
            updatePaddles(paddle1Sprite, paddle2Sprite, paddleSpeed, WindowHeight);
            updateBall(ballSprite, ballVelocity, paddle1Sprite, paddle2Sprite, WindowWidth, WindowHeight, score1, score2, hitSound, ballPaused, gamePaused);
            updateScoreText(scoreText, score1, score2, player1, player2);

            // time display
            sf::Time elapsed;
            if (gamePaused) {
                elapsed = pausedTime;
            } else {
                elapsed = pausedTime + gameClock.getElapsedTime();
            }
            std::stringstream timeStream;
            timeStream << "Time: " << static_cast<int>(elapsed.asSeconds());
            timeText.setString(timeStream.str());

            render(window, backgroundSprite, paddle1Sprite, paddle2Sprite, ballSprite, scoreText, boundary, timeText, gamePaused, font);

            // Checks if one minute has passed
            if (elapsed >= gameTime) {
                playing = false;
                inMenu = true;

                // Determines winner
                std::string winner;
                if (score1 > score2) {
                    winner = player1 + " wins!";
            } else if (score2 > score1) {
                winner = player2 + " wins!";
            } else {
                winner = "It's a tie!";
            }

            sf::Text winnerText(winner, font, 50);
            winnerText.setPosition(WindowWidth / 2 - 150, WindowHeight / 2);
            window.clear();
            window.draw(winnerText);
            window.display();

            sf::sleep(sf::seconds(3)); // Show result for 3 seconds
            saveHighScore(player1, score1);
            saveHighScore(player2, score2);
        }
    } else if (showHighScores) {
        displayHighScores(window, font);
        inMenu = true;
        showHighScores = false;
    }
}


    return 0;
}