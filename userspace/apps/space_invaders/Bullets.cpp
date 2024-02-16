#include "Bullets.h"
#include "Tank.h"
#include "Alien.h"
#include "Globals.h"
#include "Bullet.h"

Bullets::Bullets(){}

void Bullets::newPlayerBullet(Tank *tank)
{
    playerBullet = new Bullet(tank);
    playerBullet->resurrect();
}

void Bullets::newEnemyBullet(Alien *alien)
{
    srand((unsigned) time(NULL));
    int random = rand();
    sprite_bullet_type_t bulletd;
    sprite_bullet_type_t bulletu;
    if(random%2)
    {
        bulletu = SPRITE_BULLET_ALIEN1_UP;
        bulletd = SPRITE_BULLET_ALIEN1_DOWN;
    }
    else
    {
        bulletu = SPRITE_BULLET_ALIEN2_UP;
        bulletd = SPRITE_BULLET_ALIEN2_DOWN;
    }
    Bullet *newAlienBullet = new Bullet(
                        alien,
                        Globals::getSprites().getBullet(bulletd),
                        Globals::getSprites().getBullet(bulletu));
    enemyBullets.push_back(newAlienBullet);
    
    newAlienBullet->resurrect();
}
void Bullets::kill(Bullet *bullet)
{
    bullet->killBullet();
}

bool Bullets::tick()
{
    playerBullet->tick();
    // std::list<Bullet *>::iterator it;
    // for(it = enemyBullets.begin(); it != enemyBullets.end();it++)
    // {
    //     (*it)->tick();
    // }
    return true;
}