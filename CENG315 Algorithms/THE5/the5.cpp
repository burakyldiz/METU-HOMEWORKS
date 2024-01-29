#include "the5.h"
// do not add extra libraries here
// algorithm fonksiyonlari //
template <typename T>
const T& custom_min(const T& a, const T& b) {
    return (b < a) ? b : a;
}

template <typename RandomIt>
void custom_sort(RandomIt first, RandomIt last) {
    for (auto i = first; i != last; ++i) {
        for (auto j = i + 1; j != last; ++j) {
            if (*j < *i) {
                std::swap(*i, *j);
            }
        }
    }
}

template <typename ForwardIt>
ForwardIt custom_unique(ForwardIt first, ForwardIt last) {
    if (first == last) {
        return last;
    }

    ForwardIt result = first;
    while (++first != last) {
        if (!(*result == *first)) {
            *(++result) = *first;
        }
    }
    return ++result;
}
// algorithm fonksiyonlari// 
// dfs //
void dfs(int u, const std::vector<std::vector<int>>& graph, std::vector<int>& disc, std::vector<int>& low, std::vector<bool>& inStack, std::vector<int>& nodeStack, std::vector<std::vector<int>>& cycles, int& time) {
    disc[u] = low[u] = ++time;
    nodeStack.push_back(u);
    inStack[u] = true;

    for (int v = 0; v < graph.size(); ++v) {
        if (graph[u][v] == 1) {
            if (disc[v] == -1) {
                dfs(v, graph, disc, low, inStack, nodeStack, cycles, time);
                low[u] = custom_min(low[u], low[v]);
            } else if (inStack[v]) {
                low[u] = custom_min(low[u], disc[v]);
            }
        }
    }

    if (low[u] == disc[u]) {
        std::vector<int> cycle;
        bool isSelfCycle = false;
        while (nodeStack.back() != u) {
            int v = nodeStack.back();
            nodeStack.pop_back();
            inStack[v] = false;
            cycle.push_back(v);
        }
        int v = nodeStack.back();
        nodeStack.pop_back();
        inStack[v] = false;
        cycle.push_back(v);

        // Check for self-loop
        if (cycle.size() == 1 && graph[v][v] == 1) {
            isSelfCycle = true;
        }

        if ((cycle.size() > 1 && !isSelfCycle) || (cycle.size() > 2 && isSelfCycle)) {
            std::vector<int> uniqueCycle = cycle;
            custom_sort(uniqueCycle.begin(), uniqueCycle.end());
            uniqueCycle.erase(custom_unique(uniqueCycle.begin(), uniqueCycle.end()), uniqueCycle.end());

            if (uniqueCycle.size() == cycle.size()) {
                cycles.push_back(cycle);
            }
        }
    }
}
//cycle bulmak//
std::vector<std::vector<int>> findCycles(const std::vector<std::vector<int>>& graph) {
    int numVertices = graph.size();
    std::vector<int> disc(numVertices, -1), low(numVertices, -1);
    std::vector<bool> inStack(numVertices, false);
    std::vector<int> nodeStack;
    std::vector<std::vector<int>> cycles;
    int time = 0;
    bool isself;
    
    for (int i = 0; i < numVertices; ++i) {
        if (disc[i] == -1) {
            dfs(i, graph, disc, low, inStack, nodeStack, cycles, time);
        }
    }
    
    // self-cycle bulma tek elemanli cycle , digerlerinin icinde yok
    for (int i = 0; i < numVertices; ++i) 
    {
        if (graph[i][i] == 1) 
        {
            isself = true;
            for (const auto& cycle : cycles)
            {
                for (int j = 0; j < cycle.size(); ++j) 
                {
                    if(i == cycle[j])
                    {
                        isself = false;
                    }
                }
            }
            if(isself)
            {
                std::vector<int> selfCycle = {i};
                cycles.push_back(selfCycle);
            }
        }
        
    }
    return cycles;
}

void run(const std::vector<std::vector<int>>& dependencyMatrix, 
         bool& isCompilable,
         std::vector<int>& compileOrder, 
         std::vector<std::vector<int>>& cyclicDependencies)
         {
            int numFiles = dependencyMatrix.size();
            std::vector<std::vector<int>> cycles = findCycles(dependencyMatrix);
            if (!cycles.empty()) // compilable degil
            {
                isCompilable = false;
                cyclicDependencies = cycles;
            }
            
            if(cycles.empty()) // compilable
            {
                isCompilable = true;
                std::vector<int> inDegree(numFiles, 0);
                for (int i = 0; i < numFiles; ++i) // topological sort
                {
                    for (int j = 0; j < numFiles; ++j) 
                    {
                        if (dependencyMatrix[i][j] == 1) 
                        {
                            inDegree[j]++;
                        }
                    }
                }
                compileOrder.clear();
                while (compileOrder.size() < numFiles) 
                {
                    for (int i = 0; i < numFiles; ++i) 
                    {
                        if (inDegree[i] == 0) 
                        {
                            compileOrder.push_back(i);
                            inDegree[i] = -1;
                            for (int j = 0; j < numFiles; ++j) 
                            {
                                if (dependencyMatrix[i][j] == 1) 
                                {
                                    inDegree[j]--;
                                }
                            }
                        }
                    }
                }   
            }
        }
