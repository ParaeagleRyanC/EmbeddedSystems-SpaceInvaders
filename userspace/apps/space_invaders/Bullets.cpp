#include "Bullets.h"




Bullets::Bullets() {

}


// Create new bullets
void Bullets::newPlayerBullet(Tank *tank) {

}

void Bullets::newEnemyBullet(Alien *alien) {

}


// Kill this bullet (deallocate and remove, updating playerBullet or
// enemyBullets)
void Bullets::kill(Bullet *bullet) {

}

// Tick the bullets, and return whether something moved.
bool Bullets::tick() {
    return false;
}