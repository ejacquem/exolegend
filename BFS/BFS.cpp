#include "BFS.hpp" 

std::vector<MazeSquare*> BFS::shortestPath(MazeSquare* start, MazeSquare* target) {
    if (!start || !target) return {};

    std::queue<MazeSquare*> q;
    std::set<MazeSquare*> visited;
    std::map<MazeSquare*, MazeSquare*> cameFrom; 

    q.push(start);
    visited.insert(start);
    cameFrom[start] = nullptr;

    while (!q.empty()) {
        MazeSquare* current = q.front();
        q.pop();

        if (current == target) break; 


        for (MazeSquare* neighbor : {current->northSquare, current->southSquare, current->westSquare, current->eastSquare}) {
            if (neighbor && visited.find(neighbor) == visited.end()) {
                visited.insert(neighbor);
                cameFrom[neighbor] = current;
                q.push(neighbor);
            }
        }
    }

    std::vector<MazeSquare*> path;
    if (cameFrom.find(target) == cameFrom.end()) return {};

    for (MazeSquare* step = target; step != nullptr; step = cameFrom[step]) {
        path.push_back(step);
    }
    return path;
}