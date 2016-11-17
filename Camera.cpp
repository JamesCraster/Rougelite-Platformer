//
//  Camera.cpp
//  StealthPlatformer
//
//  Created by James Vaughan Craster on 07/10/2016.
//  Copyright (c) 2016 James Vaughan Craster. All rights reserved.
//

#include "Camera.h"
float lerp (float a, float b, float t) { return a + t * (b - a); }
sf::Vector2f lerp(sf::Vector2f a, sf::Vector2f b, double t){return a + sf::Vector2f(t * (b.x - a.x),t * (b.y - a.y));}




void positionView(sf::View & playerView, sf::Vector2f position, float cameraTime){
    sf::Vector2f originalPosition = playerView.getCenter();
    if(position.x > playerView.getSize().x * 0.5)
    {
        playerView.setCenter(int(lerp(playerView.getCenter().x, position.x,pow(0.000009,cameraTime))), int(playerView.getCenter().y));
        if(playerView.getCenter().x - originalPosition.x < 0){
            playerView.setCenter(originalPosition);
        }
    }  
   
}
