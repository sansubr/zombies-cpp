#include "Player.h"
#include "ZombieArena.h"
#include <SFML/Graphics.hpp>
#include <iostream>
#include "TextureHolder.h"

int main()
{

    // Instance of TextureHolder
    TextureHolder holder;
    
    // The game will always be in one of these 4 states
    enum class State { PAUSED, LEVELING_UP, GAME_OVER, PLAYING };


    // Start with the GAME_OVER state
    State state = State::GAME_OVER;

    // Get the screen resolution and create an SFML window
    sf::Vector2u resolution = sf::VideoMode::getDesktopMode().size;
    
    // Creating an SFML window with the captured information
    sf::RenderWindow window(sf::VideoMode(resolution), "ZOMBIEE!", sf::State::Fullscreen);



    // Test text
    sf::Font font("./fonts/KOMIKAP_.ttf");
    sf::Text text(font, "Hello World!", 50);
    text.setFillColor(sf::Color::Yellow);


    

    // Creating an SFML view for the main action
    sf::View mainView(sf::FloatRect(sf::Vector2f(0,0), sf::Vector2f(resolution)));



    // Clock for timing everything
    sf::Clock clock;



    // How long has the playing state been active?
    sf::Time gameTimeTotal;




    // Where is the mouse in relation to World co-ordinates?
    sf::Vector2f mouseWorldPosition;


    // Where is the mouse in relation to screen co-ordinates?
    sf::Vector2i mouseScreenPosition;



    // Creating an instance of the player class
    Player player;




    // The boundaries of the arena
    sf::IntRect arena;




    // Creating a vertex array background
    sf::VertexArray background;

    // Loading the texture for our bg vertex array
    sf::Texture textureBackground = TextureHolder::GetTexture("./graphics/background_sheet.png");




    // Preparing the horde of Zombies
    int numZombies;
    int numZombiesAlive;
    Zombie* zombies = nullptr;



    // Debug circle
    //sf::RectangleShape rect({ 10,10 });
    //rect.setFillColor(sf::Color::Red);
    //rect.setPosition(player.getCenter());
    //rect.setFillColor(sf::Color::Yellow);



    // The main game loop
    while (window.isOpen())
    {

        // Handle input

        while (const std::optional event = window.pollEvent())
        {


            // This is for the x button to close the window
            if (event->is<sf::Event::Closed>())
            {
                window.close();
            }




            // Took this from SFML event documentation
            if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
            {
                // Pausing on Enter
                if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Enter) && state == State::PLAYING)
                {
                    state = State::PAUSED;
                }
                // Restart while paused
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Enter) && state == State::PAUSED)
                {
                    state = State::PLAYING;
                    
                    // Doing this so there is no frame jump
                    // Did this in Pong also
                    clock.restart(); 
                }
                // Start a new game while in GAME_OVER state
                else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::Enter) && state == State::GAME_OVER)
                {
                    state = State::LEVELING_UP;
                }

                if (state == State::PLAYING)
                {

                }
            } // end of if (const auto* keyPressed = event->getIf<sf::Event::KeyPressed>())
        } // end of while (const std::optional event = window.pollEvent())




        // If playing, handling WSAD input
        // Note that this needs to be outside the event loop
        // sf::Keyboard::isKeyPressed() checks the real-time state of a key, not an event. Placing it inside the KeyPressed event
        // block means it only gets checked when a keyboard event occurs, which defeats the purpose of real-time input and causes 
        // movement to be tied to discrete key press events rather than continuous holding. Therefore, it has to be outside the
        // event loop
        if (state == State::PLAYING)
        {

            // Movement up and stop
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::W))
            {
                player.moveUp();
            }
            else
            {
                player.stopMovingUp();
            }

            // Movement down and stop
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::S))
            {
                player.moveDown();
            }
            else
            {
                player.stopMovingDown();
            }

            // Movement left and stop
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::A))
            {
                player.moveLeft();
            }
            else
            {
                player.stopMovingLeft();
            }


            // Movement right and stop
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::D))
            {
                player.moveRight();
            }
            else
            {
                player.stopMovingRight();
            }

        } // end of WASD while playing


        // Handling leveling up state, don't know wtf these are so far
        if (state == State::LEVELING_UP)
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Num1))
            {
                state = State::PLAYING;
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Num2))
            {
                state = State::PLAYING;
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Num3))
            {
                state = State::PLAYING;
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Num4))
            {
                state = State::PLAYING;
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Num5))
            {
                state = State::PLAYING;
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Num6))
            {
                state = State::PLAYING;
            }

            // Handling playing state
            if (state == State::PLAYING)
            {
                // Prepare the level
                arena.size.x = 500;
                arena.size.y = 500;
                arena.position.x = 0;
                arena.position.y = 0;

                // Passing the Vertex Array by reference to the createBackground() function
                int tileSize = createBackground(background, arena);

                // Spawning the player in the middle of the arena
                player.spawn(arena, sf::Vector2u(resolution), tileSize);


                // Creating a horde of Zombies
                numZombies = 10;
                // Delete the previouslly allocated memory if exists
                delete[] zombies;
                zombies = createHorde(numZombies, arena);
                std::cout << "Horde created successfully!" << std::endl;
                numZombiesAlive = numZombies;


                // Reset clock so there isn't a frame jump
                clock.restart();
            }
        } // end of levelling up state if (state == State::LEVELING_UP)



        /* UPDATING THE FRAMES */

        // Updating the frames
        if (state == State::PLAYING)
        {
            // Update the delta time
            sf::Time dt = clock.restart();

            // Update the total game time
            gameTimeTotal += dt;


            // Make a fraction of 1 from the deltaTime
            float dtAsSeconds = dt.asSeconds();


            // Where is the mouse pointer?
            mouseScreenPosition = sf::Mouse::getPosition();
            // Convert mouse position to world
            // based co-ordinates of main view
            mouseWorldPosition = window.mapPixelToCoords(sf::Mouse::getPosition(), mainView);


            // Update the player
            player.update(dtAsSeconds, sf::Mouse::getPosition());

            // Make a note of the player's new position
            sf::Vector2f playerPosition(player.getCenter());


            // Make the view center around the player
            mainView.setCenter(player.getCenter());




            // Looping through each Zombie and updating them
            std::cout << "Starting zombie update loop" << std::endl;
            for (int i = 0; i < numZombies; ++i)
            {
                if (zombies[i].isAlive())
                {
                    zombies[i].update(dt.asSeconds(), playerPosition);
                }
            }
            std::cout << "Finished zombie update loop" << std::endl;





        } // end of updating the frames with if (state == State::PLAYING)





        /* DRAWING THE SCENE */
        // Drawing the scene

        if (state == State::PLAYING)
        {
            window.clear();
            
            // set the main view to be displayed in the window
            // And draw everything related to it.
            window.setView(mainView);



            // Drawing the background
            window.draw(background, &textureBackground);



            // Drawing Zombies
            for (int i = 0; i < numZombies; i++)
            {
                std::cout << "Drawing zombie " << i << ", alive: " << zombies[i].isAlive() << std::endl;
                window.draw(zombies[i].getSprite());
            }



            // testing
            /*
            std::cout << "View center: " << mainView.getCenter().x << ", " << mainView.getCenter().y << std::endl;
            std::cout << "View size: " << mainView.getSize().x << ", " << mainView.getSize().y << std::endl;
            std::cout << "Sprite global bounds: " << player.getSprite().getGlobalBounds().position.x << ", "
                << player.getSprite().getGlobalBounds().position.y << " | "
                << player.getSprite().getGlobalBounds().size.x << "x"
                << player.getSprite().getGlobalBounds().size.y << std::endl;
            */

            // Draw the player
            window.draw(player.getSprite());
            //window.draw(rect);

            
        }

        if (state == State::LEVELING_UP)
        {

        }

        if (state == State::PAUSED)
        {

        }

        if (state == State::GAME_OVER)
        {

        }



        // window.draw(text);

        window.display();

    } // end of while (window.isOpen()) game loop




    delete[] zombies;
    return 0;
}

