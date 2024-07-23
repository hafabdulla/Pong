#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
/////////        C++ Standard I/O        /////
#include <iostream>
/////////        ////////////////       /////
///

///
/////////        File Handling        /////
#include <fstream>
#include <sstream>
/////////        //////////////       /////
#include <string>

// Function to setup the window
void setupWindow(sf::RenderWindow& window) {
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

    // BakGround
    backgroundSprite.setTexture(backgroundTexture);

    // Paddles
    float Scaling_Factor = 0.3f;
    paddle1Sprite.setTexture(paddleTexture);
    paddle1Sprite.setScale(Scaling_Factor, Scaling_Factor);
    paddle1Sprite.setPosition(50, (windowHeight - paddle1Sprite.getGlobalBounds().height) / 2 + 50);

    paddle2Sprite.setTexture(paddleTexture);
    paddle2Sprite.setScale(Scaling_Factor, Scaling_Factor);
    paddle2Sprite.setPosition(windowWidth - 50 - paddle2Sprite.getGlobalBounds().width, (windowHeight - paddle2Sprite.getGlobalBounds().height) / 2 + 50);


    // Ball
    ballSprite.setTexture(ballTexture);
    ballSprite.setScale(Scaling_Factor, Scaling_Factor);
    ballSprite.setPosition(windowWidth / 2, windowHeight / 2 + 50);
}

// Function to handle events
void handleEvents(sf::RenderWindow& window, bool& inMenu, bool& playing, bool& showHighScores, bool& ballPaused) {
    sf::Event event;
    while (window.pollEvent(event)) {
        if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)) {
            window.close(); // Close the Window
        }

        // Game States
        if (inMenu && event.type == sf::Event::KeyPressed) {
            if (event.key.code == sf::Keyboard::Num1) {
                playing = true;
                inMenu = false;
            } else if (event.key.code == sf::Keyboard::Num2) {
                showHighScores = true;
                inMenu = false;
            } else if (event.key.code == sf::Keyboard::Num3) {
                window.close();
            }
        }

        // Pause/Resume Ball
        if (playing && event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Space) {
            ballPaused = !ballPaused;
        }
    }
}


// Function to update paddle positions
void updatePaddles(sf::Sprite& paddle1Sprite, sf::Sprite& paddle2Sprite, float paddleSpeed, float windowHeight) {
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && paddle1Sprite.getPosition().y > 55) {
        paddle1Sprite.move(0, -paddleSpeed);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && paddle1Sprite.getPosition().y + paddle1Sprite.getGlobalBounds().height < windowHeight) {
        paddle1Sprite.move(0, paddleSpeed);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && paddle2Sprite.getPosition().y > 55) {
        paddle2Sprite.move(0, -paddleSpeed);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && paddle2Sprite.getPosition().y + paddle2Sprite.getGlobalBounds().height < windowHeight) {
        paddle2Sprite.move(0, paddleSpeed);
    }
}

// Function to update ball position and handle collisions
void updateBall(sf::Sprite& ballSprite, sf::Vector2f& ballVelocity, sf::Sprite& paddle1Sprite, sf::Sprite& paddle2Sprite, float windowWidth, float windowHeight, int& score1, int& score2, sf::Sound& hitSound, bool ballPaused) {
    if (!ballPaused) {
        ballSprite.move(ballVelocity);

        if (ballSprite.getPosition().y <= 50 || ballSprite.getPosition().y + ballSprite.getGlobalBounds().height >= windowHeight) {
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
void render(sf::RenderWindow& window, sf::Sprite& backgroundSprite, sf::Sprite& paddle1Sprite, sf::Sprite& paddle2Sprite,
            sf::Sprite& ballSprite, sf::Text& scoreText, sf::RectangleShape& boundary) {
    window.clear();
    window.draw(backgroundSprite);
    window.draw(boundary);
    window.draw(paddle1Sprite);
    window.draw(paddle2Sprite);
    window.draw(ballSprite);
    window.draw(scoreText);
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
        sf::Event event;
        while (window.pollEvent(event)) {
            if (event.type == sf::Event::Closed) {
                window.close();
            }
            if (event.type == sf::Event::TextEntered) {
                if (event.text.unicode < 128) {
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
    std::ostringstream scoreStream;
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
        std::istringstream iss(line);
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

    // Wait for any key to close the high scores screen
    sf::Event event;
    while (window.waitEvent(event)) {
        if (event.type == sf::Event::KeyPressed) {
            break;
        }
    }
}

int main()
{
    bool ballPaused = true; // Ball starts in a paused state

    const float PaddleSpeed = 7.0f;
    const float WindowHeight = 768, WindowWidth = 1000;
    sf::Vector2f BallVelocity(5.0f, 5.0f);
    sf::RenderWindow window(sf::VideoMode(WindowWidth, WindowHeight), "Pong", sf::Style::Titlebar | sf::Style::Close);
    setupWindow(window);

    sf::Texture PaddleTexture, BallTexture, BackgroundTexture;
    loadTextures(PaddleTexture, BallTexture, BackgroundTexture);

    sf::SoundBuffer hitBuffer;
    sf::Sound hitSound;
    loadAudio(hitSound, hitBuffer);

    sf::Font font;
    font.loadFromFile("resources/arial.ttf");

    std::string player1, player2;
    getPlayerNames(player1, player2, window, font);

    sf::Sprite paddle1Sprite, paddle2Sprite, ballSprite, backgroundSprite;
    setupSprites(paddle1Sprite, paddle2Sprite, ballSprite, backgroundSprite, PaddleTexture, BallTexture, BackgroundTexture, WindowHeight, WindowWidth);

    sf::Text scoreText("", font, 30);
    scoreText.setPosition(WindowWidth / 2 - 100, 20);
    int score1 = 0, score2 = 0;

    sf::RectangleShape boundary(sf::Vector2f(WindowWidth, 50));
    boundary.setFillColor(sf::Color::Black);

    bool inMenu = true;
    bool playing = false;
    bool showHighScores = false;

    while (window.isOpen()) {
        if (inMenu) {
            displayMenu(window, font);
            handleEvents(window, inMenu, playing, showHighScores, ballPaused);
        } else if (playing) {
            handleEvents(window, inMenu, playing, showHighScores, ballPaused);
            updatePaddles(paddle1Sprite, paddle2Sprite, PaddleSpeed, WindowHeight);
            updateBall(ballSprite, BallVelocity, paddle1Sprite, paddle2Sprite, WindowWidth, WindowHeight, score1, score2, hitSound, ballPaused);
            updateScoreText(scoreText, score1, score2, player1, player2);
            render(window, backgroundSprite, paddle1Sprite, paddle2Sprite, ballSprite, scoreText, boundary);
        } else if (showHighScores) {
            displayHighScores(window, font);
            inMenu = true;
            showHighScores = false;
        }
    }


    saveHighScore(player1, score1);
    saveHighScore(player2, score2);

    return 0;
}
