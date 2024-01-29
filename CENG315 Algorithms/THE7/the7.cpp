#include "the7.h"

// do not add extra libraries here

void dijkstra(int src, const std::vector<std::vector<std::pair<int, int>>>& graph, std::vector<float>& distances) {
    int n = graph.size();
    distances.assign(n, std::numeric_limits<float>::max());

    auto comp = [](const std::pair<int, float>& left, const std::pair<int, float>& right) {
        return left.second > right.second;
    };
    std::priority_queue<std::pair<int, float>, std::vector<std::pair<int, float>>, decltype(comp)> pq(comp);

    pq.push({src, 0});
    distances[src] = 0;

    while (!pq.empty()) {
        int u = pq.top().first;
        float d = pq.top().second;
        pq.pop();

        if (d > distances[u])
            continue;

        for (const auto& edge : graph[u]) {
            int v = edge.first;
            float weight = edge.second;

            if (distances[u] + weight < distances[v]) {
                distances[v] = distances[u] + weight;
                pq.push({v, distances[v]});
            }
        }
    }
}

void get_infection_scores(const std::vector<std::vector<std::pair<int, int>>>& network, std::vector<float>& infection_scores) {
    int N = network.size();
    infection_scores.resize(N, 0);

    float MaxDist = 0;
    std::vector<float> distances;

    for (int i = 0; i < N; ++i) {
        dijkstra(i, network, distances);
        for (int j = 0; j < N; ++j) {
            if (i != j && distances[j] < std::numeric_limits<float>::max()) {
                MaxDist = std::max(MaxDist, distances[j]);
            }
        }
    }
    
    for (int i = 0; i < N; ++i) {
        dijkstra(i, network, distances);

        float AIS = 0;
        for (int j = 0; j < N; ++j) {
            if (i != j) {
                AIS += (distances[j] == std::numeric_limits<float>::max()) ? MaxDist + 1 : distances[j];
            }
        }
        AIS /= (N - 1);
        infection_scores[i] = 1.0f / AIS;
    }
}
