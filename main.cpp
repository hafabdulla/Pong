#include <SFML/Graphics.hpp>
#include <iostream>
#include <SFML/Audio.hpp>
#include <sstream>

void setupWindow(sf::RenderWindow& window)
{
    window.setFramerateLimit(60);
    window.setKeyRepeatEnabled(false);
}

void loadTextures(sf::Texture& paddleTexture, sf::Texture& ballTexture, sf::Texture& backgroundTexture)
{
    paddleTexture.loadFromFile("resources/pad.png");
    ballTexture.loadFromFile("resources/ball.png");
    backgroundTexture.loadFromFile("resources/background.png");
}

void loadAudio(sf::Sound& hitSound, sf::SoundBuffer& hitBuffer)
{
    hitBuffer.loadFromFile("resources/hit.wav");
    hitSound.setBuffer(hitBuffer);
}

void setupSprites(sf::Sprite& paddle1Sprite, sf::Sprite& paddle2Sprite, sf::Sprite& ballSprite, sf::Sprite& backgroundSprite,
                  sf::Texture& paddleTexture, sf::Texture& ballTexture, sf::Texture& backgroundTexture, float windowHeight, float windowWidth)
{
    backgroundSprite.setTexture(backgroundTexture);

    paddle1Sprite.setTexture(paddleTexture);
    paddle1Sprite.setPosition(50, (windowHeight - paddle1Sprite.getGlobalBounds().height) / 2);

    paddle2Sprite.setTexture(paddleTexture);
    paddle2Sprite.setPosition(windowWidth - 50 - paddle2Sprite.getGlobalBounds().width, (windowHeight - paddle2Sprite.getGlobalBounds().height) / 2);

    ballSprite.setTexture(ballTexture);
    ballSprite.setPosition(windowWidth / 2, windowHeight / 2);
}

void handleEvents(sf::RenderWindow& window)
{
    sf::Event event;
    while (window.pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            window.close();
        }
        if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape)
        {
            window.close();
        }
    }
}

void updatePaddles(sf::Sprite& paddle1Sprite, sf::Sprite& paddle2Sprite, float paddleSpeed, float windowHeight)
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W) && paddle1Sprite.getPosition().y > 0)
    {
        paddle1Sprite.move(0, -paddleSpeed);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S) && paddle1Sprite.getPosition().y + paddle1Sprite.getGlobalBounds().height < windowHeight)
    {
        paddle1Sprite.move(0, paddleSpeed);
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up) && paddle2Sprite.getPosition().y > 0)
    {
        paddle2Sprite.move(0, -paddleSpeed);
    }
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down) && paddle2Sprite.getPosition().y + paddle2Sprite.getGlobalBounds().height < windowHeight)
    {
        paddle2Sprite.move(0, paddleSpeed);
    }
}

void updateBall(sf::Sprite& ballSprite, sf::Vector2f& ballVelocity, sf::Sprite& paddle1Sprite, sf::Sprite& paddle2Sprite,
                float windowWidth, float windowHeight, int& score1, int& score2, sf::Sound& hitSound)
{
    ballSprite.move(ballVelocity);

    // Bounce off top and bottom
    if (ballSprite.getPosition().y <= 0 || ballSprite.getPosition().y + ballSprite.getGlobalBounds().height >= windowHeight)
    {
        ballVelocity.y = -ballVelocity.y;
    }

    // Bounce off paddles
    if (ballSprite.getGlobalBounds().intersects(paddle1Sprite.getGlobalBounds()) ||
        ballSprite.getGlobalBounds().intersects(paddle2Sprite.getGlobalBounds()))
    {
        ballVelocity.x = -ballVelocity.x;
        hitSound.play();
    }

    // Check for scoring
    if (ballSprite.getPosition().x <= 0)
    {
        score2++;
        ballSprite.setPosition(windowWidth / 2, windowHeight / 2);
    }
    else if (ballSprite.getPosition().x + ballSprite.getGlobalBounds().width >= windowWidth)
    {
        score1++;
        ballSprite.setPosition(windowWidth / 2, windowHeight / 2);
    }
}

void updateScoreText(sf::Text& scoreText, int score1, int score2, sf::Font& font, float windowWidth)
{
    std::ostringstream ss;
    ss << "Score " << score1 << " : " << score2;
    scoreText.setString(ss.str());
    scoreText.setFont(font);
    scoreText.setCharacterSize(30);
    scoreText.setFillColor(sf::Color::White);
    scoreText.setPosition(windowWidth / 2 - scoreText.getGlobalBounds().width / 2, 10);
}

void render(sf::RenderWindow& window, sf::Sprite& backgroundSprite, sf::Sprite& paddle1Sprite, sf::Sprite& paddle2Sprite, sf::Sprite& ballSprite, sf::Text& scoreText)
{
    window.clear();

    window.draw(backgroundSprite);
    window.draw(paddle1Sprite);
    window.draw(paddle2Sprite);
    window.draw(ballSprite);
    window.draw(scoreText);

    window.display();
}

int main()
{
    const float PaddleSpeed = 5.0f;
    const float WindowHeight = 900, WindowWidth = 700;
    sf::Vector2f BallVelocity(4.0f, 4.0f);

    sf::RenderWindow window(sf::VideoMode(WindowWidth, WindowHeight), "Pong", sf::Style::Titlebar | sf::Style::Close);
    setupWindow(window);

    sf::Texture PaddleTexture, BallTexture, BackgroundTexture;
    loadTextures(PaddleTexture, BallTexture, BackgroundTexture);

    sf::SoundBuffer hitBuffer;
    sf::Sound hitSound;
    loadAudio(hitSound, hitBuffer);

    sf::Sprite paddle1Sprite, paddle2Sprite, ballSprite, backgroundSprite;
    setupSprites(paddle1Sprite, paddle2Sprite, ballSprite, backgroundSprite, PaddleTexture, BallTexture, BackgroundTexture, WindowHeight, WindowWidth);

    sf::Font font;
    font.loadFromFile("resources/arial.ttf");

    sf::Text scoreText;
    int score1 = 0, score2 = 0;

    while (window.isOpen())
    {
        handleEvents(window);
        updatePaddles(paddle1Sprite, paddle2Sprite, PaddleSpeed, WindowHeight);
        updateBall(ballSprite, BallVelocity, paddle1Sprite, paddle2Sprite, WindowWidth, WindowHeight, score1, score2, hitSound);
        updateScoreText(scoreText, score1, score2, font, WindowWidth);
        render(window, backgroundSprite, paddle1Sprite, paddle2Sprite, ballSprite, scoreText);
    }

    return 0;
}
