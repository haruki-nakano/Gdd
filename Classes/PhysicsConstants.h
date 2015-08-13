//
//  PhysicsConstants.h
//  Yuzu
//
//  Created by Haruki Nakano on 2015/08/12.
//
//

#ifndef Yuzu_PhysicsConstants_h
#define Yuzu_PhysicsConstants_h

/* Tags */
static const int TAG_PLAYER = 1;
static const int TAG_PLAYER_BULLET = 2;
static const int TAG_OPPOPENT = 3;
static const int TAG_OPPOPENT_BULLET = 4;
static const int TAG_WALL = 5;
static const int TAG_EGG = 6;
static const int TAG_ITEM = 7;
static const int TAG_WEAPON = 8;

/* Categories */
static const int CATEGORY_MASK_PLAYER = 1;
static const int CATEGORY_MASK_BULLET = 2;
static const int CATEGORY_MASK_WALL = 4;
static const int CATEGORY_MASK_EGG = 8;
static const int CATEGORY_MASK_ITEM = 16;

/* Contacts */
static const int CONTACT_MASK_PLAYER = CATEGORY_MASK_BULLET | CATEGORY_MASK_ITEM;
static const int CONTACT_MASK_WALL = CATEGORY_MASK_BULLET;
static const int CONTACT_MASK_BULLET = CATEGORY_MASK_PLAYER | CATEGORY_MASK_WALL | CATEGORY_MASK_EGG;
static const int CONTACT_MASK_EGG = CATEGORY_MASK_BULLET;
static const int CONTACT_MASK_ITEM = CATEGORY_MASK_PLAYER;

/* Collisions */
static const int COLLISION_MASK_PLAYER = CATEGORY_MASK_WALL | CATEGORY_MASK_EGG;
static const int COLLISION_MASK_WALL = CATEGORY_MASK_PLAYER | CATEGORY_MASK_EGG;
static const int COLLISION_MASK_BULLET = 0;
static const int COLLISION_MASK_EGG = CATEGORY_MASK_PLAYER | CATEGORY_MASK_WALL;
static const int COLLISION_MASK_ITEM = 0;

#endif
