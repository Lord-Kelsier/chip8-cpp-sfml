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

void checkKeys(CPU* cpu) {
    sf::Keyboard::Key keys[16] = {
        sf::Keyboard::Num1,
        sf::Keyboard::Num2,
        sf::Keyboard::Num3,
        sf::Keyboard::Q,
        sf::Keyboard::W,
        sf::Keyboard::E,
        sf::Keyboard::A,
        sf::Keyboard::S,
        sf::Keyboard::D,
        sf::Keyboard::X,
        sf::Keyboard::Z,
        sf::Keyboard::C,
        sf::Keyboard::Num4,
        sf::Keyboard::R,
        sf::Keyboard::F,
        sf::Keyboard::V,

    };
    for (int i = 0; i < 16; i++) {
        if (sf::Keyboard::isKeyPressed(keys[i])) {
            cpu->key[i] = 1;
            std::cout << "Pressed: " << keys[i] << std::endl;
        }
        else {
            cpu->key[i] = 0;
        }
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
    //cpu.loadGame("GAMES/INVADERS");
    cpu.loadGame("DEBUGGAME");
    int iters = 0;
    sf::Color black = sf::Color::Black;
    sf::Color white = sf::Color::White;
    //time_t start, end;
    //time(&start);
    // Start the game loop
    int iterations = 0;
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
        if (cpu.drawFlag) {
            // Clear screen
            //window.clear(black);
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
        if (cpu.clearScreenFlag) {
            window.clear(black);
        }
        // store key press state (Press and Release)
        checkKeys(&cpu);
        // sound
        if (cpu.sound_timer != 0) {
            std::cout << "SOUND PLAYING" << std::endl;
        }
        if (iterations == 10) {
            cpu.saveDebugInfo("Debug.txt");
        }
        // Update the window
        window.display();
        iterations++;
        //time(&end);
        //iters++;
        //std::cout <<"iters" << iters << "|" << end - start << std::endl;
    }
    cubes;
    return 0;
}
