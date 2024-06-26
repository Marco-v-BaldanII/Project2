
#include "App.h"
#include "Render.h"
#include "Textures.h"
#include "Map.h"
#include "Physics.h"
#include "Window.h"
#include "Scene.h"
#include "Optick/include/optick.h"
#include "GuiManager.h"
#include "Defs.h"
#include "Log.h"
#include "BattleScene.h"
#include <math.h>
#include "SDL_image/include/SDL_image.h"
#include <algorithm>
#include "../ItemManager.h"

Map::Map(bool isActive) : Module(isActive), mapLoaded(false)
{
    name.Create("map");
}

// Destructor
Map::~Map()
{}

// Called before render is available
bool Map::Awake(pugi::xml_node config)
{
    LOG("Loading Map Parser");
    myNode = config;
    bool ret = true;

    return ret;
}

bool Map::Start() {

    myNode = app->win->myNode.next_sibling("map");

    //Calls the functon to load the map, make sure that the filename is assigned
    SString mapPath = path;
    // CHANGE poner el en config
    if (level == 0)
    mapPath = myNode.child("lvl1").attribute("path").as_string();
    else if (level == 1)
    mapPath = myNode.child("lvl2").attribute("path").as_string();


    Load(mapPath);

    miniMapTex = app->tex->Load(myNode.child("minimap").attribute("path").as_string());

    //Initialize pathfinding 
    pathfinding = new PathFinding();
    
    //Initialize the navigation map
    uchar* navigationMap = NULL;
    CreateNavigationMap(mapData.width, mapData.height, &navigationMap);
    pathfinding->SetNavigationMap((uint)mapData.width, (uint)mapData.height, navigationMap);
    RELEASE_ARRAY(navigationMap);

    return true;
}

bool Map::Update(float dt)
{

    OPTICK_EVENT("Map::Update");
    bool ret = true;

    if(mapLoaded == false)
        return false;

    ListItem<MapLayer*>* mapLayer; 
    mapLayer = mapData.layers.start;


    CameraRestrictions();

    // L06: DONE 5: Prepare the loop to draw all tiles in a layer + DrawTexture()

    // iterates the layers in the map
    while (mapLayer != NULL) {
        //Check if the property Draw exist get the value, if it's true draw the lawyer
        if (mapLayer->data->properties.GetProperty("Draw") != NULL && mapLayer->data->properties.GetProperty("Draw")->value) {
            //iterate all tiles in a layer

            SDL_Rect const camera = app->render->camera;

            iPoint cameraPos = iPoint(0, 0);
            iPoint cameraSize = iPoint(mapLayer->data->width, mapLayer->data->height);

            // Map drawing optimization to only draw portion visible by the camera
        
                cameraPos = WorldToMap(-camera.x / app->win->GetScale(), ((camera.y) * -1) / app->win->GetScale());
                cameraSize = iPoint(16, 14);

            for (int x = cameraPos.x; x < mapData.width; x++) {
                for (int y = cameraPos.y; y < mapData.height; y++) {


                    //Get the gid from tile
                    int gid = mapLayer->data->Get(x, y);

                    //L08: DONE 3: Obtain the tile set using GetTilesetFromTileId
                    //Get the Rect from the tileSetTexture;
                    TileSet* tileSet = GetTilesetFromTileId(gid);
                    SDL_Rect tileRect = tileSet->GetRect(gid);
                    //SDL_Rect tileRect = mapData.tilesets.start->data->GetRect(gid); // (!!) we are using always the first tileset in the list

                    //Get the screen coordinates from the tile coordinates
                    iPoint mapCoord = MapToWorld(x, y);

                    // L06: DONE 9: Complete the draw function
                    app->render->DrawTexture(tileSet->texture, mapCoord.x, mapCoord.y, &tileRect);

                    if (showMinimap) {
                        if (mapLayer->data->myTiles[x][y] != nullptr) app->render->DrawRectangle(SDL_Rect{ mapCoord.x, mapCoord.y , mapData.tilewidth, mapData.tileheight }, mapLayer->data->myTiles[x][y]->GetColor(), true, true);
                    }

                    if (drawGrid) {

                        app->render->DrawRectangle(SDL_Rect{ mapCoord.x, mapCoord.y , mapData.tilewidth, mapData.tileheight },b2Color(0,0,0,0.8f) , false, true);

                    }

                    // testing if the tile's associated color is drawn
                   /* if (mapLayer->data->myTiles[0][0] != nullptr) {
                        
                    }*/
                }
            }
        }
        mapLayer = mapLayer->next;
    }



    if (app->input->GetKey(SDL_SCANCODE_M) == KEY_DOWN) {

        showMinimap = !showMinimap;

    }

    if (showMinimap) {

        app->render->DrawTexture(miniMapTex, (350/2) - (app->render->camera.x/3), (192/2) - (app->render->camera.y/3), &minimap);
    }

    return ret;
}


void Map::CameraRestrictions() {
    //Limit the camera
    if (app->render->camera.x > 0) {
        app->render->camera.x = 0;

    }
    if (app->render->camera.y > 0) { app->render->camera.y = 0; }
    if (app->render->camera.x < -39 * TILE_RESOLUTION * 3) { app->render->camera.x = -39 * TILE_RESOLUTION * 3; }
    if (app->render->camera.y < -35 * TILE_RESOLUTION * 3) {
        app->render->camera.y = -35 * TILE_RESOLUTION * 3;
    }
}

// L08: DONE 2: Implement function to the Tileset based on a tile id
TileSet* Map::GetTilesetFromTileId(int gid) const
{
    TileSet* set = NULL;

    ListItem<TileSet*>* tileSet;
    tileSet = mapData.tilesets.start;
    while (tileSet != NULL) {
        set = tileSet->data;
        if (gid >= tileSet->data->firstgid && gid < (tileSet->data->firstgid + tileSet->data->tilecount)) break;
        tileSet = tileSet->next;
    }

    return set;
}

// Called before quitting
bool Map::CleanUp()
{
    LOG("Unloading map");

    // Destroy entities

    //app->entityManager->Disable();
    app->itemManager->Disable();

    ListItem<TileSet*>* item;
    item = mapData.tilesets.start;

    //chickens = false;

    while (item != NULL)
    {
        RELEASE(item->data);
        item = item->next;
    }
    mapData.tilesets.Clear();

    // Remove all layers
    ListItem<MapLayer*>* layerItem;
    layerItem = mapData.layers.start;

    while (layerItem != NULL)
    {
        
        RELEASE(layerItem->data);
        layerItem = layerItem->next;
    }



    return true;
}

// Load new map
bool Map::Load(SString mapFileName)
{
    bool ret = true;


    // L05: DONE 3: Implement LoadMap to load the map properties
    // retrieve the paremeters of the <map> node and save it into map data

    pugi::xml_document mapFileXML;
    pugi::xml_parse_result result = mapFileXML.load_file(mapFileName.GetString());

    if(result == NULL)
    {
        LOG("Could not load map xml file %s. pugi error: %s", mapFileName, result.description());
        ret = false;
    }
    else {

        //Fill mapData variable
        mapData.width = mapFileXML.child("map").attribute("width").as_int();
        mapData.height = mapFileXML.child("map").attribute("height").as_int();
        mapData.tilewidth = mapFileXML.child("map").attribute("tilewidth").as_int();
        mapData.tileheight = mapFileXML.child("map").attribute("tileheight").as_int();

        // L09: DONE 2: Define a property to store the MapType and Load it from the map
        SString orientationStr = mapFileXML.child("map").attribute("orientation").as_string();
        if (orientationStr == "orthogonal") {
            mapData.orientation = MapOrientation::ORTOGRAPHIC;
        }
        else if (orientationStr == "isometric") {
            mapData.orientation = MapOrientation::ISOMETRIC;
        }
        else {
            LOG("Map orientation not found");
            ret = false;
        }

        TileSet* lastTileSet = nullptr;


        // L05: DONE 4: Implement the LoadTileSet function to load the tileset properties
       // Iterate the Tileset
        int i = 0;
        for (pugi::xml_node tilesetNode = mapFileXML.child("map").child("tileset"); tilesetNode != NULL; tilesetNode = tilesetNode.next_sibling("tileset")) {

            TileSet* tileset = new TileSet();

            //Load Tileset attributes
            tileset->name = tilesetNode.attribute("name").as_string();
            tileset->firstgid = tilesetNode.attribute("firstgid").as_int();
            tileset->margin = tilesetNode.attribute("margin").as_int();
            tileset->spacing = tilesetNode.attribute("spacing").as_int();
            tileset->tilewidth = tilesetNode.attribute("tilewidth").as_int();
            tileset->tileheight = tilesetNode.attribute("tileheight").as_int();
            tileset->columns = tilesetNode.attribute("columns").as_int();
            tileset->tilecount = tilesetNode.attribute("tilecount").as_int();

            //Load Tileset image
            SString mapTex = path;
            mapTex += tilesetNode.child("image").attribute("source").as_string();

            string path = mapTex.GetString();
            path.erase(0, 2);
            string finalPath = "Assets" + path;

            tileset->texture = app->tex->Load(finalPath.c_str());

            mapData.tilesets.Add(tileset);

            if (i == 0) {
                lastTileSet = tileset;
            }


        }

        // L06: DONE 3: Iterate all layers in the TMX and load each of them
        for (pugi::xml_node layerNode = mapFileXML.child("map").child("layer"); layerNode != NULL; layerNode = layerNode.next_sibling("layer")) {


            //Load the attributes and saved in a new MapLayer
            MapLayer* mapLayer = new MapLayer();
            mapLayer->id = layerNode.attribute("id").as_int();
            mapLayer->name = layerNode.attribute("name").as_string();
            mapLayer->width = layerNode.attribute("width").as_int();
            mapLayer->height = layerNode.attribute("height").as_int();

            //L08: DONE 6 Call Load Layer Properties
            LoadProperties(layerNode, mapLayer->properties);

            //Reserve the memory for the data 
            mapLayer->tiles = new uint[mapLayer->width * mapLayer->height];
            memset(mapLayer->tiles, 0, mapLayer->width * mapLayer->height);

            //Iterate over all the tiles and assign the values in the data array
            // Assign Tiles
            int i = 0;
            for (pugi::xml_node tileNode = layerNode.child("data").child("tile"); tileNode != NULL; tileNode = tileNode.next_sibling("tile")) {
                mapLayer->tiles[i] = tileNode.attribute("gid").as_uint();
                i++;
            }

            i = 0; int j = 0;
            for (pugi::xml_node tileNode = layerNode.child("data").child("tile"); tileNode != NULL; tileNode = tileNode.next_sibling("tile")) {
                int gid = tileNode.attribute("gid").as_uint();


                // acess  in myTiles matrix

                TileType type = SetTileType(mapLayer->tiles[i]);


                mapLayer->myTiles[i % MAX_TILE_AMOUNT][j] = new Tile(b2Color(1, 1, 1, 1), type);

                myTiles[i % MAX_TILE_AMOUNT][j] = mapLayer->myTiles[i % MAX_TILE_AMOUNT][j];
                myTiles[i % MAX_TILE_AMOUNT][j]->myItem = nullptr;

                if (i != 0 && i % mapLayer->width == 0) {
                    j++;

                }


                i++;

            }

            
        

        //add the layer to the map
        mapData.layers.Add(mapLayer);
        
        // Save the values of the map layer matrix to a globally accesible matrix that othe modlues might consult
       /* for (int i = 0; i < MAX_TILE_AMOUNT; ++i) {
            for (int j = 0; j < 45; ++j) {
                myTiles[i][j] = mapLayer->myTiles[i][j];
            }
        }*/


    }


        // L07 DONE 3: Create colliders      
        // L07 DONE 7: Assign collider type
        // Later you can create a function here to load and create the colliders from the map
        
        // CALL TO CREATE COLLIDERS FROM MAP

          // L05: DONE 5: LOG all the data loaded iterate all tilesetsand LOG everything
        if (ret == true)
        {
            LOG("Successfully parsed map XML file :%s", mapFileName.GetString());
            LOG("width : %d height : %d", mapData.width, mapData.height);
            LOG("tile_width : %d tile_height : %d", mapData.tilewidth, mapData.tileheight);

            LOG("Tilesets----");

            ListItem<TileSet*>* tileset;
            tileset = mapData.tilesets.start;
            while (tileset != NULL) {
                //iterate the tilesets
                LOG("name : %s firstgid : %d",tileset->data->name.GetString(), tileset->data->firstgid);
                LOG("tile width : %d tile height : %d", tileset->data->tilewidth, tileset->data->tileheight);
                LOG("spacing : %d margin : %d", tileset->data->spacing, tileset->data->margin);
                tileset = tileset->next;
            }

            LOG("Layers----");

            ListItem<MapLayer*>* mapLayer;
            mapLayer = mapData.layers.start;

            while (mapLayer != NULL) {
                LOG("id : %d name : %s", mapLayer->data->id, mapLayer->data->name.GetString());
                LOG("Layer width : %d Layer height : %d", mapLayer->data->width, mapLayer->data->height);
                mapLayer = mapLayer->next;
            }
        }

        // Find the navigation layer
        ListItem<MapLayer*>* mapLayerItem;
        mapLayerItem = mapData.layers.start;
        navigationLayer = mapLayerItem->data;

        //Search the layer in the map that contains information for navigation
        while (mapLayerItem != NULL) {
            if (mapLayerItem->data->properties.GetProperty("Navigation") != NULL && mapLayerItem->data->properties.GetProperty("Navigation")->value) {
                navigationLayer = mapLayerItem->data;
                break;
            }
            mapLayerItem = mapLayerItem->next;
        }

        //Resets the map
        if (mapFileXML) mapFileXML.reset();
    }

    mapLoaded = ret;
    return ret;
}

// L06: DONE 8: Create a method that translates x,y coordinates from map positions to world positions
iPoint Map::MapToWorld(int x, int y) const
{
    iPoint ret;

    // L09: DONE 3: Get the screen coordinates of tile positions for isometric maps 
    if (mapData.orientation == MapOrientation::ORTOGRAPHIC) {
        ret.x = x * mapData.tilewidth;
        ret.y = y * mapData.tileheight;
    }

    if (mapData.orientation == MapOrientation::ISOMETRIC) {
        ret.x = (x- y )* (mapData.tilewidth / 2);
        ret.y = (x + y) *( mapData.tileheight / 2);
    }

    return ret;
}

// L08: DONE 6: Load a group of properties from a node and fill a list with it
bool Map::LoadProperties(pugi::xml_node& node, Properties& properties)
{
    bool ret = false;

    for (pugi::xml_node propertieNode = node.child("properties").child("property"); propertieNode; propertieNode = propertieNode.next_sibling("property"))
    {
        Properties::Property* p = new Properties::Property();
        p->name = propertieNode.attribute("name").as_string();
        p->value = propertieNode.attribute("value").as_bool(); // (!!) I'm assuming that all values are bool !!

        properties.propertyList.Add(p);
    }

    return ret;
}

void Map::DrawAdjacents(iPoint tilePos, int range) {

 
        iPoint mapCoord = MapToWorld(tilePos.x -1, tilePos.y);
        if (mapCoord.x > 0) {
            app->render->DrawRectangle(SDL_Rect{ mapCoord.x, mapCoord.y , mapData.tilewidth, mapData.tileheight }, b2Color(1, 0, 0, 0.5f), true, true);
        }
         mapCoord = MapToWorld(tilePos.x +1, tilePos.y);
        if (mapCoord.x > 0) /* Change the limit*/ {
            app->render->DrawRectangle(SDL_Rect{ mapCoord.x, mapCoord.y , mapData.tilewidth, mapData.tileheight }, b2Color(1, 0, 0, 0.5f), true, true);
        }
         mapCoord = MapToWorld(tilePos.x , tilePos.y -1);
        if (mapCoord.y > 0) {
            app->render->DrawRectangle(SDL_Rect{ mapCoord.x, mapCoord.y , mapData.tilewidth, mapData.tileheight }, b2Color(1, 0, 0, 0.5f), true, true);
        }
         mapCoord = MapToWorld(tilePos.x, tilePos.y + 1);
        if (mapCoord.y > 0)/* also change*/ {
            app->render->DrawRectangle(SDL_Rect{ mapCoord.x, mapCoord.y , mapData.tilewidth, mapData.tileheight }, b2Color(1, 0, 0, 0.5f), true, true);
        }
        if (range == 2) {

            if (mapCoord.x > 1) {
                mapCoord = MapToWorld(tilePos.x - 2, tilePos.y);
                app->render->DrawRectangle(SDL_Rect{ mapCoord.x, mapCoord.y , mapData.tilewidth, mapData.tileheight }, b2Color(1, 0, 0, 0.5f), true, true);
            }
            mapCoord = MapToWorld(tilePos.x + 2, tilePos.y);
            if (mapCoord.x > 1) /* Change the limit*/ {
                app->render->DrawRectangle(SDL_Rect{ mapCoord.x, mapCoord.y , mapData.tilewidth, mapData.tileheight }, b2Color(1, 0, 0, 0.5f), true, true);
            }
            mapCoord = MapToWorld(tilePos.x, tilePos.y - 2);
            if (mapCoord.y > 1) {
                app->render->DrawRectangle(SDL_Rect{ mapCoord.x, mapCoord.y , mapData.tilewidth, mapData.tileheight }, b2Color(1, 0, 0, 0.5f), true, true);
            }
            mapCoord = MapToWorld(tilePos.x, tilePos.y + 2);
            if (mapCoord.y > 1)/* also change*/ {
                app->render->DrawRectangle(SDL_Rect{ mapCoord.x, mapCoord.y , mapData.tilewidth, mapData.tileheight }, b2Color(1, 0, 0, 0.5f), true, true);
            }



            mapCoord = MapToWorld(tilePos.x-1, tilePos.y - 1);
            if (mapCoord.y > 0 && mapCoord.x > 0) {
                app->render->DrawRectangle(SDL_Rect{ mapCoord.x, mapCoord.y , mapData.tilewidth, mapData.tileheight }, b2Color(1, 0, 0, 0.5f), true, true);
            }
            mapCoord = MapToWorld(tilePos.x +1, tilePos.y + 1);
            if (mapCoord.y > 0 && mapCoord.x > 0)/* change */ {
                app->render->DrawRectangle(SDL_Rect{ mapCoord.x, mapCoord.y , mapData.tilewidth, mapData.tileheight }, b2Color(1, 0, 0, 0.5f), true, true);
            }

            mapCoord = MapToWorld(tilePos.x + 1, tilePos.y - 1);
            if (mapCoord.y > 0 && mapCoord.x > 0) /* change x */ {
                app->render->DrawRectangle(SDL_Rect{ mapCoord.x, mapCoord.y , mapData.tilewidth, mapData.tileheight }, b2Color(1, 0, 0, 0.5f), true, true);
            }

            mapCoord = MapToWorld(tilePos.x -1, tilePos.y + 1);
            if (mapCoord.y > 0 && mapCoord.x > 0) /* change y */ {
                app->render->DrawRectangle(SDL_Rect{ mapCoord.x, mapCoord.y , mapData.tilewidth, mapData.tileheight }, b2Color(1, 0, 0, 0.5f), true, true);
            }

        }
   



}

int Map::DistanceBetweenTiles(iPoint t1, iPoint t2) {

    int distance = abs(t2.x - t1.x) + abs(t2.y - t1.y);

    return distance;
}

// L08: DONE 7: Implement a method to get the value of a custom property
Properties::Property* Properties::GetProperty(const char* name)
{
    ListItem<Property*>* property = propertyList.start;
    Property* p = NULL;

    while (property)
    {
        if (property->data->name == name) {
            p = property->data;
            break;
        }
        property = property->next;
    }

    return p;
}

// L09: DONE 5: Add method WorldToMap to obtain  map coordinates from screen coordinates 
iPoint Map::WorldToMap(int x, int y) {

    iPoint ret(0, 0);

    if (mapData.orientation == MapOrientation::ORTOGRAPHIC) {
        ret.x = x / mapData.tilewidth;
        ret.y = y / mapData.tileheight;
    }

    if (mapData.orientation == MapOrientation::ISOMETRIC) {
        float half_width = mapData.tilewidth / 2;
        float half_height = mapData.tileheight / 2;
        ret.x = int((x / half_width + y / half_height) / 2);
        ret.y = int((y / half_height - (x / half_width)) / 2);
    }

    return ret;
}

int Map::GetTileWidth() {
    return mapData.tilewidth;
}

int Map::GetTileHeight() {
    return mapData.tileheight;
}

TileType Map::SetTileType(int gid) {

  

    if (gid == 14 || gid == 5 || gid ==13|| gid == 7 || gid == 6 || gid == 15 || gid == 23 ||
        gid == 22 || gid == 21 || gid == 53 || gid == 61 || gid == 69 || gid == 62 || gid == 41) {
        return TileType::WOODS;
    }

    else if (gid == 27 ||  gid == 3 || gid == 25 ) { return TileType::WATER; }

    else  { 
        return TileType::SNOW; 
    }



}

// L13: Create navigationMap map for pathfinding
void Map::CreateNavigationMap(int& width, int& height, uchar** buffer) const
{
    bool ret = false;

    //Sets the size of the map. The navigation map is a unidimensional array 
    uchar* navigationMap = new uchar[navigationLayer->width * navigationLayer->height];
    //reserves the memory for the navigation map
    memset(navigationMap, 1, navigationLayer->width * navigationLayer->height);

    for (int x = 0; x < mapData.width; x++)
    {
        for (int y = 0; y < mapData.height; y++)
        {
            //i is the index of x,y coordinate in a unidimensional array that represents the navigation map
            int i = (y * navigationLayer->width) + x;

            //Gets the gid of the map in the navigation layer
            int gid = navigationLayer->Get(x, y);
            
            //If the gid is a blockedGid is an area that I cannot navigate, so is set in the navigation map as 0, all the other areas can be navigated
            //!!!! make sure that you assign blockedGid according to your map
            if (gid == blockedGid) 
                navigationMap[i] = 0;
            else navigationMap[i] = 1;
        }
    }

    *buffer = navigationMap;
    width = mapData.width;
    height = mapData.height;

}

