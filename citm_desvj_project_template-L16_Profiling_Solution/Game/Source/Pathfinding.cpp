#include "App.h"
#include "Map.h"
#include "PathFinding.h"
#include "../ItemManager.h"
#include "Defs.h"
#include "Log.h"

PathFinding::PathFinding() : lastPath(DEFAULT_PATH_LENGTH)
{
	map = NULL;
	width = 0;
	height = 0;
}

// Destructor
PathFinding::~PathFinding()
{
	RELEASE_ARRAY(map);
}

// Called before quitting
bool PathFinding::CleanUp()
{
	LOG("Freeing pathfinding library");

	lastPath.Clear();
	RELEASE_ARRAY(map);

	return true;
}

// Sets up the navigation map
void PathFinding::SetNavigationMap(uint w, uint h, uchar* data)
{
	width = w;
	height = h;

	RELEASE_ARRAY(map);
	map = new uchar[width*height];
	memcpy(map, data, width*height);
}

// Utility: return true if pos is inside the map boundaries
bool PathFinding::CheckBoundaries(const iPoint& pos) const
{
	return (pos.x >= 0 && pos.x <= (int)width &&
			pos.y >= 0 && pos.y <= (int)height);
}

// Utility: returns true is the tile is walkable
bool PathFinding::IsWalkable(const iPoint& pos) const
{
	uchar walkId = GetTileAt(pos);
	bool isWalkable = walkId != INVALID_WALK_CODE && walkId > 0;
	return  isWalkable;
}

// Utility: return the walkability value of a tile
uchar PathFinding::GetTileAt(const iPoint& pos) const
{
	if(CheckBoundaries(pos))
		return map[(pos.y*width) + pos.x];

	return INVALID_WALK_CODE;
}

// To request all tiles involved in the last generated path
const DynArray<iPoint>* PathFinding::GetLastPath() const
{ 
	return &lastPath;
}

// To request all tiles involved in the last generated path
void PathFinding::ClearLastPath()
{
	lastPath.Clear();
}


// ----------------------------------------------------------------------------------
// Actual A* algorithm: return number of steps in the creation of the path or -1 ----
// ----------------------------------------------------------------------------------
int PathFinding::CreatePath(const iPoint& origin, const iPoint& destination)
{
	//int ret = -1;
	//int iterations = 0;

	//// L13: DONE 1: if origin or destination are not walkable, return -1
	//if (IsWalkable(origin) && IsWalkable(destination))
	//{
	//	// L13: DONE 2: Create two lists: frontier, visited
	//	PathList frontier;
	//	PathList visited;
	//	
	//	// Create a PathNode with the origin position and add it to frontier list
	//	frontier.list.add(PathNode(0, 0, origin, nullptr));

	//	// Iterate while we have node in the frontier list
	//	while (frontier.list.count() > 0)
	//	{
	//		// L13: DONE 3: Get the lowest score cell from frontier list and delete it from the frontier list
	//		// Keep a reference to the node before deleting the node from the list
	//		p2ListItem<PathNode>* lowest = frontier.GetNodeLowestScore();
	//		p2ListItem<PathNode>* node = new p2ListItem<PathNode>(*lowest);
	//		frontier.list.del(lowest);


	//		// L13: DONE 6: If we just added the destination, we are done!
	//		if (node->data.pos == destination)
	//		{
	//			lastPath.Clear();

	//			// Backtrack to create the final path
	//			// Use the Pathnode::parent and Flip() the path when you are finish
	//			const PathNode* pathNode = &node->data;

	//			while (pathNode)
	//			{
	//				lastPath.PushBack(pathNode->pos);
	//				pathNode = pathNode->parent;
	//			}

	//			lastPath.Flip();
	//			ret = lastPath.Count();
	//			LOG("Created path of %d steps in %d iterations", ret, iterations);
	//			break;
	//		}

	//		// L13: DONE 4: Fill a list of all adjancent nodes. 
	//		// use the FindWalkableAdjacents function
	//		PathList adjacent;
	//		node->data.FindWalkableAdjacents(adjacent);

	//		// L13: DONE 5: Iterate adjancent nodes:
	//		// If it is a better path, Update the parent
	//		p2ListItem<PathNode>* neighbourg = adjacent.list.start;
	//		while (neighbourg != NULL)
	//		{
	//			// ignore nodes in the visited list
	//			if (visited.Find(neighbourg->data.pos) == NULL) {

	//				//add the neighbourg to the visited list
	//				visited.list.add(neighbourg->data);

	//				// If the neighbourgh is NOT found in the frontier list, calculate its F and add it to the frontier list
	//				p2ListItem<PathNode>* neighbourgInFrontier = frontier.Find(neighbourg->data.pos);
	//				if (neighbourgInFrontier == NULL)
	//				{
	//					neighbourg->data.CalculateF(destination);
	//					frontier.list.add(neighbourg->data);
	//				}
	//				else
	//				{
	//					// If it is already in the frontier list, check if it is a better path (compare G)
	//					if (neighbourgInFrontier->data.g > neighbourg->data.g + 1)
	//					{
	//						neighbourgInFrontier->data.parent = neighbourg->data.parent;
	//						neighbourgInFrontier->data.CalculateF(destination);
	//					}
	//				}
	//			}

	//			neighbourg = neighbourg->next;
	//		}
	//		++iterations;
	//	}
	//}
	int ret = -1;

	PathList open;
	PathList close;
	PathList adjacents;

	PathNode nodeStart(0, 0, origin, nullptr);

	open.list.add(nodeStart);

	p2ListItem<PathNode>* current = open.list.start;
	p2ListItem<PathNode>* lowest;
	lastPath.Clear();

	if (IsWalkable(origin) && IsWalkable(destination))
	{
		while (open.list.count() > 0)
		{

			lowest = open.GetNodeLowestScore();
			current = close.list.add(lowest->data);
			open.list.del(lowest);

			if (current->data.pos == destination)
			{
				lastPath.Clear();
				// backtrack the path
				const PathNode* lastItem = &current->data;
				while (lastItem)
				{
					lastPath.PushBack(lastItem->pos);

					lastItem = lastItem->parent;
				}
				lastPath.Flip();
				ret = lastPath.Count();
				break;
			}

			current->data.FindWalkableAdjacents(adjacents);

			p2ListItem<PathNode>* adj = adjacents.list.start;
			while (adj != NULL)
			{
				if (close.Find(adj->data.pos) != NULL)
				{
					adj = adj->next;
					continue;
				}

				p2ListItem<PathNode>* adjacentInOpen = open.Find(adj->data.pos);

				if (adjacentInOpen == NULL)
				{
					adj->data.CalculateF(destination);
					open.list.add(adj->data);
				}
				else {

					if (adjacentInOpen->data.g > (adj->data.g + 1))
					{
						adjacentInOpen->data.parent = adj->data.parent;
						adjacentInOpen->data.CalculateF(destination);
					}
				}
				adj = adj->next;
			}
		}
	}
	return ret;
}

// PathList ------------------------------------------------------------------------
// Looks for a node in this list and returns it's list node or NULL
// ---------------------------------------------------------------------------------
p2ListItem<PathNode>* PathList::Find(const iPoint& point) const
{
	p2ListItem<PathNode>* item = list.start;
	while(item)
	{
		if(item->data.pos == point)
			return item;
		item = item->next;
	}
	return item;
}

// PathList ------------------------------------------------------------------------
// Returns the Pathnode with lowest score in this list or NULL if empty
// ---------------------------------------------------------------------------------
p2ListItem<PathNode>* PathList::GetNodeLowestScore() const
{
	p2ListItem<PathNode>* ret = NULL;
	int min = 65535;

	p2ListItem<PathNode>* item = list.end;
	while(item)
	{
		if(item->data.Score() < min)
		{
			min = item->data.Score();
			ret = item;
		}
		item = item->prev;
	}

	return ret;
}

// PathNode -------------------------------------------------------------------------
// Convenient constructors
// ----------------------------------------------------------------------------------
PathNode::PathNode() : g(-1), h(-1), pos(-1, -1), parent(NULL)
{}

PathNode::PathNode(int g, int h, const iPoint& pos, const PathNode* parent) : g(g), h(h), pos(pos), parent(parent)
{}

PathNode::PathNode(const PathNode& node) : g(node.g), h(node.h), pos(node.pos), parent(node.parent)
{}

// PathNode -------------------------------------------------------------------------
// Fills a list (PathList) of all valid adjacent pathnodes
// ----------------------------------------------------------------------------------
uint PathNode::FindWalkableAdjacents(PathList& listToFill) const
{
	iPoint tile;
	uint before = listToFill.list.count();

	// top
	tile.Create(pos.x, pos.y + 1);
	if(app->map->pathfinding->IsWalkable(tile)) listToFill.list.add(PathNode(-1, -1, tile, this));

	// bottom
	tile.Create(pos.x, pos.y - 1);
	if(app->map->pathfinding->IsWalkable(tile)) listToFill.list.add(PathNode(-1, -1, tile, this));

	// left
	tile.Create(pos.x + 1, pos.y);
	if(app->map->pathfinding->IsWalkable(tile)) listToFill.list.add(PathNode(-1, -1, tile, this));

	// right
	tile.Create(pos.x - 1, pos.y);
	if(app->map->pathfinding->IsWalkable(tile)) listToFill.list.add(PathNode(-1, -1, tile, this));

	return listToFill.list.count();
}

// PathNode -------------------------------------------------------------------------
// Calculates this tile score
// ----------------------------------------------------------------------------------
int PathNode::Score() const
{
	return g + h;
}

// PathNode -------------------------------------------------------------------------
// Calculate the F for a specific destination tile
// ----------------------------------------------------------------------------------
int PathNode::CalculateF(const iPoint& destination)
{
	g = parent->g + 1;
	h = pos.DistanceTo(destination);

	return g + h;
}

int PathFinding::CreateVisitedPath(const iPoint& origin, const iPoint& destination)
{
	int ret = -1;

	PathList open;
	PathList close;
	PathList adjacents;

	PathNode nodeStart(0, 0, origin, nullptr);

	open.list.add(nodeStart);

	p2ListItem<PathNode>* current = open.list.start;
	p2ListItem<PathNode>* lowest;
	lastPath.Clear();

	if (IsVisited(origin) && IsVisited(destination))
	{
		while (open.list.count() > 0)
		{

			lowest = open.GetNodeLowestScore();
			current = close.list.add(lowest->data);
			open.list.del(lowest);

			if (current->data.pos == destination)
			{
				lastPath.Clear();
				// backtrack the path
				const PathNode* lastItem = &current->data;

				//check that the last node is empty,
				//if not last item is parent item
				if (!IsTileEmpty(lastItem->pos))
					lastItem = lastItem->parent;

				while (lastItem)
				{
					lastPath.PushBack(lastItem->pos);

					lastItem = lastItem->parent;
				}

				lastPath.Flip();
				ret = lastPath.Count();
				break;
			}

			current->data.FindVisitedAdjacents(adjacents, this);

			p2ListItem<PathNode>* adj = adjacents.list.start;
			while (adj != NULL)
			{
				if (close.Find(adj->data.pos) != NULL)
				{
					adj = adj->next;
					continue;
				}

				p2ListItem<PathNode>* adjacentInOpen = open.Find(adj->data.pos);

				if (adjacentInOpen == NULL)
				{
					adj->data.CalculateF(destination);
					open.list.add(adj->data);
				}
				else {

					if (adjacentInOpen->data.g > (adj->data.g + 1))
					{
						adjacentInOpen->data.parent = adj->data.parent;
						adjacentInOpen->data.CalculateF(destination);
					}
				}
				adj = adj->next;
			}
		}
	}
	return ret;
}

uint PathNode::FindVisitedAdjacents(PathList& listToFill, PathFinding* path) const
{
	iPoint cell;
	uint before = listToFill.list.count();

	// north
	cell.Create(pos.x, pos.y + 1);
	if (path->IsVisited(cell))
		listToFill.list.add(PathNode(-1, -1, cell, this));

	// south
	cell.Create(pos.x, pos.y - 1);
	if (path->IsVisited(cell))
		listToFill.list.add(PathNode(-1, -1, cell, this));

	// east
	cell.Create(pos.x + 1, pos.y);
	if (path->IsVisited(cell))
		listToFill.list.add(PathNode(-1, -1, cell, this));

	// west
	cell.Create(pos.x - 1, pos.y);
	if (path->IsVisited(cell))
		listToFill.list.add(PathNode(-1, -1, cell, this));

	return listToFill.list.count();
}

bool PathFinding::IsVisited(const iPoint& pos) const
{
	p2ListItem<iPoint>* p = visited.start;

	while (p != NULL)
	{
		if (p->data == pos)
			return true;

		p = p->next;
	}

	return false;
}

bool PathFinding::IsTileEmpty(const iPoint& pos) const
{

	//create function later to check the list of all entities
	for (int i = 0; i < app->entityManager->entities.Count(); i++)
	{
		
		
		if (pos == app->entityManager->entities.At(i)->data->tilePos)
		{
			if (app->entityManager->entities.At(i)->data->hp > 0)
			return false;
		}

	}
	for (int i = 0; i < app->itemManager->doors.Count(); i++)
	{


		if (pos == app->itemManager->doors.At(i)->data->tilePos && app->itemManager->doors.At(i)->data->isOpen == false)
		{
			return false;
		}

	}
	for (int i = 0; i < app->itemManager->levers.Count(); i++)
	{


		if (pos == app->itemManager->levers.At(i)->data->tilePos )
		{
			return false;
		}

	}
	return true;
}

bool PathFinding::IsLeverThere(const iPoint& pos) const
{
	for (int i = 0; i < app->itemManager->levers.Count(); i++)
	{
		if (pos == app->itemManager->levers.At(i)->data->tilePos && app->itemManager->levers.At(i)->data->isPressed == false)
		{
			return true;
		}
	}
	return false;
}

void PathFinding::ActivateLever(const iPoint& pos) const
{
	for (int i = 0; i < app->itemManager->levers.Count(); i++)
	{
		if (pos == app->itemManager->levers.At(i)->data->tilePos && app->itemManager->levers.At(i)->data->isPressed == false)
		{
			app->itemManager->levers.At(i)->data->isPressed = true;
			
				if (app->itemManager->doors.At(i)->data->num == app->itemManager->levers.At(i)->data->num) 
				{
					app->itemManager->doors.At(i)->data->isOpen = true;
				}

			
		}
	}
}
int PathFinding::DistanceBetweenTiles(const iPoint& pos1, const iPoint& pos2) const
{
	int distance = abs(pos2.x - pos1.x) + abs(pos2.y - pos1.y);

	

	return  distance;
}

bool PathFinding::IsEnemyThereEmpty(const iPoint& pos) const
{

	//create function later to check the list of all entities
	for (int i = 0; i < app->entityManager->enemies.Count(); i++)
	{


		if (pos == app->entityManager->enemies.At(i)->data->tilePos)
		{
			return false;
		}
	}

	return true;
}

void PathFinding::ClearPath()
{
	if (lastPath.Count() > 0)
		lastPath.Clear();
}
void PathFinding::GenerateWalkeableArea(iPoint center, int range)
{
	InitBFS(center);
	iPoint current;

	for (int x = 0; x <= 100; x++)
	{
		if (frontier.Pop(current)) {
			p2List<iPoint> neightbours;

			neightbours.add(iPoint(current.x + 1, current.y));
			neightbours.add(iPoint(current.x, current.y + 1));
			neightbours.add(iPoint(current.x, current.y - 1));
			neightbours.add(iPoint(current.x - 1, current.y));

			p2ListItem<iPoint>* neightbour = neightbours.start;
			while (neightbour != NULL)
			{
				if (visited.find(neightbour->data) == false && IsWalkable(neightbour->data)
					&& IsTileEmpty(neightbour->data) && CreatePath(center, neightbour->data) <= range)
				{
					frontier.Push(neightbour->data);
					visited.add(neightbour->data);
				}
				neightbour = neightbour->next;
			}
		}
	}

	lastPath.Clear();
}

void PathFinding::DrawBFSPath()
{
	iPoint point;

	// Draw visited
	p2ListItem<iPoint>* item = visited.start;
	SDL_Rect rect;
	while (item != nullptr)
	{
		point = item->data;

		iPoint pos = app->map->MapToWorld(point.x, point.y);
		rect.x = pos.x;
		rect.y = pos.y;
		rect.w = (app->map->GetTileWidth());
		rect.h = (app->map->GetTileHeight());
		app->render->DrawRectangle(rect, 100, 50, 100, 125);

		item = item->next;
	}

	// Draw frontier
	//for (uint i = 0; i < frontier.Count(); ++i)
	//{
	//	point = *(frontier.Peek(i));
	//	rect.x = point.x;
	//	rect.y = point.y;
	//	rect.w = (app->map->mapData.tileWidth);
	//	rect.h = (app->map->mapData.tileHeight);
	//	app->render->DrawRectangle(rect, 100, 0, 155, 100);
	//}
}




void PathFinding::PropagateBFS()
{
	iPoint current;
	if (frontier.Pop(current)) {

		p2List<iPoint> neightbours;

		neightbours.add(iPoint(current.x + 1, current.y));
		neightbours.add(iPoint(current.x, current.y + 1));
		neightbours.add(iPoint(current.x, current.y - 1));
		neightbours.add(iPoint(current.x - 1, current.y));

		p2ListItem<iPoint>* neightbour = neightbours.start;
		while (neightbour != NULL)
		{
			if (visited.find(neightbour->data) == false && IsWalkable(neightbour->data) && IsEnemyThereEmpty(neightbour->data))
			{
				frontier.Push(neightbour->data);
				visited.add(neightbour->data);
			}
			neightbour = neightbour->next;
		}
	}
}

void PathFinding::ResetBFSPath()
{
	frontier.Clear();
	visited.clear();
}
void PathFinding::InitBFS(iPoint pos)
{
	frontier.Push(pos);
	visited.add(pos);
}
