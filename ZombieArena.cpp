#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <iostream>
#include "Player.h"
#include "ZombieArena.h"
#include "TextureHolder.h"
#include "Bullet.h"
#include "Pickup.h"
#include <sstream>
#include <fstream>

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
    // sf::Text text(font, "Hello World!", 50);
    // text.setFillColor(sf::Color::Yellow);


    

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
    int bulletsSpare = 48;
    int bulletsInClip = 12;
    int clipSize = 12;
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



    // For the home/game over screen
    sf::Sprite spriteGameOver(TextureHolder::GetTexture("graphics/background.png"));
    spriteGameOver.setPosition(sf::Vector2f(0, 0));

    // Create a view for the HUD
    sf::View hudView(sf::FloatRect(sf::Vector2f(0, 0), sf::Vector2f(1920, 1080)));

    // Create a sprite for the ammo icon
    sf::Sprite spriteAmmoIcon(TextureHolder::GetTexture("graphics/ammo_icon.png"));
    spriteAmmoIcon.setPosition(sf::Vector2f(20, 980));


    // Setting up the text
    // Font is already declared on top

    // This is when paused
    sf::Text textPaused(font, "Press Enter \nto Continue", 50);
    textPaused.setFillColor(sf::Color::Yellow);
    textPaused.setPosition(sf::Vector2f(400, 400));


    // This is when Game Over
    sf::Text textGameOver(font, "Press Enter to play", 90);
    textGameOver.setFillColor(sf::Color::White);
    textGameOver.setPosition(sf::Vector2f(300, 600));
    

    // This is when Leveling up
    sf::Text textLevelUp(font, "", 80);
    textLevelUp.setFillColor(sf::Color::Yellow);
    textLevelUp.setPosition(sf::Vector2f(150, 250));
    std::stringstream levelUpStream;
    levelUpStream << "1 - Increased rate of fire" <<
        "\n2 - Increased clip size" <<
        "\n3 - Increased max health" <<
        "\n4 - Increased run speed" <<
        "\n5 - More and better health pickups" <<
        "\n6 - More and better ammo pickups";
    textLevelUp.setString(levelUpStream.str());

    // Ammo text
    sf::Text textAmmo(font, "", 55);
    textGameOver.setFillColor(sf::Color::White);
    textPaused.setPosition(sf::Vector2f(200, 980));
    
    // Score text
    sf::Text textScore(font, "", 55);
    textScore.setFillColor(sf::Color::White);
    textScore.setPosition(sf::Vector2f(20, 0));

    // High-score text
    std::ifstream inputFile("gamedata/scores.txt");
    if (inputFile.is_open())
    {
        // Reads the data
        inputFile >> highScore;
        inputFile.close();
    }
    sf::Text textHighScore(font, "", 55);
    textHighScore.setFillColor(sf::Color::White);
    textHighScore.setPosition(sf::Vector2f(1400, 0));
    std::stringstream s;
    s << "High score: " << highScore;
    textHighScore.setString(s.str());

    // Zombies remaining
    sf::Text textZombiesRemaining(font, "Zombies: 100", 55);
    textZombiesRemaining.setFillColor(sf::Color::White);
    textZombiesRemaining.setPosition(sf::Vector2f(1500, 980));


    // Wave number
    int wave = 0;
    sf::Text textWaveNumber(font, "Wave: 0", 55);
    textWaveNumber.setFillColor(sf::Color::White);
    textWaveNumber.setPosition(sf::Vector2f(1250, 980));


    // Health bar
    sf::RectangleShape healthBar;
    healthBar.setFillColor(sf::Color::Red);
    healthBar.setPosition(sf::Vector2f(450, 980));
    

    // When did we last update the hud?
    int framesSinceLastUpdate = 0;

    // How often (in frames) should we update the HUD
    int fpsMeasurementFrameInterval = 1000;


    // setting up bg music
    sf::Music bgMusic("sound/bg.mp3");


    // Setting up the sound
    sf::SoundBuffer soundBufferHit("sound/hit.wav");
    sf::Sound soundHit(soundBufferHit);

    // Preparing the splat sound
    sf::SoundBuffer soundBufferSplat("sound/splat.wav");
    sf::Sound soundSplat(soundBufferSplat);

    // Preparing the shoot sound
    sf::SoundBuffer soundBufferShoot("sound/shoot.wav");
    sf::Sound soundShoot(soundBufferShoot);

    // Preparing the reload sound
    sf::SoundBuffer soundBufferReload("sound/reload.wav");
    sf::Sound soundReload(soundBufferReload);
    
    sf::SoundBuffer soundBufferReloadFailed("sound/reload_failed.wav");
    sf::Sound soundReloadFailed(soundBufferReloadFailed);


    // Prepare the power up sound
    sf::SoundBuffer soundBufferPowerup("sound/powerup.wav");
    sf::Sound soundPowerup(soundBufferPowerup);

    // Prepare the pickup sound
    sf::SoundBuffer soundBufferPickup("sound/pickup.wav");
    sf::Sound soundPickup(soundBufferPickup);
    

    






    // The main game loop
    while (window.isOpen())
    {

        


        static State lastState = State::GAME_OVER;
        if (state != lastState)
        {
            // std::cout << "State changed to: " << (int)state << std::endl;
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
                    wave = 0;
                    score = 0;

                    // Prepare the gun and ammo for the next game
                    currentBullet = 0;
                    bulletsSpare = 24;
                    bulletsInClip = 6;
                    clipSize = 6;
                    fireRate = 5;

                    // reset the player's stats
                    player.resetPlayerStats();
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
                            soundReload.play();
                        }
                        else if (bulletsSpare > 0)
                        {
                            // only a few bullets left
                            bulletsInClip = bulletsSpare;
                            bulletsSpare = 0;
                            soundReload.play();
                        }
                        else
                        {
                            soundReloadFailed.play();
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
                    soundShoot.play();
                    bulletsInClip--;
                }
            } // End firing a bullet


        } // end of WASD while playing


        // Handling leveling up state, don't know wtf these are so far
        if (state == State::LEVELING_UP)
        {
            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Num1))
            {
                // Increase fire rate
                fireRate++;
                state = State::PLAYING;
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Num2))
            {
                clipSize += clipSize;
                state = State::PLAYING;
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Num3))
            {
                player.upgradeHealth();
                state = State::PLAYING;
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Num4))
            {
                player.upgradeSpeed();
                state = State::PLAYING;
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Num5))
            {
                healthPickup.upgrade();
                state = State::PLAYING;
            }

            if (sf::Keyboard::isKeyPressed(sf::Keyboard::Scancode::Num6))
            {
                ammoPickup.upgrade();
                state = State::PLAYING;
            }

            // Handling playing state
            if (state == State::PLAYING)
            {
                bgMusic.play();
                // Increase the wave number
                wave++;


                // Prepare the level
                arena.size.x = 500 * wave;
                arena.size.y = 500 * wave;
                arena.position.x = 0;
                arena.position.y = 0;

                // Passing the Vertex Array by reference to the createBackground() function
                int tileSize = createBackground(background, arena);

                // Spawning the player in the middle of the arena
                player.spawn(arena, sf::Vector2u(resolution), tileSize);


                // Configuring the pickups
                healthPickup.setArena(arena);
                ammoPickup.setArena(arena);
                // std::cout << "After setArena - Health spawned: " << healthPickup.isSpawned() << std::endl;
                // std::cout << "After setArena - Ammo spawned: " << ammoPickup.isSpawned() << std::endl;


                


                // Creating a horde of Zombies
                numZombies = 5 * wave;
                // Delete the previouslly allocated memory if exists
                delete[] zombies;
                zombies = createHorde(numZombies, arena);
                std::cout << "Horde created successfully!" << std::endl;
                numZombiesAlive = numZombies;

                // Play the power up sound
                soundPowerup.play();


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

                            // Play the splat sound
                            soundSplat.play();

                        } // end of if (bullets[i].getPosition().findIntersection(zombies[j].getPosition()))
                    } // end of if (bullets[i].isInFlight() && zombies[j].isAlive())
                } // end of for (int j = 0; j < numZombies; j++)
            } // end of for (int i = 0; i < 100; i++)




            // Collision detection with the player
            for (int i = 0; i < numZombies; i++)
            {
                if (zombies[i].isAlive() && player.getPosition().findIntersection(zombies[i].getPosition()) )
                {
                    if (player.hit(gameTimeTotal))
                    {
                        soundHit.play();
                    }

                    if (player.getHealth() <= 0)  
                    {
                       state = State::GAME_OVER;
                       std::ofstream outputFile("gamedata/scores.txt");
                       // Writing the data
                       outputFile << highScore;
                       outputFile.close();
                    }
                }
            } // End collision detection with player


            /* Collision detection with the pickups */ 
            /*          ********************        */
            // Detecting health pickup colission
            if (player.getPosition().findIntersection(healthPickup.getPosition()) && healthPickup.isSpawned())
            {
                player.increaseHealthLevel(healthPickup.gotIt());
                soundPickup.play();
            }

            // Detecting ammo pickup collision
            if (player.getPosition().findIntersection(ammoPickup.getPosition()) && ammoPickup.isSpawned())
            {
                bulletsSpare += ammoPickup.gotIt();
                soundReload.play();
            }




            // Size up the health bar
            healthBar.setSize(sf::Vector2f(player.getHealth() * 3, 50));

            // Increment the number of frames since the last update
            framesSinceLastUpdate++;

            // recalculate every fpsMeasurementFrameInterval frames
            if (framesSinceLastUpdate > fpsMeasurementFrameInterval)
            {
                // update game hud text
                std::stringstream ssAmmo;
                std::stringstream ssScore;
                std::stringstream ssHighScore;
                std::stringstream ssWave;
                std::stringstream ssZombiesAlive;

                // updating Ammo text
                ssAmmo << bulletsInClip << "/" << bulletsSpare;
                textAmmo.setString(ssAmmo.str());

                // updating the score text
                ssScore << "Score: " << score;
                textScore.setString(ssScore.str());

                // updating the highScore text
                ssHighScore << "High Score: " << highScore;
                textHighScore.setString(ssHighScore.str());

                // updating the wave
                ssWave << "Wave: " << wave;
                textWaveNumber.setString(ssWave.str());

                // Updating the Zombies alive
                ssZombiesAlive << "Zombies: " << numZombiesAlive;
                textZombiesRemaining.setString(ssZombiesAlive.str());

                framesSinceLastUpdate = 0;

            } // end of if (framesSinceLastUpdate > fpsMeasurementFrameInterval)




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

            
            // Switch to HUD view
            window.setView(hudView);


            // Draw all the HUD elements
            window.draw(spriteAmmoIcon);
            window.draw(textAmmo);
            window.draw(textScore);
            window.draw(textHighScore);
            window.draw(healthBar);
            window.draw(textWaveNumber);
            window.draw(textZombiesRemaining);






        } // End of if (state == State::PLAYING)

        if (state == State::LEVELING_UP)
        {

            window.draw(spriteGameOver);
            window.draw(textLevelUp);

        }

        if (state == State::PAUSED)
        {
            window.draw(textPaused);
        }

        if (state == State::GAME_OVER)
        {
            window.draw(spriteGameOver);
            window.draw(textGameOver);
            window.draw(textScore);
            window.draw(textHighScore);

        }



        // window.draw(text);
        
        window.display();


    } // end of while (window.isOpen()) game loop




    delete[] zombies;
    return 0;
}

