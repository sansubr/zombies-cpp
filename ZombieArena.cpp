#include <SFML/Graphics.hpp>
#include <iostream>
#include "Player.h"
#include "ZombieArena.h"
#include "TextureHolder.h"
#include "Bullet.h"
#include "Pickup.h"
#include <sstream>

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

    // Bullets
    Bullet bullets[100];
    int currentBullet = 0;
    int bulletsSpare = 24;
    int bulletsInClip = 6;
    int clipSize = 6;
    float fireRate = 5;

    // When was the fire button last pressed?
    sf::Time lastPressed;
    // R key reloads the clip - code for that is in the game loop in if PLAYING

    
    // Replace the mouse pointer with a cross hair
    window.setMouseCursorVisible(false);
    sf::Sprite spriteCrossHair(TextureHolder::GetTexture("graphics/crosshair.png"));
    spriteCrossHair.setOrigin(sf::Vector2f(25, 25));



    // Creating a couple of pickups
    Pickup healthPickup(1);
    Pickup ammoPickup(2);




    // Setting up the score
    int score = 0;
    int highScore = 0;





    // The main game loop
    while (window.isOpen())
    {


        static State lastState = State::GAME_OVER;
        if (state != lastState)
        {
            std::cout << "State changed to: " << (int)state << std::endl;
            lastState = state;
        }


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
                    // Reloading
                    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Key::R))
                    {
                        if (bulletsSpare >= clipSize)
                        {
                            // Plenty of bullets, reload!
                            bulletsInClip = clipSize;
                            bulletsSpare -= clipSize;
                        }
                        else if (bulletsSpare > 0)
                        {
                            // only a few bullets left
                            bulletsInClip = bulletsSpare;
                            bulletsSpare = 0;
                        }
                        else
                        {
                            std::cout << "Bullets are over!" << std::endl;
                            // more here soon!
                        }
                    }

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


            // Firing a bullet
            if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left))
            {
                if (gameTimeTotal.asMilliseconds() - lastPressed.asMilliseconds() > 1000 / fireRate && bulletsInClip > 0)
                {
                    bullets[currentBullet].shoot(player.getCenter().x, player.getCenter().y, mouseWorldPosition.x, mouseWorldPosition.y);
                    currentBullet++;
                    if (currentBullet > 99)
                    {
                        currentBullet = 0;
                    }
                    lastPressed = gameTimeTotal;
                    bulletsInClip--;
                }
            } // End firing a bullet


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
                std::cout << "===== INITIALIZING GAME =====" << std::endl;
                // Prepare the level
                arena.size.x = 500;
                arena.size.y = 500;
                arena.position.x = 0;
                arena.position.y = 0;

                // Passing the Vertex Array by reference to the createBackground() function
                int tileSize = createBackground(background, arena);

                // Spawning the player in the middle of the arena
                player.spawn(arena, sf::Vector2u(resolution), tileSize);


                // Configuring the pickups
                healthPickup.setArena(arena);
                ammoPickup.setArena(arena);
                std::cout << "After setArena - Health spawned: " << healthPickup.isSpawned() << std::endl;
                std::cout << "After setArena - Ammo spawned: " << ammoPickup.isSpawned() << std::endl;


                // Debug
                std::cout << "Arena: " << arena.position.x << ", " << arena.position.y
                    << " Size: " << arena.size.x << "x" << arena.size.y << std::endl;
                std::cout << "Player position: " << player.getCenter().x << ", " << player.getCenter().y << std::endl;
                std::cout << "View center: " << mainView.getCenter().x << ", " << mainView.getCenter().y << std::endl;


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

            // Set the crosshair to the mouse world location
            spriteCrossHair.setPosition(mouseWorldPosition);



            // Update the player
            player.update(dtAsSeconds, sf::Mouse::getPosition());

            // Make a note of the player's new position
            sf::Vector2f playerPosition(player.getCenter());


            // Make the view center around the player
            mainView.setCenter(player.getCenter());




            // Looping through each Zombie and updating them
            // std::cout << "Starting zombie update loop" << std::endl;
            for (int i = 0; i < numZombies; ++i)
            {
                if (zombies[i].isAlive())
                {
                    zombies[i].update(dt.asSeconds(), playerPosition);
                }
            }
            // std::cout << "Finished zombie update loop" << std::endl;

            // Update any bullets in-flight
            for (int i = 0; i < 100; i++)
            {
                if (bullets[i].isInFlight())
                {
                    bullets[i].update(dtAsSeconds);
                }
            }

            // Update the pickups
            healthPickup.update(dtAsSeconds);
            ammoPickup.update(dtAsSeconds);




            // Collision detection
            // Have any zombies been shot?

            // Looping through bullets
            for (int i = 0; i < 100; i++)
            {
                // Looping through the number of zombies
                for (int j = 0; j < numZombies; j++)
                {
                    // Check to see if the bullet is in flight and the zombie is alive
                    if (bullets[i].isInFlight() && zombies[j].isAlive())
                    {
                        // Check to see if the bullet has intersected with the Zombie
                        if (bullets[i].getPosition().findIntersection(zombies[j].getPosition()))
                        {
                            // stop the bullet
                            bullets[i].stop();

                            // Register the hit and see if it was a kill
                            if (zombies[j].hit())
                            {
                                // not just a hit but a kill too
                                score += 10;
                                if (score >= highScore)
                                {
                                    highScore = score;
                                }
                                
                                // Reducing the number of zombies
                                numZombiesAlive--;

                                // When all the zombies are dead, level up state
                                if (numZombiesAlive == 0)
                                {
                                    state = State::LEVELING_UP;
                                }
                            } // end of if (zombies[j].hit())
                        } // end of if (bullets[i].getPosition().findIntersection(zombies[j].getPosition()))
                    } // end of if (bullets[i].isInFlight() && zombies[j].isAlive())
                } // end of for (int j = 0; j < numZombies; j++)
            } // end of for (int i = 0; i < 100; i++)




            // Collision detection with the player
            for (int i = 0; i < numZombies; i++)
            {
                if (player.getPosition().findIntersection(zombies[i].getPosition()) && zombies[i].isAlive())
                {
                    if (player.hit(gameTimeTotal))
                    {
                        // more stuff to come here
                    }

                    if (player.getHealth() <= 0)
                    {
                       state = State::GAME_OVER;
                    }
                }
            } // End collision detection with player


            /* Collision detection with the pickups */ 
            /*          ********************        */
            // Detecting health pickup colission
            if (player.getPosition().findIntersection(healthPickup.getPosition()) && healthPickup.isSpawned())
            {
                player.increaseHealthLevel(healthPickup.gotIt());
            }

            // Detecting ammo pickup collision
            if (player.getPosition().findIntersection(ammoPickup.getPosition()) && ammoPickup.isSpawned())
            {
                bulletsSpare += ammoPickup.gotIt();
            }

    






            // Add these lines:
            if (healthPickup.isSpawned())
            {
                std::cout << "Health pickup position: " << healthPickup.getSprite().getPosition().x
                    << ", " << healthPickup.getSprite().getPosition().y << std::endl;
            }
            if (ammoPickup.isSpawned())
            {
                std::cout << "Ammo pickup position: " << ammoPickup.getSprite().getPosition().x
                    << ", " << ammoPickup.getSprite().getPosition().y << std::endl;
            }
            std::cout << "Player position: " << player.getCenter().x << ", " << player.getCenter().y << std::endl;
            std::cout << "View center: " << mainView.getCenter().x << ", " << mainView.getCenter().y << std::endl;


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
                // std::cout << "Drawing zombie " << i << ", alive: " << zombies[i].isAlive() << std::endl;
                window.draw(zombies[i].getSprite());
            }


            for (int i = 0; i < 100; i++)
            {
                if (bullets[i].isInFlight())
                {
                    window.draw(bullets[i].getShape());
                }
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



            // Draw the pickups if currently spawned
            if (ammoPickup.isSpawned())
            {
                window.draw(ammoPickup.getSprite());
            }

            if (healthPickup.isSpawned())
            {
                window.draw(healthPickup.getSprite());
            }



            // Draw crosshair
            window.draw(spriteCrossHair);

            
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

