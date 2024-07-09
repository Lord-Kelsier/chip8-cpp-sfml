#include <SFML/Graphics.hpp>
#include <iostream>
#include <cstdlib>
#include "CPU.h"

bool getRandBinColor() {
    int var = rand() / (RAND_MAX / 2) - 1;
    return (bool)var;

}
void setupGraphics(sf::RectangleShape* cubes) {
    
    float cubeSize = 20.0f;
    // Set the frame rate
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
}
int main() {
    // Create the main window
    sf::RectangleShape cubes[64 * 32];
    sf::RenderWindow window(sf::VideoMode(1280, 640), "SFML Moving Cubes");
    window.setFramerateLimit(60);
    setupGraphics(cubes);
    CPU cpu;
    cpu.initialize();
    cpu.loadGame("pong");
    int iters = 0;
    sf::Color black = sf::Color::Black;
    sf::Color white = sf::Color::White;
    //time_t start, end;
    //time(&start);
    // Start the game loop
    while (window.isOpen()) {
        // Process events
        sf::Event event;
        while (window.pollEvent(event)) {
            // Close window: exit
            if (event.type == sf::Event::Closed)
                window.close();
        }
        // Emulate one cycle
        cpu.emulateCycle();
        // If the draw flag is set, update screen
        if (cpu.drawFlag || true) {
            // Clear screen
            window.clear(sf::Color::Black);
            // Draw the cubes
            int offset = 0;
            for (int byteIndex = 0; byteIndex < 64 * 32; byteIndex++) {
                if (cpu.gfx[byteIndex]) {
                    cubes[byteIndex].setFillColor(white);
                }
                else {
                    cubes[byteIndex].setFillColor(black);
                }
                window.draw(cubes[byteIndex]);
            }
        }
        // store key press state (Press and Release)
        cpu.setKeys();
        // Update the window
        window.display();
        //time(&end);
        //iters++;
        //std::cout <<"iters" << iters << "|" << end - start << std::endl;
    }
    cubes;
    return 0;
}
