#include <iostream>
#include <vector>
#include <algorithm>
#include <queue>

using namespace std;

struct Edge {
    int u, v;
};

const int MAXN = 100005;
const int MAXM = 300005;

int n, m;
vector<Edge> edges;
vector<int> adj[MAXN];

int dfn[MAXN], low[MAXN], timer;
int stk[MAXM], top;

bool can_invite[MAXN];
int visited_bcc[MAXN];
int bcc_id = 0;
vector<int> bcc_adj[MAXN];
int color[MAXN];

void process_bcc(const vector<int>& bcc_edges) {
    bcc_id++;
    vector<int> bcc_nodes;
    for (int e : bcc_edges) {
        int u = edges[e].u;
        int v = edges[e].v;
        if (visited_bcc[u] != bcc_id) {
            visited_bcc[u] = bcc_id;
            bcc_nodes.push_back(u);
            bcc_adj[u].clear();
        }
        if (visited_bcc[v] != bcc_id) {
            visited_bcc[v] = bcc_id;
            bcc_nodes.push_back(v);
            bcc_adj[v].clear();
        }
        bcc_adj[u].push_back(v);
        bcc_adj[v].push_back(u);
    }
    
    bool is_bipartite = true;
    for (int u : bcc_nodes) {
        color[u] = 0;
    }
    
    if (!bcc_nodes.empty()) {
        queue<int> q;
        int start = bcc_nodes[0];
        color[start] = 1;
        q.push(start);
        while (!q.empty()) {
            int curr = q.front();
            q.pop();
            for (int nxt : bcc_adj[curr]) {
                if (color[nxt] == 0) {
                    color[nxt] = 3 - color[curr];
                    q.push(nxt);
                } else if (color[nxt] == color[curr]) {
                    is_bipartite = false;
                }
            }
        }
    }
    
    if (!is_bipartite) {
        for (int u : bcc_nodes) {
            can_invite[u] = true;
        }
    }
}

void dfs_iterative(int start_node) {
    struct Frame {
        int u;
        int p_edge;
        int adj_idx;
    };
    vector<Frame> call_stk;
    
    dfn[start_node] = low[start_node] = ++timer;
    call_stk.push_back({start_node, -1, 0});
    
    while (!call_stk.empty()) {
        auto& frame = call_stk.back();
        int u = frame.u;
        int p_edge = frame.p_edge;
        
        bool pushed = false;
        while (frame.adj_idx < (int)adj[u].size()) {
            int e_idx = adj[u][frame.adj_idx++];
            if (e_idx == p_edge) continue;
            
            int v = edges[e_idx].u ^ edges[e_idx].v ^ u;
            if (u == v) {
                continue;
            }
            if (dfn[v]) {
                low[u] = min(low[u], dfn[v]);
                if (dfn[v] < dfn[u]) {
                    stk[++top] = e_idx;
                }
            } else {
                stk[++top] = e_idx;
                dfn[v] = low[v] = ++timer;
                call_stk.push_back({v, e_idx, 0});
                pushed = true;
                break;
            }
        }
        
        if (pushed) continue;
        
        call_stk.pop_back();
        if (!call_stk.empty()) {
            int parent_u = call_stk.back().u;
            low[parent_u] = min(low[parent_u], low[u]);
            if (low[u] >= dfn[parent_u]) {
                vector<int> bcc_edges;
                while (true) {
                    int e = stk[top--];
                    bcc_edges.push_back(e);
                    if (e == p_edge) break;
                }
                process_bcc(bcc_edges);
            }
        }
    }
}

int main() {
    ios_base::sync_with_stdio(false);
    cin.tie(NULL);
    
    if (!(cin >> n >> m)) return 0;
    
    for (int i = 0; i < m; ++i) {
        int u, v;
        cin >> u >> v;
        edges.push_back({u, v});
        adj[u].push_back(i);
        adj[v].push_back(i);
    }
    
    for (int i = 1; i <= n; ++i) {
        if (!dfn[i]) {
            dfs_iterative(i);
        }
    }
    
    int ans = 0;
    for (int i = 1; i <= n; ++i) {
        if (!can_invite[i]) {
            ans++;
        }
    }
    
    cout << ans << "\n";
    
    return 0;
}