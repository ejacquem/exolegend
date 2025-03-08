// #include <iostream>
// #include <vector>

// enum dir{
// 	UP,
// 	DOWN,
// 	LEFT,
// 	RIGHT
// };

// struct ij {
// 	int i;
// 	int j;
// };

// void printDir(dir t) {
// 	switch(t) {
// 		case UP:
// 			std::cout << "UP";
// 			break;
// 		case DOWN:
// 			std::cout << "DOWN";
// 			break;
// 		case LEFT:
// 			std::cout << "LEFT";
// 			break;
// 		case RIGHT:
// 			std::cout << "RIGHT";
// 			break;
// 	}
// }

// // return the cardinal direction from a to b
// dir getDir(ij *a, ij *b) {
// 	dir t;
// 	if(a->i == b->i)
// 		t = a->j < b->j ? DOWN : UP;
// 	else
// 		t =  a->i < b->i ? LEFT : RIGHT;
// 	printDir(t);
// 	return t;
// }

// void getStraightLine(std::vector<ij *> *path, ij *start) {
// 	std::vector<ij *>::iterator it = path->end() - 1;

// 	std::cout << "start: " << start->i << " " << start->j << std::endl;
// 	std::cout << "end: " << (*it)->i << " " << (*it)->j << std::endl;

// 	dir currentDir = getDir(start, *it);

// 	while (it != path->begin()) {
// 		std::cout << "current: " << (*it)->i << " " << (*it)->j << std::endl;
// 		std::cout << "after: " << (*(it - 1))->i << " " << (*(it - 1))->j << std::endl;
// 		if (currentDir == getDir(*it, *(it - 1)))
// 			path->pop_back();
// 		else
// 			break;
// 		it--;
// 	}
// }

// int main(int argc, char const *argv[])
// {
// 	/* code */

// 	std::vector<ij *> path;
// 	ij start = {1, 1};
// 	ij a = {1, 2};
// 	ij b = {1, 3};
// 	ij c = {2, 4};

// 	path.push_back(&c);
// 	path.push_back(&b);
// 	path.push_back(&a);


// 	for (int i = 0; i < path.size(); ++i)
// 	{
// 		std::cout << path[i]->i << " " << path[i]->j << std::endl;
// 	}

// 	std::cout << "----------------" << std::endl;
	
// 	getStraightLine(&path, &start);
// 	std::cout << std::endl;

// 	for (int i = 0; i < path.size(); ++i)
// 	{
// 		std::cout << path[i]->i << " " << path[i]->j << std::endl;
// 	}

// 	return 0;
// }
