#ifndef __DIALOGUE_H__
#define __DIALOGUE_H__

#include "Source/Module.h"
#include "Source/Entity.h"
#include "Source/List.h"
#include "Source/DynArray.h"
#include <string>
#include <map>
#include "../Timer.h"
#include "Source/Textures.h"
#include "DialogueManager.h"

using namespace std;



class Dialogue {

public:
	string owner;

	string text;

	Position myPos;

	SDL_Texture* texture = nullptr;

	Dialogue(string owner, string text) {
		this->owner = owner;
		this->text = text;
		myPos = MIDDLE;
	}

	Dialogue(string owner, string text, string choiceA, string choiceB) {
		this->owner = owner;
		this->text = text;
		myPos = MIDDLE;
	}

	SDL_Texture* background = nullptr;


	string choiceA;
	string choiceB;


};

struct Node {

	Dialogue* dialogue;

	int ID;

	// if both aren't nullptr it means it is a choice
	Node* leftChild;
	Node* rightChild;

};
class Tree {
public:

	Node* GetNode(Dialogue* dialogue, int ID) {

		Node* N = new Node;

		N->dialogue = dialogue;
		N->ID = ID;
		N->leftChild = nullptr;
		N->rightChild = nullptr;

		return N;
	}

	Node* Insert(Node* root, Dialogue* dialogue, int ID) {

		// Recursion will break when this finds an empty space to create the node, return it to insert
		if (root == NULL) {
			root = GetNode(dialogue, ID);
			return root;
		}

		// criteria to insert the nodes
		else if (ID % 2 == 0) {
			// you pass the root (first elem) and the cost & name of the new root you want to make
			root->rightChild = Insert(root->rightChild, dialogue, ID);
		}
		else if (ID % 2 != 0) {
			root->leftChild = Insert(root->leftChild, dialogue, ID);
		}

		return root;
	}

};

//void PrintSkill(Node* root, int points) {
//
//	cout << root->name << " Unlocked " << points << " points spent\n";
//
//}


//int OptimizePoints(Node* root, int points) {
//
//	// to optmize pointx you should always TRY to go left
//
//	if (root->leftChild == NULL && root->rightChild == NULL || points < 15) {
//		//if possible unlock the last skill
//		if (points - root->cost > 0) { 
//			PrintSkill(root, root->cost);
//			cout << points - root->cost << " points remaining";
//			return 1;
//		}
//
//		else{ return 0; }
//		
//	}
//	
//		
//	else if (root->leftChild != NULL && points - root->cost > 0) {
//		    PrintSkill(root, root->cost);
//			return 1 + OptimizePoints(root->leftChild, points - root->cost);
//	}
//	
//	else {
//		// if we can only go right
//		if (root->rightChild != NULL && points - root->cost > 0) {
//			PrintSkill(root, root->cost);
//			return 1 + OptimizePoints(root->rightChild, points - root->cost);
//		}
//	
//	}
//	
//	// This only happens if you ran out of points to obtain any skills
//	return 0;
//
//}

//int main() {
//
//	Node* root = NULL;
//
//	root = Insert(root, "Double Spinning Edge", 40);
//	root = Insert(root, "Anchor Shot", 15);
//	root = Insert(root, "Rolling Smash", 30);
//	root = Insert(root, "Prominance Revolt", 70);
//	root = Insert(root, "Flame Nova", 60);
//	root = Insert(root, "Burning Sword", 80);
//	root = Insert(root, "Sword Bash", 30);
//	root = Insert(root, "Blazing Edge", 50);
//
//	// Uncomment to TEST
//	//int skills = OptimizePoints(root, 200);
//
//	//cout << endl << skills << " abilities have been unlocked !!"<< endl << endl;
//
//	//int HeavySkills = OptimizePoints(root->rightChild, 200);
//
//	//cout << endl << HeavySkills << " abilities have been unlocked !!" << endl << endl;
//
//	//
//	//int skills3 = OptimizePoints(root->rightChild->rightChild, 79);
//
//	//cout << endl << skills3 << " abilities have been unlocked !!" << endl << endl;
//
//}

#endif // __DIALOGUE_H__