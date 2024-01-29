#include "the6.h"

int find(std::vector<int>& parent, int i) {
    if (parent[i] == -1) return i;
    return find(parent, parent[i]);
}

void Union(std::vector<int>& parent, int x, int y) {
    int xset = find(parent, x);
    int yset = find(parent, y);
    if (xset != yset) {
        parent[xset] = yset;
    }
}

int find_structure(std::vector< std::vector<std::pair<int,int> > >& bond_energies, 
                   std::vector< std::vector<std::pair<int,int> > >& lowest_energy_structure) {
    int n = bond_energies.size();
    std::vector<int> parent(n, -1);
    std::vector<std::pair<int, std::pair<int, int>>> edges;

    for (int i = 0; i < n; ++i) {
        for (auto& edge : bond_energies[i]) {
            edges.push_back({edge.second, {i, edge.first}});
        }
    }

    std::sort(edges.begin(), edges.end());

    int lowest_total_energy = 0;
    lowest_energy_structure.resize(n);

    for (auto& edge : edges) {
        int weight = edge.first;
        int u = edge.second.first;
        int v = edge.second.second;

        int set_u = find(parent, u);
        int set_v = find(parent, v);

        if (set_u != set_v) {
            lowest_energy_structure[u].push_back({v, weight});
            lowest_energy_structure[v].push_back({u, weight});
            lowest_total_energy += weight;
            Union(parent, set_u, set_v);
        }
    }

    return lowest_total_energy;
}

void dfs(const std::vector< std::vector<std::pair<int,int> > >& graph, int v, 
         std::vector<bool>& visited, int& maxDist, int dist, 
         int& farthestNode, std::vector<int>& path, std::vector<int>& longestPath) {
    visited[v] = true;
    path.push_back(v);

    if (dist > maxDist) {
        maxDist = dist;
        farthestNode = v;
        longestPath = path;
    }

    for (auto& edge : graph[v]) {
        if (!visited[edge.first]) {
            dfs(graph, edge.first, visited, maxDist, dist + 1, farthestNode, path, longestPath);
        }
    }

    path.pop_back();
}

int find_longest_chain( std::vector< std::vector<std::pair<int,int> > >& molecule_structure, 
                       std::vector<int>& chain) {
    int n = molecule_structure.size();
    std::vector<bool> visited(n, false);
    int farthestNode = 0;
    int maxDist = 0;
    std::vector<int> path, longestPath;

    dfs(molecule_structure, 0, visited, maxDist, 0, farthestNode, path, longestPath);
    std::fill(visited.begin(), visited.end(), false);
    path.clear();
    maxDist = 0;

    dfs(molecule_structure, farthestNode, visited, maxDist, 0, farthestNode, path, longestPath);

    chain = longestPath;

    return maxDist + 1;
}
