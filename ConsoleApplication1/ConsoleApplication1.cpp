#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdlib>

bool getRandBinColor() {
    int var = rand() / (RAND_MAX / 2) - 1;
    return (bool)var;

}
int main() {
    // Create the main window
    sf::RenderWindow window(sf::VideoMode(1280, 640), "SFML Moving Cubes");
    float cubeSize = 20.0f;
    // Set the frame rate
    window.setFramerateLimit(60);
    sf::RectangleShape cubes[64*32];
    if (cubes == NULL) exit(1);
    for (int i = 0; i < 64 * 32; i++) {
        // Define two white cubes
        cubes[i] = sf::RectangleShape(sf::Vector2f(cubeSize, cubeSize));
        if (getRandBinColor()) {
            cubes[i].setFillColor(sf::Color::White);
        }
        else {
            cubes[i].setFillColor(sf::Color::Black);
        }
        cubes[i].setPosition((i % 64) * cubeSize, (i / 64) * cubeSize);
    }
    


    // Start the game loop
    while (window.isOpen()) {
        // Process events
        sf::Event event;
        while (window.pollEvent(event)) {
            // Close window: exit
            if (event.type == sf::Event::Closed)
                window.close();
        }
        // Clear screen
        window.clear(sf::Color::Black);
        // Draw the cubes
        for (int i = 0; i < 64 * 32; i++) {
            window.draw(cubes[i]);
        }
        // Update the window
        window.display();
    }
    cubes;
    return 0;
}
