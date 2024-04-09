#ifndef __MAP_H__
#define __MAP_H__

#include "Module.h"
#include "List.h"
#include "Point.h"
#include "PQueue.h"
#include "DynArray.h"
#include "Pathfinding.h"
#include "Box2D/Box2D/Box2D.h"
#include "PugiXml\src\pugixml.hpp"
#include "Entity.h"

#define MAX_TILE_AMOUNT 96

class Entity;

// L09: DONE 2: Define a property to store the MapType and Load it from the map
enum MapOrientation
{
    ORTOGRAPHIC = 0,
    ISOMETRIC
};

enum TileType {
    SNOW,
    WOODS,
    WATER
};

struct Tile {
private:
    b2Color color = b2Color(1, 0, 0, 1);

public:

    Tile(b2Color col, TileType type) {
        
        this->type = type;

        switch (type) {
        case SNOW:
            color = b2Color(1, 1, 1, 0.6f);
            break;
        case WOODS:
            color = b2Color(0, 1,0, 0.6f);
            break;
        case WATER:
            color = b2Color(0, 0, 1, 0.6f);
            break;
        }

    }

    Entity* myEntity = nullptr;

    b2Color GetColor() {
        if (myEntity == nullptr) {
            return color;
        }
        else {
            return b2Color(0.6f, 0.2f, 0.4f, 0.6f);
        }
    }
    

    float evasion;

    TileType type;




};


// L05: DONE 2: Create a struct to hold information for a TileSet
// Ignore Terrain Types and Tile Types for now, but we want the image!
struct TileSet
{
    int firstgid;
    SString name;
    int tilewidth;
    int tileheight;
    int spacing;
    int margin;
    int tilecount;
    int columns;

    SDL_Texture* texture;

    

    // L06: DONE 7: Implement the method that receives the gid and returns a Rect

    SDL_Rect GetRect(uint gid) {
        SDL_Rect rect = { 0 };

        int relativeIndex = gid - firstgid;
        rect.w = tilewidth;
        rect.h = tileheight;
        rect.x = margin + (tilewidth + spacing) * (relativeIndex % columns);
        rect.y = margin + (tileheight + spacing) * (relativeIndex / columns);

        return rect;
    }

   

    // Include a method to return a selected tileStruct

};



// L08: DONE 5: Add attributes to the property structure
struct Properties
{
    struct Property
    {
        SString name;
        bool value; //We assume that we are going to work only with bool for the moment
    };

    List<Property*> propertyList;

    ~Properties()
    {
        //...
        ListItem<Property*>* property;
        property = propertyList.start;

        while (property != NULL)
        {
            RELEASE(property->data);
            property = property->next;
        }

        propertyList.Clear();
    }

    // L08: DONE 7: Method to ask for the value of a custom property
    Property* GetProperty(const char* name);

};

// L05: DONE 1: Create a struct needed to hold the information to Map node
struct MapLayer
{
    // L06: DONE 1: Add the info to the MapLayer Struct
    int id;
    SString name;
    int width;
    int height;
    uint* tiles;
    Properties properties;

    // matrix of tiles
    Tile* myTiles[MAX_TILE_AMOUNT][MAX_TILE_AMOUNT] = { nullptr };

    // L06: DONE 6: Short function to get the gid value of x,y
    uint Get(int x, int y) const
    {
        return tiles[(y * width) + x];
    }

    Tile* GetTile(uint gid) {

        int row = gid / width;
        int   column = gid % height;

        return myTiles[row][column];

    }
};

struct MapData
{
    int width;
    int height;
    int tilewidth;
    int tileheight;
    List<TileSet*> tilesets;

    // L09: DONE 2: Define a property to store the MapType and Load it from the map
    MapOrientation orientation; 

    // L06: DONE 2: Add a list/array of layers to the map
    List<MapLayer*> layers;
};

class Map : public Module
{
public:

    Map(bool isActive);

    // Destructor
    virtual ~Map();

    // Called before render is available
    bool Awake(pugi::xml_node config);

    // Called before the first frame
    bool Start();

    // Called each loop iteration
    bool Update(float dt);

    // Called before quitting
    bool CleanUp();

    // Load new map
    bool Load(SString mapFileName);

    // L06: DONE 8: Create a method that translates x,y coordinates from map positions to world positions
    iPoint MapToWorld(int x, int y) const;

    // L09: DONE 5: Add method WorldToMap to obtain  map coordinates from screen coordinates 
    iPoint WorldToMap(int x, int y);

    // L08: DONE 2: Implement function to the Tileset based on a tile id
    TileSet* GetTilesetFromTileId(int gid) const;

    // L06: DONE 6: Load a group of properties 
    bool LoadProperties(pugi::xml_node& node, Properties& properties);

    // L13: Create navigation map for pathfinding
    void CreateNavigationMap(int& width, int& height, uchar** buffer) const;

    TileType SetTileType(int gid);

    int GetTileWidth();
    int GetTileHeight();


public: 
    SString name;
    SString path;
    PathFinding* pathfinding;

    SDL_Rect minimap = SDL_Rect{ 0,0, 150,150 };
    bool showMinimap = false;

    SDL_Texture* miniMapTex = nullptr;


    Tile* myTiles[MAX_TILE_AMOUNT][MAX_TILE_AMOUNT] = { nullptr };


private:
    // L05: DONE 1: Declare a variable data of the struct MapData
    MapData mapData;
    bool mapLoaded;
    MapLayer* navigationLayer;
    int blockedGid = 49; //!!!! make sure that you assign blockedGid according to your map

    pugi::xml_node myNode;


 };

#endif // __MAP_H__