//
//  main.cpp
//  StealthPlatformer
//
//  Created by James Vaughan Craster on 29/08/2016.
//  Copyright (c) 2016 James Vaughan Craster. All rights reserved.
//


#include "ResourcePath.hpp"
#include <vector>
#include <iostream>
#include <cmath>
#include "LevelLoader.h"
#include "Camera.h"
#include <iostream>
#include "LevelGenerator.h"
#include "Controller.h"
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <Box2D/Box2D.h>


class smartClock:public sf::Clock{
public:
    bool active = 0;
};

std::vector<std::vector<int>> generatePlatform(sf::Vector2f Size, int width, bool spikes){
    std::vector<std::vector<int>> levelVector;
    int counter = 0;
    for(int x = 0; x < Size.x; x++){
        levelVector.push_back(std::vector<int>{});
        for(int y = 27; y >= 0; y--){
            if(y == Size.y){
                levelVector[x].push_back(99);
                counter ++;
            }
            if(y > Size.y|| y <= width){
                levelVector[x].push_back(0);
            }else{
                levelVector[x].push_back(1);
                
            }
        }
    }
    if(Size.x > 8 && spikes){
    generator(levelVector, 99, std::vector<int>{0,10}, std::vector<float>{0.9,0.1}, std::vector<bool>{0,1}, std::vector<int>{0,2},std::vector<int>{0,0});
    }
    return levelVector;
}
std::vector<std::vector<int>> generateNarrowPassage(sf::Vector2f BottomSize, sf::Vector2f UpperSize){
    std::vector<std::vector<int>> levelVector;
    for(int x = 0; x < BottomSize.x; x++){
        levelVector.push_back(std::vector<int>{});
        for(int y = 27; y >= 0; y--){
            if(y < BottomSize.y){
                levelVector[x].push_back(1);
            }
            else if(y == UpperSize.y -1){
                levelVector[x].push_back(99);
            }
            else if(y >= UpperSize.y){
                levelVector[x].push_back(1);
                
            }else{
                levelVector[x].push_back(0);
                
            }
        }
    }
    if(BottomSize.x > 3){
        generator(levelVector, 99, std::vector<int>{0,12}, std::vector<float>{0,1}, std::vector<bool>{0,1}, std::vector<int>{0,2},std::vector<int>{0,0});
    }
    return levelVector;
}
std::vector<std::vector<int>> generateGap(float width){
    std::vector<std::vector<int>> levelVector;
    for(int x = 0; x < width; x++){
        levelVector.push_back(std::vector<int>{});
        for(int y = 27; y > 0; y--){
           levelVector[x].push_back(0);        }
    }
    return levelVector;
}

int addChain(ChunkChain &chunkChain, float previousHeight, float distance){
    //make difficulty a function of distance
    
    srand(time(0));
    int height = rand()%8 - 3 + previousHeight;
    if(height >= 23){
        height = 23 - rand() % 7;
    }
    if(height <= 3){
        height = 2 + rand()%3;
    }
    int width;
    if(previousHeight > height){
        width =  rand()%9 + 0;
    }else{
        width = rand()%9 + 4;
        
    }
    
    chunkChain.appendChunk(Chunk(generateGap(width),sf::Vector2f(chunkChain.chunkChain[chunkChain.chunkChain.size()-1].position.x + chunkChain.chunkChain[chunkChain.chunkChain.size()-1].size.x,chunkChain.chunkChain[chunkChain.chunkChain.size()-1].position.y)));
    

    width = rand()%16 + 3;

    if(previousHeight < height && rand()%2){
    int narrow = rand()%2 + 3;
    chunkChain.appendChunk(Chunk(generateNarrowPassage(sf::Vector2f(width, height),sf::Vector2f(width, height + narrow)),sf::Vector2f(chunkChain.chunkChain[chunkChain.chunkChain.size()-1].position.x + chunkChain.chunkChain[chunkChain.chunkChain.size()-1].size.x,chunkChain.chunkChain[chunkChain.chunkChain.size()-1].position.y)));
    
    }else{
        chunkChain.appendChunk(Chunk(generatePlatform(sf::Vector2f(width, height),0,1),sf::Vector2f(chunkChain.chunkChain[chunkChain.chunkChain.size()-1].position.x + chunkChain.chunkChain[chunkChain.chunkChain.size()-1].size.x,chunkChain.chunkChain[chunkChain.chunkChain.size()-1].position.y)));
    }
    return height;
}

void drawStars(sf::RenderWindow & window, sf::View & playerView, int seed){
    srand(seed);
    sf::CircleShape moon(20);
    moon.setFillColor(sf::Color(170,170,170,255));
    moon.setPosition(playerView.getCenter().x - playerView.getSize().x/2 + 700, 20);
    for(int i = 0; i < rand()% 5 + 30 ; i++){
    sf::RectangleShape star;
    star.setSize(sf::Vector2f(2,2));
    star.setFillColor(sf::Color(150,150,150,150));
    star.setPosition(rand() % 1400 + playerView.getCenter().x - playerView.getSize().x/2, rand() % 70 + playerView.getCenter().y - playerView.getSize().y/2);
    
    window.draw(star);
    }
    //window.draw(moon);
    
}

int startLevel(sf::RenderWindow & window, int & score, int mode){
    ControllerController controllerController;
    std::vector<Spike*> spikeVector;
    std::vector<sf::RectangleShape*> tileVector;
    ChunkChain chunkChain;
    chunkChain.appendChunk(Chunk(generatePlatform(sf::Vector2f(20,10),0,0),sf::Vector2f(0,0)));
    chunkChain.appendChunk(Chunk(generateGap(3),sf::Vector2f(20 + 20*32,0)));
    std::vector<sf::RectangleShape*> showTileVector;
    auto levelVector = loadLevelFromChunkChain(chunkChain, sf::Vector2f(32,32), spikeVector, tileVector, showTileVector);
    
    //load images
    sf::Texture runNinja1;
    runNinja1.loadFromFile(resourcePath() + "BlueRun.png", sf::IntRect(0,0,10,20));
    sf::Texture runNinja2;
    runNinja2.loadFromFile(resourcePath() + "BlueRun.png", sf::IntRect(10,0,10*2,20));
    sf::Texture runNinja3;
    runNinja3.loadFromFile(resourcePath() + "BlueRun.png", sf::IntRect(10*2,0,10*3,20));
    sf::Texture runNinja4;
    runNinja4.loadFromFile(resourcePath() + "BlueRun.png", sf::IntRect(10*3,0,10*4,20));
    sf::Texture runNinja5;
    runNinja5.loadFromFile(resourcePath() + "BlueRun.png", sf::IntRect(10*4,0,10*5,20));
    sf::Texture runNinja6;
    runNinja6.loadFromFile(resourcePath() + "BlueRun.png", sf::IntRect(10*5,0,10*6,20));
    sf::Texture runNinja7;
    runNinja7.loadFromFile(resourcePath() + "BlueRun.png", sf::IntRect(10*6,0,10*7,20));
    sf::Texture runNinja8;
    runNinja8.loadFromFile(resourcePath() + "BlueRun.png", sf::IntRect(10*7,0,10*8,20));
    sf::Texture runNinja9;
    runNinja9.loadFromFile(resourcePath() + "BlueRun.png", sf::IntRect(10*8,0,10*9,20));
    sf::Texture runNinja10;
    runNinja10.loadFromFile(resourcePath() + "BlueRun.png", sf::IntRect(10*9,0,10*10,20));
    
    sf::Texture runEnemy1;
    runEnemy1.loadFromFile(resourcePath() + "RedRun.png", sf::IntRect(0,0,10,20));
    sf::Texture runEnemy2;
    runEnemy2.loadFromFile(resourcePath() + "RedRun.png", sf::IntRect(10,0,10*2,20));
    sf::Texture runEnemy3;
    runEnemy3.loadFromFile(resourcePath() + "RedRun.png", sf::IntRect(10*2,0,10*3,20));
    sf::Texture runEnemy4;
    runEnemy4.loadFromFile(resourcePath() + "RedRun.png", sf::IntRect(10*3,0,10*4,20));
    sf::Texture runEnemy5;
    runEnemy5.loadFromFile(resourcePath() + "RedRun.png", sf::IntRect(10*4,0,10*5,20));
    sf::Texture runEnemy6;
    runEnemy6.loadFromFile(resourcePath() + "RedRun.png", sf::IntRect(10*5,0,10*6,20));
    sf::Texture runEnemy7;
    runEnemy7.loadFromFile(resourcePath() + "RedRun.png", sf::IntRect(10*6,0,10*7,20));
    sf::Texture runEnemy8;
    runEnemy8.loadFromFile(resourcePath() + "RedRun.png", sf::IntRect(10*7,0,10*8,20));
    sf::Texture runEnemy9;
    runEnemy9.loadFromFile(resourcePath() + "RedRun.png", sf::IntRect(10*8,0,10*9,20));
    sf::Texture runEnemy10;
    runEnemy10.loadFromFile(resourcePath() + "RedRun.png", sf::IntRect(10*9,0,10*10,20));
    
    sf::Texture spikeTexture;
    spikeTexture.loadFromFile(resourcePath() + "Spikes.png");
    for (int x = 0; x < spikeVector.size(); x++){
        spikeVector[x]->spikeSprite.setTexture(spikeTexture);
    }
    sf::Texture dustTexture1;
    dustTexture1.loadFromFile(resourcePath() + "SmokeAnimation.png", sf::IntRect(0,0,11,10));
    sf::Texture dustTexture2;
    dustTexture2.loadFromFile(resourcePath() + "SmokeAnimation.png", sf::IntRect(11,0,11*2,10));
    sf::Texture dustTexture3;
    dustTexture3.loadFromFile(resourcePath() + "SmokeAnimation.png", sf::IntRect(11*2,0,11*3,10));
    
    sf::Clock AfterJumpClock;
    
    
    smartClock deathClock;
    
    Player player(std::vector<AnimationController> {},25,10,23, sf::Vector2f(5,9),sf::Vector2f(-5,-9),sf::Vector2f(10,20),46000/20,3);
    Enemy enemy(std::vector<AnimationController> {},20,10,23,sf::Vector2f(5,9),sf::Vector2f(5,9),sf::Vector2f(10,20),16000/20);
    player.dust.animationControllerVector.push_back(AnimationController({0.05f,0.1f,0.1f},{&dustTexture1,&dustTexture2,&dustTexture3}, &player.dust.sprite));
    player.dust.sprite.setTexture(dustTexture1);
    
    AnimationController run(std::vector<float> (10,0.5),std::vector<sf::Texture*> {&runNinja1,&runNinja2,&runNinja3,&runNinja4,&runNinja5,&runNinja6,&runNinja7,&runNinja8,&runNinja9,&runNinja10},&player.sprite);
    AnimationController redRun(std::vector<float> (10,0.5),std::vector<sf::Texture*> {&runEnemy1,&runEnemy2,&runEnemy3,&runEnemy4,&runEnemy5,&runEnemy6,&runEnemy7,&runEnemy8,&runEnemy9,&runEnemy10},&enemy.sprite);
    run.active = 1;
    player.animationControllerVector.push_back(run);
    enemy.animationControllerVector.push_back(redRun);
    
    player.sprite.setOrigin(5,8);
    player.sprite.setScale(2, 2);
    player.sprite.setPosition(sf::Vector2f(100,300));
    player.physicsController.displacement = sf::Vector2f(100,300);
    enemy.sprite.setPosition(300, 600);
    enemy.sprite.setOrigin(5, 8);
    enemy.sprite.setScale(2,2);
    enemy.sprite.setTexture(runEnemy1,true);
    enemy.animationControllerVector[0].active = 1;
    
    sf::Clock jumpClock;
    player.sprite.setTexture(runNinja1,true);
    player.animationControllerVector[0].setAnimationSpeed(4);
    
    
    sf::Clock frameTime;
    long double storedFrameTime;
    
    sf::Clock cameraClock;
    double cameraTime;
    double cameraAccumulator = 0;
    
    sf::Texture floorTexture;
    floorTexture.loadFromFile(resourcePath() + "FloorTexture.png");
    sf::Texture floorTextureRotated;
    floorTextureRotated.loadFromFile(resourcePath() + "FloorTextureRotated.png");
    
    sf::View playerView(sf::FloatRect(0, 0, int(1400), int(851)));
    window.setView(playerView);
    sf::Vector2f playerViewScrollVector;
    bool mostRecentlyPressed = 1;
    long double accumulator = 0;
    const long double dt = 1/360.f;
    srand(time(0));
    int seed = rand();
    
    
    
    sf::Vector2f swingPoint(400,400);
    
    sf::Texture coin1;
    coin1.loadFromFile(resourcePath() + "coin.png",sf::IntRect(0,0,10,18));
    sf::Texture coin2;
    coin2.loadFromFile(resourcePath() + "coin.png",sf::IntRect(10,18,10*2,18*2));
    sf::Texture coin3;
    coin3.loadFromFile(resourcePath() + "coin.png",sf::IntRect(10*2,18*2,10*3,18*3));
    sf::Texture coin4;
    coin4.loadFromFile(resourcePath() + "coin.png",sf::IntRect(10*3,18*3,10*4,18*4));
    
    bool storedSouthContact;
    int storedDirection = player.direction;
    bool jumpCutoff = 0;

    sf::Vector2f cameraMovement = sf::Vector2f(window.sf::Window::getSize().x/2.f,window.sf::Window::getSize().y/2.f);
    sf::Sprite healthSprite;
    sf::Texture healthTexture;
    healthTexture.loadFromFile(resourcePath() + "HealthBar.png");
    healthSprite.setTexture(healthTexture);
    sf::Joystick ps3Cont;
    player.isJumping = 1;
    
    sf::Font font;
    font.loadFromFile(resourcePath() + "arcadeclassic.regular.ttf");
    sf::Text text;
    text.setFont(font);
    text.setPosition(700, 50);
    float previousHeight = 10;
    

    
    
    int highScore = 0;
    
    std::ifstream inf(resourcePath() + "Sample.dat");
    if(inf){
        std::string string;
        getline(inf,string);
        highScore = std::stoi(string);
        inf.close();
    }if(!inf){
        inf.close();
        std::ofstream ioutf(resourcePath() + "Sample.dat");
        ioutf << "0";
        ioutf.close();
        inf.open(resourcePath() + "Sample.dat");
        std::string string;
        getline(inf,string);
        
        highScore = std::stoi(string);
        inf.close();
        
        
    }
    long double t = 0;
    sf::Vector2f currentPos = player.sprite.getPosition();
    
    ChunkChain backGroundChunkChain;
    backGroundChunkChain.appendChunk(Chunk(generatePlatform(sf::Vector2f(11,20),0,0),sf::Vector2f(0,0)));
    backGroundChunkChain.appendChunk(Chunk(generateGap(10),sf::Vector2f(20,0)));
    
    std::vector<Spike*> junkSpikeVector;
    std::vector<sf::RectangleShape*> junkTileVector;
    std::vector<sf::RectangleShape*> backgroundShowTileVector;
    
    auto backGroundLevelVector = loadLevelFromChunkChain(backGroundChunkChain, sf::Vector2f(32,32), junkSpikeVector, junkTileVector,backgroundShowTileVector);
    
    
    sf::Clock hasBeenJumping;
    
    sf::CircleShape hangPoint(10);
    hangPoint.setPosition(600, 400);
    hangPoint.setFillColor(sf::Color::Green);
    hangPoint.setOrigin(10, 10);
    

    frameTime.restart();
    float fallHeight = 0;
    float peakHeight = 0;
    storedFrameTime = 1/60.f;
    bool earlyJump = 0;
    sf::Vector2f crossRadialVector;
    int doublePress = 0;
    while (window.isOpen())
    {
        // Process events
        // std::cout << 1/storedFrameTime << "," << std::endl;
       // std::cout << player.sprite.getPosition().x << "," << std::endl;
        //fixing my timestep results in poor performance on my macbook. Cutting out this line will fix the physics timestep:
         controllerController.getInput();
        if(mode){
           storedFrameTime = 1/60.f;
        }
        if(storedFrameTime > 1/50.f){
            std::cout << "Occurred" << std::endl;
            storedFrameTime = 1/60.f;
        }
        float accumulateCount = 0;
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Close window: exit
            if (event.type == sf::Event::Closed) {
                window.close();
                return 1;
            }
            
            // Escape pressed: exit
            if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape) {
                window.close();
                return 1;
            }
           /* if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::D) {
                
                if(mostRecentlyPressed == 0){
                    
                    // player.physicsController.xVelocity *= pow(0.0,(1/60)/(storedFrameTime));
                    if(player.physicsController.tileCollisionController.southContact == 1){
                    player.physicsController.velocity.x *= 0.15;
                    }
                    
                    player.sprite.setScale(2, 2);
                    
                }
                
                //player.animationControllerVector[0].setCurrentFrame(5);
                
                mostRecentlyPressed = 1;
                
            }*/
            //if (event.type == sf::Event::KeyReleased && event.key.code == sf::Keyboard::W) {
             
             //if(jumpCutoff && player.isJumping){
             
            // if(player.physicsController.velocity.y < 0){
             
            // player.physicsController.velocity.y = 0;
             
            // jumpCutoff = 0;
             
            // }
             
            // }
             
            // }
            
           /* if (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::A) {
             
             if(mostRecentlyPressed == 1){
                 if(player.physicsController.tileCollisionController.southContact == 1){

                     player.physicsController.velocity.y *= 0.15 /*pow(0.0,(1/60)/(storedFrameTime))*//*;
                 }
            
             }
             
             
             
             mostRecentlyPressed = 0;
             
             }*/
            /*if((event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::W) ||  ps3Cont.isButtonPressed(0,11)){
             
             if((player.physicsController.tileCollisionController.southContact || jumpClock.getElapsedTime().asSeconds() < 0.1) && player.isJumping == 0){
             
             player.isJumping = 1;
             
             player.animationControllerVector[0].active = 0;
             
             player.animationControllerVector[0].setCurrentFrame(0);
             
             jumpCutoff = 1;
             
             player.physicsController.force.y = -450000;
             
             }
             
             }else{
             
             if(!player.physicsController.tileCollisionController.southContact){
             
             // player.isJumping = 0;
             
             player.animationControllerVector[0].active = 1;
             
             
             
             }*/
             
             
             
             
        }
        //or if button held!
        //issue: when holding button to jump again before peaking, this interferes with jumping again on landing.
        if(controllerController.wasPressed(Controller::JUMP)){
            doublePress += 1;
            if(!player.physicsController.tileCollisionController.southContact && player.physicsController.velocity.y > 0)
            {
                AfterJumpClock.restart();
                earlyJump = 0;
            }
            if(!player.physicsController.tileCollisionController.southContact)
            {
                
                player.animationControllerVector[0].active = 1;
                
            }
            
        }
        
        //if (controllerController.wasReleased(Controller::JUMP)) {
        //  if(jumpCutoff && player.isJumping){
        //    if(player.physicsController.velocity.y < 0){
        //      player.physicsController.velocity.y = 0;
        //      jumpCutoff = 0;
        //}
        //}
        // }
        
        //let there be minimum height for jump initiated before landing.
        //place in physics loop
        if(!(controllerController.isButtonHeld(Controller::JUMP)) && (player.isJumping == 1)){
            if(jumpCutoff){
                if(player.physicsController.velocity.y < 0 /*&& player.physicsController.velocity.y > -850*/){
                    player.physicsController.velocity.y = 0;
                    jumpCutoff = 0;
                    
                }
            }
        }
        if(controllerController.isButtonHeld(Controller::JUMP)){
            //and if the jump button has been pressed twice while the player has been in midair
            if((player.physicsController.tileCollisionController.southContact || jumpClock.getElapsedTime().asSeconds() < 0.15) && player.isJumping == 0 && doublePress >=1)
            {
                player.isJumping = 1;
                player.animationControllerVector[0].active = 0;
                player.animationControllerVector[0].setCurrentFrame(0);
                jumpCutoff = 1;
                player.physicsController.force.y = -10970000;
            }
        }
        
        if(!player.isJumping){
            player.animationControllerVector[0].active = 1;
        }
        if(player.physicsController.tileCollisionController.southContact != storedSouthContact){
            if(player.physicsController.tileCollisionController.southContact == 0){
                jumpClock.restart();
                earlyJump = 0;
                doublePress = 0;
            }else{
                player.isJumping = 0;
                if(AfterJumpClock.getElapsedTime().asSeconds() < 0.10 && earlyJump == 0 && doublePress >= 2){
                    player.isJumping = 1;
                    player.animationControllerVector[0].active = 0;
                    player.animationControllerVector[0].setCurrentFrame(0);
                    jumpCutoff = 1;
                    player.physicsController.force.y = -10970000;
                    earlyJump = 1;
                    
                }
                
                if(player.peakVelocity > 990){
                    player.dust.animationControllerVector[0].setCurrentFrame(0);
                    player.dust.animationControllerVector[0].active = 1;
                }
            }
        }else{
            //earlyJump = 0;
            if(player.dust.animationControllerVector[0].getCurrentFrame() == 2){
                player.dust.animationControllerVector[0].active = 0;
            }
        }
        
        player.dust.animationControllerVector[0].update();
        while(chunkChain.chunkChain.size() != 0 && (playerView.getCenter().x + playerView.getSize().x/2 > chunkChain.chunkChain[chunkChain.chunkChain.size()-1].position.x + chunkChain.chunkChain[chunkChain.chunkChain.size()-1].size.x - 3 - 20 )){
            previousHeight = addChain(chunkChain, previousHeight,int(player.sprite.getPosition().x/32));
            levelVector = loadLevelFromChunkChain(chunkChain, sf::Vector2f(32,32), spikeVector, tileVector,showTileVector);
            //addChain(backGroundChunkChain, 10, 0);
            //backGroundLevelVector = loadLevelFromChunkChain(backGroundChunkChain, sf::Vector2f(32,32), junkSpikeVector, junkTileVector, backgroundShowTileVector);
        }
        
        player.animationControllerVector[0].setAnimationSpeed(std::abs(player.physicsController.velocity.x)/50);
        storedSouthContact = player.physicsController.tileCollisionController.southContact;
        window.clear();
        drawStars(window, playerView,seed);
        
        if(player.isJumping && (player.physicsController.velocity.y < 0)){
            player.animationControllerVector[0].setCurrentFrame(0);
        }
        if(player.isJumping && player.physicsController.velocity.y  > 0 && player.animationControllerVector[0].getCurrentFrame() == 0){
            peakHeight = player.sprite.getPosition().y;
        }
        accumulator += storedFrameTime;
        sf::Vector2f originalPos = player.sprite.getPosition();
        sf::Vector2f previousPos;
        while (accumulator >= dt) {
            sf::Clock physicsTime;
            player.testTileCollisions(tileVector);
            
            if(player.physicsController.tileCollisionController.southContact == 0){
                player.physicsController.force.y += player.physicsController.mass * player.gravity;
            }else{
                if(std::abs(player.physicsController.velocity.x) < 0.1){
                    player.animationControllerVector[0].setCurrentFrame(3);
                    
                }else{
                    
                }
            }
            
            if(std::abs(player.physicsController.velocity.x) < 0.1){
                player.animationControllerVector[0].setCurrentFrame(3);
                
            }
            /*if(mostRecentlyPressed == 1){
                
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::D)||  ps3Cont.getAxisPosition(0, sf::Joystick::Axis::X) > 10)
                    
                {
                    
                    player.direction = 1;
                    
                    player.animationControllerVector.setActiveAnimation(0);
                    
                    player.sprite.setScale(2, 2);
                    
                    
                    
                }else if(sf::Keyboard::isKeyPressed(sf::Keyboard::A) ||  ps3Cont.getAxisPosition(0, sf::Joystick::Axis::X) < -10)
                    
                {
                    
                    player.direction = -1;
                    
                    player.animationControllerVector.setActiveAnimation(0);
                    
                    // player.physicsController.xVelocity *= 0.15;
                    
                    player.sprite.setScale(-2, 2);
                    
                    
                    
                    
                    
                }else
                    
                {
                    
                    player.direction  = 0;
                    if(player.physicsController.tileCollisionController.southContact == 1){
                        player.physicsController.velocity.x *= 0.15;
                    }if(player.physicsController.tileCollisionController.southContact == 0){
                        player.physicsController.velocity.x *= 0.95;
                    }
                    
                    //player.animationControllerVector[0].setCurrentFrame(3);
                    
                }
                
            }else
                
            {
                
                if(sf::Keyboard::isKeyPressed(sf::Keyboard::A) ||  ps3Cont.getAxisPosition(0, sf::Joystick::Axis::X) > 10)
                    
                {
                    
                    player.direction = -1;
                    
                    player.animationControllerVector.setActiveAnimation(0);
                    
                    player.sprite.setScale(-2, 2);
                    
                    
                    
                }else if(sf::Keyboard::isKeyPressed(sf::Keyboard::D))
                    
                {
                    
                    
                    
                    player.direction = 1;
                    
                    player.animationControllerVector.setActiveAnimation(0);
                    
                    player.sprite.setScale(2, 2);
                    
                    
                    
                    
                    
                }else
                    
                {
                    
                    player.direction  = 0;
                    if(player.physicsController.tileCollisionController.southContact == 1){
                        player.physicsController.velocity.x *= 0.15;
                    }
                    
                    
                    
                }
                
                
                
            }
            
            
            
            if(storedDirection != player.direction){
                if(player.physicsController.tileCollisionController.southContact == 1){
                    player.physicsController.velocity.x *= 0.40;
                }
                
            }*/
        
            player.direction = 1;
            if(player.physicsController.tileCollisionController.southContact && !player.physicsController.tileCollisionController.eastContact){
                player.physicsController.force.x += 90000 * player.direction;
            }else{
                // player.physicsController.force.x += 90000 * player.direction;
            }
            if(player.alive == 0){
                if(player.sprite.getOrigin() != sf::Vector2f(5,18)){
               
                    player.sprite.move(player.sprite.getOrigin() + sf::Vector2f(5,18));
                    player.sprite.setOrigin(5,18);
                }
                
                player.physicsController.force.x = 0;
                player.animationControllerVector[0].active = 0;
                player.animationControllerVector[0].setCurrentFrame(3);
                if(player.sprite.getRotation() < 90){
                    player.sprite.rotate(4);
                }
            }
            
            if(player.physicsController.tileCollisionController.eastContact){
                player.physicsController.force.x = 0;
            }
            previousPos = currentPos;
            float magnitude = pow(pow(hangPoint.getPosition().x - player.sprite.getPosition().x,2) + pow(hangPoint.getPosition().y - player.sprite.getPosition().y,2),0.5);
            
           float crossRadialForce = player.physicsController.force.x * -(hangPoint.getPosition().y - player.sprite.getPosition().y)/magnitude + player.physicsController.force.y * (hangPoint.getPosition().x - player.sprite.getPosition().x)/magnitude;
            
            
           crossRadialVector = sf::Vector2f(- crossRadialForce * (hangPoint.getPosition().y - player.sprite.getPosition().y)/magnitude,crossRadialForce *
                                        (hangPoint.getPosition().x - player.sprite.getPosition().x)/magnitude);
            
            float radialForce = player.physicsController.force.x * (hangPoint.getPosition().x - player.sprite.getPosition().x)/magnitude + player.physicsController.force.y * (hangPoint.getPosition().y - player.sprite.getPosition().y)/magnitude;
            
            radialForce = player.physicsController.force.y + (player.physicsController.mass * pow(pow(player.physicsController.velocity.x,2)+ pow(player.physicsController.velocity.y,2),1))/magnitude;
            /*+ (player.physicsController.force.y / player.physicsController.mass  /*+ player.physicsController.force.x / player.physicsController.mass) * player.physicsController.mass*/;
            //radialForce = 2 * player.physicsController.force.y;
            
            sf::Vector2f radialVector = sf::Vector2f(radialForce * (hangPoint.getPosition().x - player.sprite.getPosition().x)/magnitude,radialForce * (hangPoint.getPosition().y - player.sprite.getPosition().y)/magnitude);
            
            if(radialForce < 0){
                radialForce = 0;
            }
        
            //player.physicsController.force.x *= -(hangPoint.getPosition().y - player.sprite.getPosition().y)/magnitude;
            //player.physicsController.force.y *= (hangPoint.getPosition().x - player.sprite.getPosition().x)/magnitude;
            //player.physicsController.force = crossRadialVector;
            //std::cout << crossRadialForce << std::endl;
            //float alpha = atan2f(-(hangPoint.getPosition().y - player.sprite.getPosition().y)/magnitude, (hangPoint.getPosition().x - player.sprite.getPosition().x)/magnitude);
            //if(sf::Keyboard::isKeyPressed(sf::Keyboard::L)){
                //player.physicsController.force += radialVector;
            //}else{
                //hangPoint.setPosition(player.sprite.getPosition().x + 300, player.sprite.getPosition().y - 300);
            
            //}
            
            //player.physicsController.force = crossRadialForce;
           // sf::Vector2f(-(hangPoint.getPosition().y - player.sprite.getPosition().y)/magnitude + player.sprite.getPosition().x, (hangPoint.getPosition().x - player.sprite.getPosition().x)/magnitude + player.sprite.getPosition().y);
            
           
            player.update(t,dt,accumulator,tileVector,mostRecentlyPressed);

            accumulateCount += 1;

            accumulator -= dt;
            t += dt;
        }

     
        player.sprite.setPosition(int(player.physicsController.displacement.x*(accumulator/dt) + player.physicsController.previous.displacement.x*(1-(accumulator/dt))),
                                int(player.physicsController.displacement.y*(accumulator/dt) + player.physicsController.previous.displacement.y*(1-(accumulator/dt))));
        if(player.physicsController.tileCollisionController.southContact == 1){player.dust.sprite.setPosition(player.sprite.getPosition() - sf::Vector2f(17,-7));
        }
      if(player.alive == 0){
            player.sprite.move(0, 5);
        }
        for(int x = 0; x < spikeVector.size(); x++){
            if(player.boundingBox.intersects(spikeVector[x]->collisionBox)){
                player.damage(1);
            }
        }
        
        sf::VertexArray line(sf::LinesStrip);
        line.append(sf::Vertex(sf::Vector2f(playerView.getCenter().x - playerView.getSize().x/2, (27-previousHeight)*32),sf::Color::White));
        line.append(sf::Vertex(sf::Vector2f(playerView.getCenter().x + playerView.getSize().x/2, (27-previousHeight)*32),sf::Color::White));
        for(int i = 0; i < chunkChain.chunkChain.size(); i++){
            if(i == chunkChain.readPosition){
            }else{
            }
             
        }
        
        if(chunkChain.chunkChain.size() > 0){
            while(playerView.getCenter().x - playerView.getSize().x/2 > chunkChain.chunkChain[chunkChain.readPosition].position.x + /*chunkChain.chunkChain[1].size.x*/ + chunkChain.chunkChain[chunkChain.readPosition].size.x){
                chunkChain.removeChunk();
                levelVector.clear();
                levelVector = loadLevelFromChunkChain(chunkChain, sf::Vector2f(32,32), spikeVector, tileVector,showTileVector);
            }
        }
        
        sf::VertexArray grappleLine(sf::LinesStrip);
        grappleLine.append(sf::Vertex(player.sprite.getPosition(), sf::Color::White));
        grappleLine.append(sf::Vertex(hangPoint.getPosition(), sf::Color::White));
        
        float magnitude = powf(powf(hangPoint.getPosition().x - player.sprite.getPosition().x,2) + powf(hangPoint.getPosition().y - player.sprite.getPosition().y,2),0.5);
        sf::VertexArray perpendicularLine(sf::LinesStrip);
        perpendicularLine.append(sf::Vertex(player.sprite.getPosition(), sf::Color::White));
        perpendicularLine.append(sf::Vertex(sf::Vector2f(-(hangPoint.getPosition().y - player.sprite.getPosition().y)/magnitude * 50 + player.sprite.getPosition().x, (hangPoint.getPosition().x - player.sprite.getPosition().x)/magnitude * 50 + player.sprite.getPosition().y), sf::Color::White));
        
        perpendicularLine.append(sf::Vertex(crossRadialVector + player.sprite.getPosition(),sf::Color::Green));
        
        sf::RectangleShape skyScraper;
        skyScraper.setPosition(100, 200);
        skyScraper.setSize(sf::Vector2f(300, 1000));
        skyScraper.setOutlineColor(sf::Color(24/3,118/3,118/3));
        skyScraper.setOutlineThickness(5);
        skyScraper.setFillColor(sf::Color(24,118,118,0));
        skyScraper.move(int(-(cameraMovement.x - playerView.getCenter().x)/1.1),0);
        window.draw(skyScraper);
        
        positionView(playerView, sf::Vector2f(player.sprite.getPosition().x+600,player.sprite.getPosition().y-200), 1/60.f);

        window.setView(playerView);
        
        for(int i = 0; i <  backgroundShowTileVector.size(); i++){
            backgroundShowTileVector[i]->setFillColor(sf::Color::Black);
            backgroundShowTileVector[i]->setOutlineColor(sf::Color(100,100,100,100));
            backgroundShowTileVector[i]->setOutlineThickness(0);
            backgroundShowTileVector[i]->setSize(sf::Vector2f(backgroundShowTileVector[i]->getSize().x+1, backgroundShowTileVector[i]->getSize().y+1));
            window.draw(*backgroundShowTileVector[i]);
            backgroundShowTileVector[i]->setSize(sf::Vector2f(backgroundShowTileVector[i]->getSize().x-1, backgroundShowTileVector[i]->getSize().y-1));
            
        }
        drawFloorSpriteFromLevelVector(backGroundLevelVector, floorTexture, floorTextureRotated, window);
        
        for(int i = 0; i < tileVector.size(); i++){
            tileVector[i]->setFillColor(sf::Color::Black);
            tileVector[i]->setOutlineColor(sf::Color(100,100,100,100));
            tileVector[i]->setSize(sf::Vector2f(tileVector[i]->getSize().x+1, tileVector[i]->getSize().y+1));
            tileVector[i]->setSize(sf::Vector2f(tileVector[i]->getSize().x-1, tileVector[i]->getSize().y-1));
            
        }
        for(int i = 0; i < showTileVector.size(); i++){
            showTileVector[i]->setFillColor(sf::Color::Black);
            showTileVector[i]->setOutlineColor(sf::Color(100,100,100,100));
            showTileVector[i]->setOutlineThickness(0);
            showTileVector[i]->setSize(sf::Vector2f(showTileVector[i]->getSize().x+1, showTileVector[i]->getSize().y+1));
            window.draw(*showTileVector[i]);
            showTileVector[i]->setSize(sf::Vector2f(showTileVector[i]->getSize().x-1, showTileVector[i]->getSize().y-1));
            
        }
        
        drawFloorSpriteFromLevelVector(levelVector, floorTexture, floorTextureRotated, window);
        for(int i = 0; i < player.health; i++){
            healthSprite.setPosition(playerView.getCenter().x - playerView.getSize().x/2 + 21 + 49 * i,playerView.getCenter().y  - playerView.getSize().y/2 + 20);
            window.draw(healthSprite);
        }
       
        if(player.sprite.getPosition().y > 851){
            player.health = 0;
            player.die();
            for(int i = 0; i < tileVector.size(); i++){
                delete tileVector[i];
            }
            
            score = int(player.sprite.getPosition().x/32);
            if(score > highScore){
                std::ofstream outf(resourcePath() + "Sample.dat");
                outf << std::to_string(score);
                
                outf.close();
            }
            spikeVector.clear();
            tileVector.clear();
            levelVector.clear();
            return 0;
        }
        
        player.draw(window);
        
        player.animationControllerVector.setActiveAnimation(0);
        for (int x = 0; x < spikeVector.size(); x++){
            spikeVector[x]->spikeSprite.setTexture(spikeTexture);
            window.draw(spikeVector[x]->spikeSprite);
        }
        if(player.alive == 0 && !deathClock.active){
            deathClock.restart();
            deathClock.active = 1;
        }
        if(player.alive == 0 && deathClock.active && deathClock.getElapsedTime().asSeconds() > 0.5){
            for(int i = 0; i < tileVector.size(); i++){
                delete tileVector[i];
            }

            score = int(player.sprite.getPosition().x/32);
            if(score > highScore){
                std::ofstream outf(resourcePath() + "Sample.dat");
                outf << std::to_string(score);
                
                outf.close();
            }
            spikeVector.clear();
            tileVector.clear();
            levelVector.clear();
            return 0;
            
        }
        
        
        //storedDirection = player.direction;
    
        text.setString("Score\n" + std::to_string(int(player.sprite.getPosition().x/32)) + "M");
        text.setPosition(playerView.getCenter().x - playerView.getSize().x/2 + 1000,playerView.getCenter().y - playerView.getSize().y/2 + 30);
        
        text.setScale(sf::Vector2f(1.2,1.2));
        
        sf::Text text2;
        text2.setFont(font);
        text2.setString("High\n" + std::to_string(highScore)+ "M");
        text2.setPosition(playerView.getCenter().x - playerView.getSize().x/2 + 1200,playerView.getCenter().y - playerView.getSize().y/2 + 30);
        text2.setScale(sf::Vector2f(1.2,1.2));
        
        sf::Text frameText;
        frameText.setFont(font);
        frameText.setString(std::to_string(accumulateCount));
        frameText.setPosition(playerView.getCenter().x - playerView.getSize().x/2 + 200, 40);
        
        sf::Text highscoreText;
        highscoreText.setPosition(playerView.getCenter().x - playerView.getSize().x/2 + 920,playerView.getCenter().y - playerView.getSize().y/2 + 30);
        highscoreText.setFont(font);
        window.draw(highscoreText);
        window.draw(text2);
        
        window.draw(text);
        //window.draw(frameText);
        if(player.dust.animationControllerVector[0].active && player.physicsController.tileCollisionController.southContact == 1 && player.isJumping == 0){player.dust.sprite.setScale(1,1);
            window.draw(player.dust.sprite);
            player.dust.sprite.setScale(-1, 1); player.dust.sprite.move(30,0); window.draw(player.dust.sprite);}
        //window.draw(hangPoint);
        //window.draw(grappleLine);
        //window.draw(perpendicularLine);
        window.display();
        storedFrameTime =  frameTime.restart().asSeconds();
    }
    
    
    return EXIT_SUCCESS;
}
int showTitle(sf::RenderWindow & window){
    sf::View titleView;
    titleView.setCenter(0 + window.getSize().x/2, 0 + window.getSize().y/2);
    titleView.setSize(window.getSize().x, window.getSize().y);
    window.setView(titleView);
    
    sf::Texture selectorTexture;
    selectorTexture.loadFromFile(resourcePath() + "Selector.png");
    sf::Sprite selectorSprite;
    selectorSprite.setTexture(selectorTexture);
    sf::Texture nextLevelTexture;
    nextLevelTexture.loadFromFile(resourcePath() + "NewGame.png");
    sf::Texture skylineTexture;
    skylineTexture.loadFromFile(resourcePath() + "Skyline.png");
   
    sf::Sprite skyline;
    skyline.setScale(15, 15);
    skyline.setTexture(skylineTexture);
    skyline.setPosition(700 - 350, 200);
    
    sf::Font font;
    font.loadFromFile(resourcePath() + "arcadeclassic.regular.ttf");
    sf::Text newGameText;
    sf::Text options;
    
    
    
    selectorSprite.setPosition(600, 706);
    int selectorPosition = 0;
    
    while (window.isOpen())
    {
        sf::Event event;
    while (window.pollEvent(event))
    {
        // Close window: exit
        if (event.type == sf::Event::Closed) {
            window.close();
            return 0;
        }
        if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::Return){
            if(selectorPosition == 0){
                return 1;
            }
            if(selectorPosition == 1){
                return 2;
            }
        }
        if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::Down){
            selectorPosition += 1;
            if(selectorPosition == 2){
                selectorPosition = 1;
            }
        }
        
        if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::Up){
            selectorPosition -= 1;
            if(selectorPosition == -1){
                selectorPosition = 0;
            }
        }
    }
    
        window.clear(sf::Color::Black);
        
        selectorSprite.setPosition(600, 706 + selectorPosition * 50);
        
        newGameText.setPosition(620, 690);
        newGameText.setFont(font);
        newGameText.setString("New Game");
        
        options.setPosition(620, 690 + 50);
        options.setFont(font);
        options.setString("Options");
        
        window.draw(newGameText);
        window.draw(newGameText);
        window.draw(options);
        
        window.draw(skyline);
        
        window.draw(selectorSprite);
        
        
        window.display();

    }
    
    
}
int showEndScreen(sf::RenderWindow &window, int score){
    sf::View titleView;
    titleView.setCenter(0 + window.getSize().x/2, 0 + window.getSize().y/2);
    titleView.setSize(window.getSize().x, window.getSize().y);
    window.setView(titleView);
    
    sf::Texture selectorTexture;
    selectorTexture.loadFromFile(resourcePath() + "Selector.png");
    sf::Sprite selectorSprite;
    selectorSprite.setTexture(selectorTexture);
    sf::Texture nextLevelTexture;
    nextLevelTexture.loadFromFile(resourcePath() + "NewGame.png");

    
    
    sf::Font font;
    font.loadFromFile(resourcePath() + "arcadeclassic.regular.ttf");
    sf::Text newGameText;
   
    std::ifstream inf(resourcePath() + "Sample.dat");
    std::string string;
    getline(inf,string);
    sf::Text record;
    record.setFont(font);
    if(inf){
        record.setString("High Score  " + string + "\n" + "Score  " + std::to_string(score));
    }else{
        record.setString("High Score  0 \nScore  " + std::to_string(score));
    }
    record.setPosition(600, 400);

    inf.close();

    int selectorPosition = 0;
    selectorSprite.setPosition(600, 603);
    sf::Text options;
    
    sf::Text mainMenu;
    
    
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Close window: exit
            if (event.type == sf::Event::Closed || (event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::Escape)) {
                window.close();
                return 0;
            }
            if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::Return){
                if(selectorPosition == 0){
                    return 1;
                }
                if(selectorPosition == 2){
                    return 2;
                }
                if(selectorPosition == 1){
                    return 3;
                }
                
            }
            if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::Down){
                selectorPosition += 1;
                if(selectorPosition == 3){
                    selectorPosition = 2;
                }
            }
            
            if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::Up){
                selectorPosition -= 1;
                if(selectorPosition == -1){
                    selectorPosition = 0;
                }
            }
            
        }
        
        window.clear(sf::Color::Black);
        options.setPosition(620, 590 + 50);
        options.setFont(font);
        options.setString("Options");
        
        selectorSprite.setPosition(600, 603 + 50 * selectorPosition);
        mainMenu.setPosition(620, 590 + 100);
        mainMenu.setString("Main Menu");
        mainMenu.setFont(font);
        
        newGameText.setPosition(620, 590);
        newGameText.setFont(font);
        newGameText.setString("New Game");
        window.draw(record);
        window.draw(newGameText);
        window.draw(selectorSprite);
        window.draw(options);
        window.draw(mainMenu);
        srand(time(0));
        window.display();
        
    }
    
}
int options(sf::RenderWindow &window, int score){
    sf::View titleView;
    titleView.setCenter(0 + window.getSize().x/2, 0 + window.getSize().y/2);
    titleView.setSize(window.getSize().x, window.getSize().y);
    window.setView(titleView);
    
    sf::Texture selectorTexture;
    selectorTexture.loadFromFile(resourcePath() + "Selector.png");
    sf::Sprite selectorSprite;
    selectorSprite.setTexture(selectorTexture);
    sf::Texture nextLevelTexture;
    nextLevelTexture.loadFromFile(resourcePath() + "NewGame.png");
    
    
    
    sf::Font font;
    font.loadFromFile(resourcePath() + "arcadeclassic.regular.ttf");
    sf::Text newGameText;
    
    int selectorPosition = 0;
    selectorSprite.setPosition(600, 303);
    sf::Text options;
    
    sf::Text mainMenu;
    
    
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            // Close window: exit
            if (event.type == sf::Event::Closed) {
                window.close();
                return 0;
            }
            if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::Return){
                if(selectorPosition == 2){
                    return 2;
                }
            }
            if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::Down){
                selectorPosition += 1;
                if(selectorPosition == 3){
                    selectorPosition = 2;
                }
            }
            
            if(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Key::Up){
                selectorPosition -= 1;
                if(selectorPosition == -1){
                    selectorPosition = 0;
                }
            }
            
        }
        
        window.clear(sf::Color::Black);
        options.setPosition(620, 590 + 50);
        options.setFont(font);
        options.setString("Framerate");
        selectorSprite.setPosition(600, 603 + 50 * selectorPosition);
        mainMenu.setPosition(620, 590 + 100);
        mainMenu.setString("Exit");
        mainMenu.setFont(font);
        newGameText.setPosition(620, 590);
        newGameText.setFont(font);
        newGameText.setString("Vsync");
        window.draw(newGameText);
        window.draw(selectorSprite);
        window.draw(options);
        window.draw(mainMenu);
        srand(time(0));
        window.display();
        
    }

}
int main(int, char const**)
{
    
    sf::RenderWindow window(sf::VideoMode(1400, 851), "SkyLines", sf::Style::Close);
    window.setMouseCursorVisible(false);
    window.setKeyRepeatEnabled(false);
    int mode = 0;
    if(mode){
        window.setVerticalSyncEnabled(true);
    }else{
        window.setVerticalSyncEnabled(true);
    }
    while(1){
    int showTitleReturn = showTitle(window);
    if(!showTitleReturn){
        return EXIT_SUCCESS;
    }
    if(showTitleReturn == 2){
        options(window, 0);
        continue;
    }
    int doubleBreak = 0;
    
    bool gameClosed = 0;
    while (gameClosed != 1){
        int score = 0;
        if(doubleBreak){
            break;
        }
        
        gameClosed = startLevel(window, score,mode);
        while(1){
        int showEndScreenReturn = showEndScreen(window, score);
        
        if(!showEndScreenReturn){
            return EXIT_SUCCESS;
        }
        if(showEndScreenReturn == 1){
            break;
        }
        if(showEndScreenReturn == 3){
            options(window, score);
            break;
        }
        if(showEndScreenReturn == 2){
            doubleBreak = 1;
            break;
        }
    }
    }
    }
    return EXIT_SUCCESS;
    

}


