#include <algorithm>
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <stdlib.h>
#include <math.h>
#include <string>
#include <iostream>
#include <sstream>
#include <vector>
#include <fstream>
#include <string.h>
#include "definitions.h"
#include "graph.h"

using namespace std;
#define EdgeSeparator SEPARATOR /**< Token used to separate strings and ints */


bool matchCompareUnique(Match lhs, Match rhs) {
    if (lhs.value == rhs.value) {
        if (lhs.lnode == rhs.lnode) {
            return (lhs.rnode == rhs.rnode);
        }
        return (lhs.lnode == rhs.lnode);
    }
    else {
        return lhs.value == rhs.value;
    }
}


bool matchCompareSort(Match lhs, Match rhs) {
    if (lhs.value == rhs.value) {
        if (lhs.lnode == rhs.lnode) {
            return (lhs.rnode > rhs.rnode);
        }
        return (lhs.lnode > rhs.lnode);
    }
    else {
        return lhs.value > rhs.value;
    }
}


Graph::Graph(bool directed) {
    _nodes = 0;
    _nEdges = NULL;
    _edges = NULL;
    __edges__ = NULL;

    _directed = directed;
}


Graph::Graph(const Graph &g) {

    _nodes = g._nodes;
    _nEdges = new int[_nodes];
    _edges = new int *[_nodes];

    int total_edges = 0; //Total edges is used to create a contiguous block of memory

    for (int i = 0; i < _nodes; i++) {
        _nEdges[i] = g._nEdges[i];
        total_edges += _nEdges[i];
    }

    __edges__ = new int[total_edges];

    total_edges = 0;
    for (int i = 0; i < _nodes; i++) {
        this->_edges[i] = __edges__ + total_edges; //Setting the pointers to the list of edges to the right place
        for (int j = 0; j < _nEdges[i]; j++) //Copying the edges
        {
            this->_edges[i][j] = g._edges[i][j];
        }
        total_edges += _nEdges[i];
    }

    _directed = g._directed;

}

Graph::Graph(const Graph &g, double s) //Create a graph by sampling from graph g by sampling probability s.
{

    for (int i = 0; i < g._nodes; i++) {
        _nodeIDtoint[i] = i;
        _inttonodeID[i] = i;
    }

    std::list<Edge> le;
    for (int i = 0; i < g._nodes; i++) {
        for (int j = 0; j < g._nEdges[i]; j++) //Copying the edges
        {
            if (g._edges[i][j] > i && ((double(rand()) / RAND_MAX) < s)) {
                Edge e;
                e.u = i;
                e.v = g._edges[i][j];
                le.push_back(e);
            }
        }
    }

    createGraph(g._nodes, le);

    le.clear();
}


Graph::Graph(const Graph &g, double s, double t) //Create a graph by sampling from graph g by sampling probability s.
{

    std::list<Edge> le;
    int nodesize = 0;
    for (int i = 0; i < g._nodes; i++) {
        if ((double(rand()) / RAND_MAX) < t) {
            _nodeIDtoint[i] = nodesize;
            _inttonodeID[nodesize] = i;
            nodesize += 1;
        }
    }
    int iID = 0;
    int jID = 0;

    cout << "\tgenerate subgraph" << endl;
    for (int i = 0; i < g._nodes; i++) {

        if (_nodeIDtoint.find(i) != _nodeIDtoint.end()) {
            for (int j = 0; j < g._nEdges[i]; j++) //Copying the edges
            {
                if (g._edges[i][j] > i && ((double(rand()) / RAND_MAX) < s) &&
                    _nodeIDtoint.find(g._edges[i][j]) != _nodeIDtoint.end()) {
                    Edge e;
                    e.u = _nodeIDtoint[i];
                    e.v = _nodeIDtoint[g._edges[i][j]];
                    le.push_back(e);
                }
            }
        }
    }
    cout << "\tV_count = " << _nodeIDtoint.size() << endl;
    createGraph(_nodeIDtoint.size(), le);

    le.clear();
}


inline std::string Graph::createMatchID(int i, int j) {
    std::stringstream sstm;
    sstm << i << "$" << j;
    return sstm.str();
}


void Graph::untokenizeMatchID(std::string pair, int &first, int &second) {
    std::string ES = EdgeSeparator;
    int index = pair.find(ES);
    first = atoi(pair.substr(0, index).c_str());
    if (index != -1)
        second = atoi(pair.substr(pair.find(ES) + ES.size()).c_str());
}

void Graph::createGraph(int nodes, list<Edge> &le) {
    initialize(nodes);
    //Calculating the number of neighbors of each node
    for (list<Edge>::iterator it = le.begin(); it != le.end(); ++it) {
        _nEdges[it->u]++;
        if (!_directed) {
            _nEdges[it->v]++;
        }
    }

    //Create a contiguous memory buffer for storing the edges
    long long size = 0;
    for (int i = 0; i < nodes; i++) {
        size += _nEdges[i];
    }
    __edges__ = new int[size];
    size = 0;
    for (int i = 0; i < nodes; i++) {
        _edges[i] = __edges__ + size;
        size += _nEdges[i];
    }

    //Read for the second time: writing edges: No need for try-catch block. File exists unless user deleted it...
    int *pos = new int[nodes]; //How many edges have been added for each node
    for (int i = 0; i < nodes; i++) {
        pos[i] = 0;
    }

    for (list<Edge>::iterator it = le.begin(); it != le.end(); ++it) //Writing the edges
    {
        int u = it->u, v = it->v;
        _edges[u][pos[u]++] = v;
        if (!_directed) {
            _edges[v][pos[v]++] = u;
        }
    }

    for (int i = 0; i < _nodes; i++) //Sort the neighbors to simplify search in the future
    {
        if (_nEdges[i] > 0) {
            random_shuffle(_edges[i], _edges[i] + _nEdges[i]);
            //sort( _edges[ i ], _edges[ i ] + _nEdges[ i ] );
        }
    }

    delete[] pos;
}

Graph::~Graph() {
    if (_nodes > 0) {
        delete[] __edges__;
        delete[] _edges;
        delete[] _nEdges;
    }
}

void Graph::initialize(int n) {
    _nodes = n;
    _nEdges = new int[n];
    _edges = new int *[n];

    for (int i = 0; i < n; i++) {
        _nEdges[i] = 0;
        _edges[i] = NULL;
    }
    _directed = false;
}

std::map<int, int> Graph::getnodeIDtoint() {
    return _nodeIDtoint;
}

std::map<int, int> Graph::getinttonodeID() {
    return _inttonodeID;
}


int Graph::getNEdges() {
    int ret = 0;
    for (int i = 0; i < _nodes; i++) {
        ret += _nEdges[i];
    }
    if (_directed) //If it is not a directed graph, then it means that each edge was counted twice
    {
        return ret;
    }
    return ret / 2;
}


void Graph::readGraph(string filename, bool is_shuffled) {
    int u, v, n;

    if (__edges__ != 0) //If the graph has been already constructed, avoid a memory leak
    {
        cout << __FILE__ << " " << __LINE__ << endl;
        throw ALLOC_EXCEPTION;
    }

    ifstream file;

    try {
        //Read for the first time: discover number of edges
        file.open(filename.c_str());
        file >> n; //Read the number of nodes
        initialize(n); //Initialize the graph with its correct size
        cout << "n = " << n << endl;
        if (is_shuffled) {
            while (file >> u >> v) {
                _nEdges[u - 1]++;
                if (!_directed) {
                    _nEdges[v - 1]++;
                }
            }
        } else {
            while (file >> u >> v) {

                _nEdges[u]++;
                if (!_directed) {
                    _nEdges[v]++;
                }
            }
        }


        file.close();
    }
    catch (...) {
        cout << "Problem reading the graph file: " << __FILE__ << " " << __LINE__ << endl;
        return;
    }

    //Create a contiguous memory buffer for storing the edges
    int size = 0;
    for (int i = 0; i < n; i++) {
        size += _nEdges[i];
    }
    __edges__ = new int[size];
    size = 0;
    for (int i = 0; i < n; i++) {
        _edges[i] = __edges__ + size;
        size += _nEdges[i];
    }

    //Read for the second time: writing edges: No need for try-catch block. File exists unless user deleted it...
    int *it = new int[n]; //How many edges have been added for each node
    for (int i = 0; i < n; i++) {
        it[i] = 0;
    }

    file.open(filename.c_str());
    file >> n;
    if (is_shuffled) {
        while (file >> u >> v) {
            u--;
            v--;
            _edges[u][it[u]++] = v;
            if (!_directed) //If it is not a directed graph, add the opposite edge too
            {
                _edges[v][it[v]++] = u;
            }
        }
    } else {
        while (file >> u >> v) {
            _edges[u][it[u]++] = v;
            if (!_directed) //If it is not a directed graph, add the opposite edge too
            {
                _edges[v][it[v]++] = u;
            }
        }
    }

    file.close();

    for (int i = 0; i < _nodes; i++) //Sort the neighbors to simplify search in the future
    {
        if (_nEdges[i] > 0)
            sort(_edges[i], _edges[i] + _nEdges[i]);
    }

    delete[] it;
}


int Graph::getNNodes() {
    return _nodes;
}


int Graph::getNNeighbors(int index) {
    if (index >= _nodes) {
        return 0;
    }
    return _nEdges[index];
}


int *Graph::getNeighbors(int index) {
    if (index >= _nodes) {
        return NULL;
    }
    return _edges[index];
}


bool Graph::isDirected() {
    return _directed;
}


bool Graph::edgeExists(int u, int v) //O( logN )
{
    if (_nEdges[u] == 0 or _nEdges[v] == 0) return false;
    int pos = binary_search(_nEdges[u], _edges[u], v);
    if (pos < 0 or pos >= _nodes) {
        cout << __FILE__ << " " << __LINE__ << endl;
        throw SEGFAULT;
    }
    return _edges[u][pos] == v;
}


void Graph::writeGraph(string output_file) {
    ofstream output(output_file.c_str());
    output << _nodes << endl; //Output number of nodes
    for (int i = 0; i < _nodes; i++) {
        for (int j = 0; j < _nEdges[i]; j++) {
            if (i > _edges[i][j]) //Prints each edge only once
                continue;
            output << i << " " << _edges[i][j] << endl;
        }
    }
    output.close();
}


double Graph::compareGraph(const Graph &g) //O( M )
{
    //Compare two graphs according to the existence (or not) of an edge in both graphs

    int total_size = 0, total_match = 0;
    int *it_this, *it_g;

    for (int u = 0; u < _nodes; u++) //O( M )
    {
        it_this = _edges[u];
        it_g = g._edges[u];

        total_size += (_nEdges[u] + g._nEdges[u]);

        //The neighbors of both graphs are sorted, so we can perform a matching in linear time
        while (it_this != _edges[u] + _nEdges[u] and it_g != g._edges[u] + g._nEdges[u]) {
            if (*it_this < *it_g) {
                it_this++;
            }
            else if (*it_this > *it_g) {
                it_g++;
            }
            else {
                it_this++;
                it_g++;
                total_match += 2;
            }
        }
    }

    if (total_size == 0)
        return 1.; //If there is no edge in both graphs, then it means that they match exactly, as they have the same number of neighbors

    return double(total_match) / double(total_size);
}


void Graph::display_time(const char *str) {
    time_t rawtime;
    time(&rawtime);
    cout << str << ": " << ctime(&rawtime);
}

std::string Graph::retrieveString(char *buf) {

    size_t len = 0;
    while (buf[len] != '\0') {
        len++;
    }
    return string(buf, len);
}

void Graph::showInterResults(set<Match, CompareMatches> &matches, Graph &g) {
    int correct = 0, wrong = 0;

    for (pair_iterator it = matches.begin(); it != matches.end(); ++it) {
        if (_inttonodeID[it->lnode] == g._inttonodeID[it->rnode]) {
            correct++;
        } else {
            wrong++;
        }
    }
    cout << "Correct: " << correct << " Wrong: " << wrong << endl;
}

set<Match, CompareMatches> Graph::noisySeeds(Graph &g, list<Match> &seed, int r) {
    set<Match, CompareMatches> matches; //M
    set<int> leftNodes; //G1(M)
    set<int> rightNodes; //G2(M)
    set<Match, CompareMatches> matched_not_used; // M \ Z
    set<string> match_used; // Z
    map<string, int> score_map;

    cout << "Start percolation matching" << endl;

    cout << "M <- A_0 and Z <- A_0" << endl;

    // Z <- A_0
    for (list<Match>::iterator it = seed.begin(); it != seed.end(); ++it) {
        string ID = createMatchID(it->lnode, it->rnode);
        match_used.insert(ID);
    }

    // for all pairs [i,j] in A_0
    for (list<Match>::iterator it = seed.begin(); it != seed.end(); ++it) {
        int left_node = it->lnode;
        int right_node = it->rnode;

        // add one mark to all neighboring pairs
        for (int i = 0; i < _nEdges[left_node]; i++) {
            for (int j = 0; j < g._nEdges[right_node]; j++) {
                // if not in G_1,2(M)
                if (leftNodes.find(_edges[left_node][i]) == leftNodes.end() &&
                    rightNodes.find(g._edges[right_node][j]) == rightNodes.end()) {

                    string ID = createMatchID(_edges[left_node][i], g._edges[right_node][j]);

                    if (score_map.find(ID) == score_map.end()) {
                        score_map[ID] = 1;
                    } else {
                        score_map[ID]++;
                        // and if mark > r
                        if (score_map[ID] > r) {
                            // then add [i',j'] to M
                            Match m;
                            m.lnode = _edges[left_node][i];
                            m.rnode = g._edges[right_node][j];
                            m.value = score_map[ID];
                            // add to M
                            matches.insert(m);
                            leftNodes.insert(m.lnode);
                            rightNodes.insert(m.rnode);
                            // add to M \ Z
                            if (match_used.find(ID) == match_used.end()) {
                                Match m2;
                                memcpy(&m2, &m, sizeof m);
                                matched_not_used.insert(m2);
                            }

                            score_map.erase(ID);
                        }
                    }
                }
            }
        }

    }
    seed.clear();

    cout << "while M \\ Z != None do" << endl;
    // while M \ Z != None do
    int show_count = 0;
    bool show_res_once = true;
    unsigned long match_size = 0;
    while (!matched_not_used.empty()) {
        // randomly choose a pair [i,j] from unused matched
        double r_index = rand() % matched_not_used.size();
        set<Match, CompareMatches>::iterator it_not_used = matched_not_used.begin();
        for (; r_index != 0; r_index--) it_not_used++;

        int left_node = it_not_used->lnode;
        int right_node = it_not_used->rnode;

        // and add to used
        matched_not_used.erase(it_not_used);
        string ID_used = createMatchID(left_node, right_node);
        match_used.insert(ID_used);

        // add one mark to all neighboring pairs
        for (int i = 0; i < _nEdges[left_node]; i++) {
            for (int j = 0; j < g._nEdges[right_node]; j++) {
                // if not in G_1,2(M)
                if (leftNodes.find(_edges[left_node][i]) == leftNodes.end() &&
                    rightNodes.find(g._edges[right_node][j]) == rightNodes.end()) {
                    if (show_count++ == 100000) {
                        cout << "[" << left_node << "," << right_node << "] " << "(" << i << "," << j << ")" << endl;
                        show_count = 0;
                    }

                    string ID = createMatchID(_edges[left_node][i], g._edges[right_node][j]);
                    // and if mark > r
                    if (score_map.find(ID) == score_map.end()) {
                        score_map[ID] = 1;
                    } else {
                        // then add [i',j'] to M
                        Match m;
                        m.lnode = _edges[left_node][i];
                        m.rnode = g._edges[right_node][j];
                        m.value = score_map[ID];;
                        score_map.erase(ID);
                        matches.insert(m);
                        leftNodes.insert(m.lnode);
                        rightNodes.insert(m.rnode);

                        if (match_used.find(ID) == match_used.end()) {
                            Match m2;
                            memcpy(&m2, &m, sizeof m);
                            matched_not_used.insert(m2);
                        }

                    }
                }
                if (match_size != matches.size() && matches.size() % 2000 == 0) {
                    showInterResults(matches, g);
                    match_size = matches.size();
                }

            }
        }
    }


    leftNodes.clear();
    rightNodes.clear();
    matched_not_used.clear();
    match_used.clear();
    score_map.clear();

    return matches;
}


set<Match, CompareMatches> Graph::expandOnce(Graph &g, std::list<Match> &seed, int r, int seed_max_count) {
    multiset<Match, CompareMatches> activated; //A
    set<int> leftNodes; //V_1(U)
    set<int> rightNodes; //V_2(U)
    set<Match, CompareMatches> used_pairs; // Z
    list<Match> noisy_seeds; // A'_o

    /*
     * A'_0 <- A_0
     * A <- A_0
     */
    for (list<Match>::iterator it = seed.begin(); it != seed.end(); ++it) {
        Match m1, m2;
        m1.lnode = it->lnode;
        m1.rnode = it->rnode;
        m1.value = 1;
        activated.insert(m1);

        memcpy(&m2, &m1, sizeof m1);
        noisy_seeds.push_back(m2);
    }

    // while A'_0 < a'
    while (noisy_seeds.size() < seed_max_count) {
        // Z <- None
        used_pairs.clear();
        // and U <- A'_0
        for (list<Match>::iterator it_pair = seed.begin(); it_pair != seed.end(); ++it_pair) {
            leftNodes.insert(it_pair->lnode);
            rightNodes.insert(it_pair->rnode);
        }

        for (multiset<Match, CompareMatches>::iterator it_pair = activated.begin();
             it_pair != activated.end(); ++it_pair) {
            // [i,j]
            int left_node = it_pair->lnode;
            int right_node = it_pair->rnode;
            //for all neighboring pairs [i',j']
            for (int i = 0; i < _nEdges[left_node]; i++) {
                // |A'_0| < a'
                for (int j = 0; noisy_seeds.size() < seed_max_count && j < g._nEdges[right_node]; j++) {
                    // and i' or j' not in V1,2(U)
                    if (leftNodes.find(_edges[left_node][i]) == leftNodes.end() &&
                        rightNodes.find(g._edges[right_node][j]) == rightNodes.end()) {
                        // add [i', j'] to A'_0 and Z
                        Match m1, m2;
                        m1.lnode = _edges[left_node][i];
                        m1.rnode = g._edges[right_node][j];
                        m1.value = 1;
                        noisy_seeds.push_back(m1);
                        leftNodes.insert(m1.lnode);
                        rightNodes.insert(m1.rnode);

                        memcpy(&m2, &m1, sizeof m1);
                        used_pairs.insert(m2);
                    }
                }
            }
        }

        // A <- Z
        for (set<Match>::iterator it = used_pairs.begin(); it != used_pairs.end(); ++it) {
            Match m1;
            m1.lnode = it->lnode;
            m1.rnode = it->rnode;
            m1.value = 1;
            activated.insert(m1);
        }
    }

    used_pairs.clear();
    activated.clear();
    leftNodes.clear();
    rightNodes.clear();
    cout << "Noisy seeds are generated:" << endl;
    cout << "\tbefore = " << seed.size() << "; with generated = " << noisy_seeds.size() << endl;
    return noisySeeds(g, noisy_seeds, r);
}

std::set<Match, CompareMatches> Graph::pgm(Graph &g, std::list<Match> &seed, int r,
                                           bool flag) { //ToDo what is the flag?
    std::set<Match, CompareMatches> matches;
    std::map<std::string, int> score_map;
    std::multiset<Match, CompareMatches> scores;
    std::set<int> leftNodes;
    std::set<int> rightNodes;
    std::map<int, int> rinttonodeID = g.getinttonodeID();

    seed.sort(matchCompareSort);
    seed.unique(matchCompareUnique);

    if (flag) {
        for (list<Match>::iterator it = seed.begin(); it != seed.end(); ++it) {
            leftNodes.insert(it->lnode);
            rightNodes.insert(it->rnode);
            Match m;
            m.lnode = it->lnode;
            m.rnode = it->rnode;
            m.value = 1;
            matches.insert(m);
        }
    }
    int state = 0;
    for (list<Match>::iterator it = seed.begin(); it != seed.end(); ++it) {
        if (state % 10000 == 0)
            cout << state << endl;
        state++;
        int left_node = it->lnode;
        int right_node = it->rnode;

        for (int i = 0; i < _nEdges[left_node]; i++) {
            for (int j = 0; j < g._nEdges[right_node]; j++) {
                if (leftNodes.find(_edges[left_node][i]) == leftNodes.end() &&
                    rightNodes.find(g._edges[right_node][j]) == rightNodes.end()) {
                    std::string ID = createMatchID(_edges[left_node][i], g._edges[right_node][j]);
                    if (score_map.find(ID) == score_map.end()) {
                        score_map[ID] = 1;
                        Match m;
                        m.lnode = _edges[left_node][i];
                        m.rnode = g._edges[right_node][j];
                        m.value = 1;
                        scores.insert(m);
                    }
                    else {
                        int val = score_map[ID];
                        Match m;
                        m.lnode = _edges[left_node][i];
                        m.rnode = g._edges[right_node][j];
                        m.value = val;
                        std::set<Match, CompareMatches>::iterator it = scores.find(m);
                        if (it != scores.end())
                            scores.erase(it);
                        score_map[ID] = val + 1;
                        val += 1;
                        m.lnode = _edges[left_node][i];
                        m.rnode = g._edges[right_node][j];
                        m.value = val;
                        scores.insert(m);
                    }
                }
            }
        }
    }
    cout << "Done with the seeds!" << endl;
    int c = 0;
    int w = 0;
    int t = 0;
    int show_counter = 0;
    while ((scores.begin()->value > r) && (scores.size() > 0)) {

        std::set<Match, CompareMatches>::iterator it = scores.begin();
        int left_node = it->lnode;
        int right_node = it->rnode;
        if (leftNodes.find(left_node) == leftNodes.end() && rightNodes.find(right_node) == rightNodes.end()) {
            Match m;
            m.lnode = it->lnode;
            m.rnode = it->rnode;
            m.value = 1;
            matches.insert(m);

            if (matches.size() % 8000 == 0) {
                showInterResults(matches, g);
            }

            if (it->lnode == it->rnode) {//ToDo comment
                c++;
            }
            else {

                w++;
            }
            if (t++ / 10000 == 1) {
                cout << "correct: " << c << endl;
                cout << "wrong: " << w << endl;
                t = 0;
            }

            leftNodes.insert(it->lnode);
            rightNodes.insert(it->rnode);

            for (int i = 0; i < _nEdges[left_node]; i++) {
                for (int j = 0; j < g._nEdges[right_node]; j++) {
                    if (leftNodes.find(_edges[left_node][i]) == leftNodes.end() &&
                        rightNodes.find(g._edges[right_node][j]) == rightNodes.end()) {
                        if (show_counter++ == 100000) {
                            cout << "[" << left_node << ", " << right_node << "] " << "(" << i << ", " << j << ")" << endl;
                            show_counter = 0;
                        }
                        std::string ID = createMatchID(_edges[left_node][i], g._edges[right_node][j]);
                        if (score_map.find(ID) == score_map.end()) {
                            score_map[ID] = 1;
                            Match m;
                            m.lnode = _edges[left_node][i];
                            m.rnode = g._edges[right_node][j];
                            m.value = 1;
                            scores.insert(m);
                        }
                        else {
                            int val = score_map[ID];
                            Match m;
                            m.lnode = _edges[left_node][i];
                            m.rnode = g._edges[right_node][j];
                            m.value = val;
                            std::set<Match, CompareMatches>::iterator it = scores.find(m);
                            if (it != scores.end())
                                scores.erase(it);
                            score_map[ID] = val + 1;
                            val += 1;
                            m.lnode = _edges[left_node][i];
                            m.rnode = g._edges[right_node][j];
                            m.value = val;
                            scores.insert(m);
                        }
                    }
                }
            }
        }
        scores.erase(it);

        /*if (matches.size() % 10000 == 0) {

            int correct = 0, wrong = 0;
            for (std::set<Match, CompareMatches>::iterator it = matches.begin(); it != matches.end(); ++it) {
                if (_inttonodeID[it->lnode] == rinttonodeID[it->rnode]) {
                    correct++;
                }
                else {
                    wrong++;
                }
            }
//            cout << "Base Correct: " << correct << " Wrong: " << wrong << endl;
        }*/
    }

    score_map.clear();
    scores.clear();
    leftNodes.clear();
    rightNodes.clear();
    return matches;
}


set<Match, CompareMatches> Graph::expandWhenStuck(Graph &g, list<Match> &seed, int r, int push_level, int push_size) {
    set<Match, CompareMatches> matches; // M
    map<string, int> score_map; // marks for every pair
    multiset<Match, CompareMatches> inactive_pairs; // marks for every pair mark count > r
    set<int> leftNodesM; //G1(M)
    set<int> rightNodesM; //G2(M)
    set<string> seed_used; //Z
    seed.sort(matchCompareSort);
    seed.unique(matchCompareUnique);

    unsigned long matchSize = 0;
    int push = 0; // for check push level ToDo what for is this?
    bool tie_break = true; // with degree check

#define not_in_match(left_node, right_node) (leftNodesM.find(left_node) == leftNodesM.end() && rightNodesM.find(right_node) == rightNodesM.end())
#define not_in_matched(left_node, i, right_node, j) (leftNodesM.find(_edges[left_node][i]) == leftNodesM.end() && rightNodesM.find(g._edges[right_node][j]) == rightNodesM.end())


    // M <- A_0 ps: A = A_0
    for (seed_iterator it_seed = seed.begin(); it_seed != seed.end(); ++it_seed) {
        Match m;
        m.lnode = it_seed->lnode;
        m.rnode = it_seed->rnode;
        m.value = 1;
        matches.insert(m);
        leftNodesM.insert(it_seed->lnode);
        rightNodesM.insert(it_seed->rnode);
    }
    matchSize = matches.size();

    // while |A| > 0 do
    int iter_num = 1;

//ToDo delete
    pair_iterator found[10000];

    while (seed.size() > 0 && push < push_level) {
        cout << "############## iteration = " << iter_num << endl;
        cout << "for all pairs [i,j] of A do" << endl; //ToDo delete
        cout << "seed size = " << seed.size() << endl;
        //for all pairs [i,j] of A do
        for (seed_iterator seed_it = seed.begin(); seed_it != seed.end(); ++seed_it) {
            int left_node = seed_it->lnode;
            int right_node = seed_it->rnode;
            string ID_i_j = createMatchID(left_node, right_node);
            // add [i,j] to Z
            seed_used.insert(ID_i_j);
            int show_counter = 0;
            // add one mark to all neighboring pairs of [i,j]
            for (int i = 0; i < _nEdges[left_node]; i++) {
                for (int j = 0; j < g._nEdges[right_node]; j++) {
                    if (not_in_matched(left_node, i, right_node, j)) {
                        string ID_neighbor = createMatchID(_edges[left_node][i], g._edges[right_node][j]);
                        if (show_counter++ > 100000) {  //ToDO for debuging
                            cout << "\t" << ID_neighbor << endl;
                            show_counter = 0;
                        }
                        if (score_map.find(ID_neighbor) == score_map.end()) {
                            score_map[ID_neighbor] = 1;
                        } else { // update score
                            int val = score_map[ID_neighbor];
                            Match m;
                            m.lnode = _edges[left_node][i];
                            m.rnode = g._edges[right_node][j];
                            m.value = val;
                            pair_iterator it_delete = inactive_pairs.find(m);
                            if (it_delete != inactive_pairs.end())
                                inactive_pairs.erase(it_delete);
                            score_map[ID_neighbor] = val + 1;
                            m.value = val + 1;
                            inactive_pairs.insert(m);
                        }
                    }
                }
            }
        }
        // A <- None
        seed.clear();
        cout << "Seeds are expanded" << endl;

        bool stopFlag = true; //ToDo what is this about
        long show_counter = 0;
        bool printFlag = true;
        // while there exists an unmatched pair with score at least r+1
        while (inactive_pairs.size() > 0 && (inactive_pairs.begin()->value > r) && matches.size() < 200000000) {
            if (show_counter++ == 10000) { //ToDo delete
                cout << "\tin progress..." << endl;
            }
            if (printFlag) {
                cout << "-----" << endl;
                printFlag = false;
            }
            // among the pairs with the highest score
            pair_iterator it_inctv = inactive_pairs.begin();
            // remove from start matched pairs
            while (inactive_pairs.size() > 0) {
                if (not_in_match(it_inctv->lnode, it_inctv->rnode)) {
                    break;
                } else if (it_inctv != inactive_pairs.end()) { // ToDo solve this shit
                    inactive_pairs.erase(it_inctv++);
                }
            }
            // check if |A| > 0
            if (inactive_pairs.size() == 0) {
                break;
            }
            // select the unmatched pair [i,j] with the minimum |d_1,i - d_2,j| (degree diff)
            if (show_counter % 1000 == 0) {
                cout << "[" << show_counter << "] select the unmatched pair [i,j] with the minimum |d_1,i - d_2,j|" << endl;
            }
            if (tie_break) {
                /* pair_iterator it_search = inactive_pairs.begin();
                 it_inctv = inactive_pairs.begin();
                 int min_deg_diff = abs(_nEdges[it_inctv->lnode] - g._nEdges[it_inctv->rnode]);
                 int marks_count = it_inctv->value;
                 while (it_search->value == marks_count && min_deg_diff > 0) {
                     // select the unmatched pair [i,j]
                     if (not_in_match(it_search->lnode, it_search->rnode)) {
                         // find min |d_1,i - d_2,j|
                         if (abs(_nEdges[it_search->lnode] - g._nEdges[it_search->rnode]) < min_deg_diff) {
                             min_deg_diff = abs(_nEdges[it_search->lnode] - g._nEdges[it_search->rnode]);
                             it_inctv = it_search;
                         }
                         it_search++;
                     } else if (it_search != inactive_pairs.end()) {
                         inactive_pairs.erase(it_search++);
                     } else { // finish with inactive seeds
                         break;
                     }

                 }*/

//                ////////////////////////////////////////////////////

                it_inctv = inactive_pairs.begin();
                int marks_count = it_inctv->value;


                int count = 0;
                for (pair_iterator it_search = inactive_pairs.begin(); it_search->value == marks_count;) {
                    // select the unmatched pair [i,j]
                    if (not_in_match(it_search->lnode, it_search->rnode)) {
                        // find min |d_1,i - d_2,j|
                        if (abs(_nEdges[it_search->lnode] - g._nEdges[it_search->rnode]) == 0) {
                            found[count++] = it_search;
                        }
                        it_search++;
                    } else if (it_search != inactive_pairs.end()) {
                        inactive_pairs.erase(it_search++);
                    } else { // finish with inactive seeds
                        break;
                    }

                }
                if (count > 0)
                    it_inctv = found[rand() % count];


//                ////////////////////////////////////////////////////
            }
//            cout << "Selected pair is [" << it_inctv->lnode << "," << it_inctv->rnode << "]" << endl;

            int left_node = it_inctv->lnode;
            int right_node = it_inctv->rnode;
            string ID_not_active = createMatchID(left_node, right_node);
            if ((it_inctv->value > r) && not_in_match(left_node, right_node)) {
                // add [i,j] to M
                Match mtchd;
                mtchd.lnode = left_node;
                mtchd.rnode = right_node;
                mtchd.value = 1;
                matches.insert(mtchd);
                leftNodesM.insert(left_node);
                rightNodesM.insert(right_node);

                // if [i,j] not in Z
                if (seed_used.find(ID_not_active) == seed_used.end()) {
                    // add [i,j] to Z
                    seed_used.insert(ID_not_active);
                    // add one marks to all of its neighbouring pairs
                    for (int i = 0; i < _nEdges[left_node]; i++) {
                        for (int j = 0; j < g._nEdges[right_node]; j++) {
                            if (not_in_matched(left_node, i, right_node, j)) {
                                string ID_neighbor = createMatchID(_edges[left_node][i], g._edges[right_node][j]);
                                if (score_map.find(ID_neighbor) == score_map.end()) {
                                    score_map[ID_neighbor] = 1;
                                } else {
                                    int val = score_map[ID_neighbor];
                                    Match m;
                                    m.lnode = _edges[left_node][i];
                                    m.rnode = g._edges[right_node][j];
                                    m.value = val;
                                    pair_iterator it_delete = inactive_pairs.find(m);
                                    if (it_delete != inactive_pairs.end())
                                        inactive_pairs.erase(it_delete);
                                    score_map[ID_neighbor] = val + 1;
                                    m.value = val + 1;
                                    inactive_pairs.insert(m); //ToDo YOU ARE STUPID IDIOT!!!!!
                                }
                            }
                        }
                    }
                }
            }

            if (matches.size() % 80000 == 0) {
                showInterResults(matches, g);
            }

            if (it_inctv != inactive_pairs.end()) {
                inactive_pairs.erase(it_inctv);
            }
//            cout << "Marks expanded" << endl;

            //###########################################################
            // Garbage collector. Numbers are chosen based on my memory #
            //###########################################################
            if ((((score_map.size() > 650000000 or inactive_pairs.size() > 100000000) and (matches.size() % 10000 == 0)) or
                 (matches.size() == 500 or matches.size() == 5000 or matches.size() == 50000)) and matchSize != matches.size()) {
                cout << "erase!" << endl;

                matchSize = matches.size();
                showInterResults(matches, g);


                cout << "size score_map: " << score_map.size() << endl;
                int a, b;
                for (score_iterator it = score_map.begin(); it != score_map.end(); ++it) {
                    untokenizeMatchID(it->first, a, b);
                    if (!not_in_match(a, b)) {
                        score_map.erase(it);
                    }
                }
                cout << "size score_map: " << score_map.size() << endl;

                cout << "size inactive_pairs: " << inactive_pairs.size() << endl;
                for (pair_iterator it = inactive_pairs.begin(); it != inactive_pairs.end(); ++it) {
                    if (!not_in_match(it->lnode, it->rnode)) {
                        inactive_pairs.erase(it);
                    }
                }
                cout << "size inactive_pairs: " << inactive_pairs.size() << endl;
            }

            if ((matches.size() % 10000 == 0 or matches.size() == 1) && matches.size() != 0 && matches.size() != matchSize) {
                matchSize = matches.size();
                if (matches.size() >= 50000) {
                    tie_break = false;  //ToDo decide why
                }
                bool flag_grbage = true;
                if (flag_grbage and matches.size() % 100 == 0) {
                    int correct = 0, wrong = 0;
                    for (pair_iterator it = matches.begin(); it != matches.end(); ++it) {
                        if (_inttonodeID[it->lnode] == g._inttonodeID[it->rnode]) {
                            correct++;
                        }
                        else {
                            wrong++;
                        }
                    }
                    cout << "Correct: " << correct << " Wrong: " << wrong << endl;
                }

            }
        }
        cout << "Finish with inactive_pairs" << endl;


        int seedCnt = 0;
        if (stopFlag) {
            cout << "++++++" << endl;

            push++;
            // A <- all neighbors of M [i,j] not in Z, i,j not in V_1,V_2(M)
            for (pair_iterator it = matches.begin(); it != matches.end(); ++it) {
                int left_node = it->lnode;
                int right_node = it->rnode;
                // all neighbors of M
                for (int i = 0; i < _nEdges[left_node]; i++) {
                    for (int j = 0; j < g._nEdges[right_node]; j++) {
                        // i,j not in V_1,V_2(M)
                        if (not_in_matched(left_node, i, right_node, j)) {
                            string ID = createMatchID(_edges[left_node][i], g._edges[right_node][j]);
                            // [i,j] not in Z
                            if (seed_used.find(ID) == seed_used.end() and seedCnt < push_size) {
                                Match m;
                                m.lnode = _edges[left_node][i];
                                m.rnode = g._edges[right_node][j];
                                m.value = 1;
                                seed.push_back(m);
                                stopFlag = false; //ToDo decide about this flag why it should stop
                                seedCnt++;
                            }
                        }

                    }
                }
            }

            cout << "Extended seed size: " << seed.size() << endl;
            if (push == push_level - 1 or seed.size() == 0) { //ToDo decide about this one

                cout << "size inactive_pairs with value = 1: " << inactive_pairs.size() << endl;
                for (pair_iterator it = inactive_pairs.begin(); it != inactive_pairs.end(); ++it) {
                    if (it->value == 1) {
                        inactive_pairs.erase(it);
                    }
                }
                cout << "size: " << inactive_pairs.size() << endl;
                inactive_pairs.clear();
                cout << "Finish with inactive_pairs" << endl;
            }
        }
    }

    score_map.clear();
    inactive_pairs.clear();
    seed.clear();
    leftNodesM.clear();
    rightNodesM.clear();
    return matches;
}

set<Match, CompareMatches> Graph::expandTopStack(Graph &g, list<Match> &seed, int r, int push_level, int push_size) {
    set<Match, CompareMatches> matches; // M
    map<string, int> score_map; // marks for every pair
    multiset<Match, CompareMatches> inactive_pairs; // marks for every pair mark count > r
    set<int> leftNodesM; //G1(M)
    set<int> rightNodesM; //G2(M)
    set<string> seed_used; //Z
    seed.sort(matchCompareSort);
    seed.unique(matchCompareUnique);

    unsigned long matchSize = 0;
    int push = 0; // for check push level ToDo what for is this?
    bool tie_break = false; // with degree check

#define not_in_match(left_node, right_node) (leftNodesM.find(left_node) == leftNodesM.end() && rightNodesM.find(right_node) == rightNodesM.end())
#define not_in_matched(left_node, i, right_node, j) (leftNodesM.find(_edges[left_node][i]) == leftNodesM.end() && rightNodesM.find(g._edges[right_node][j]) == rightNodesM.end())

    int super_v_count = -1;
    // |A| > 0
    list<int> size_list{2 * 2, 3 * 3, 4 * 4, 5 * 5, 6 * 6, 7 * 7, 8 * 8, 9 * 9, 100};
    while (seed.size() > 0) {
        // M <- A
        for (seed_iterator it_seed = seed.begin(); it_seed != seed.end(); ++it_seed) {
            Match m;
            m.lnode = it_seed->lnode;
            m.rnode = it_seed->rnode;
            m.value = 1;
            matches.insert(m);
            leftNodesM.insert(it_seed->lnode);
            rightNodesM.insert(it_seed->rnode);
        }
        matchSize = matches.size();

        // while |A| > 0 do
        int iter_num = 1;
        while (seed.size() > 0 && push < push_level) {
            cout << "############## iteration = " << iter_num << endl;
            cout << "for all pairs [i,j] of A do" << endl; //ToDo delete
            cout << "seed size = " << seed.size() << endl;
            //for all pairs [i,j] of A do
            for (seed_iterator seed_it = seed.begin(); seed_it != seed.end(); ++seed_it) {
                int left_node = seed_it->lnode;
                int right_node = seed_it->rnode;
                string ID_i_j = createMatchID(left_node, right_node);
                // add [i,j] to Z
                seed_used.insert(ID_i_j);
                int show_counter = 0;
                // add one mark to all neighboring pairs of [i,j]
                for (int i = 0; i < _nEdges[left_node]; i++) {
                    for (int j = 0; j < g._nEdges[right_node]; j++) {
                        if (not_in_matched(left_node, i, right_node, j)) {
                            string ID_neighbor = createMatchID(_edges[left_node][i], g._edges[right_node][j]);
                            if (show_counter++ > 100000) {  //ToDO for debuging
                                cout << "\t" << ID_neighbor << endl;
                                show_counter = 0;
                            }
                            if (score_map.find(ID_neighbor) == score_map.end()) {
                                score_map[ID_neighbor] = 1;
                            } else { // update score
                                int val = score_map[ID_neighbor];
                                Match m;
                                m.lnode = _edges[left_node][i];
                                m.rnode = g._edges[right_node][j];
                                m.value = val;
                                pair_iterator it_delete = inactive_pairs.find(m);
                                if (it_delete != inactive_pairs.end())
                                    inactive_pairs.erase(it_delete);
                                score_map[ID_neighbor] = val + 1;
                                m.value = val + 1;
                                inactive_pairs.insert(m);
                            }
                        }
                    }
                }
            }
            // A <- None
            seed.clear();
            cout << "Seeds are expanded" << endl;

            bool stopFlag = true; //ToDo what is this about
            long show_counter = 0;
            bool printFlag = true;
            // while there exists an unmatched pair with score at least r+1
            while (inactive_pairs.size() > 0 && (inactive_pairs.begin()->value > r) && matches.size() < 200000000) {
                if (show_counter++ == 10000) { //ToDo delete
                    cout << "\tin progress..." << endl;
                }
                if (printFlag) {
                    cout << "-----" << endl;
                    printFlag = false;
                }
                // among the pairs with the highest score
                pair_iterator it_inctv = inactive_pairs.begin();
                // remove from start matched pairs
                while (inactive_pairs.size() > 0) {
                    if (not_in_match(it_inctv->lnode, it_inctv->rnode)) {
                        break;
                    } else if (it_inctv != inactive_pairs.end()) {
                        inactive_pairs.erase(it_inctv++);
                    }
                }
                // check if |A| > 0
                if (inactive_pairs.size() == 0) {
                    break;
                }
                // select the unmatched pair [i,j] with the minimum |d_1,i - d_2,j| (degree diff)
                if (show_counter % 1000 == 0) {
                    cout << "[" << show_counter << "] select the unmatched pair [i,j] with the minimum |d_1,i - d_2,j|" << endl;
                }


                if (tie_break) {
                    /////////////////
                    //  SUPER VERTEX
                    /////////////////
                    // ToDo reiplement in authomorphis style with top matches selection
                    int marks_count = it_inctv->value;
                    set<Match, CompareMatches> found;

                    map<int, list<int>> l_auto;
                    map<int, list<int>> r_auto;

                    for (pair_iterator it_search = inactive_pairs.begin(); it_search->value == marks_count;) {
                        // select the unmatched pair [i,j]
                        if (not_in_match(it_search->lnode, it_search->rnode)) {
                            // find overlapping
                            if (it_search->lnode == it_inctv->lnode && abs(_nEdges[it_search->lnode] - g._nEdges[it_search->rnode]) == 0) {
                                Match m;
                                m.lnode = it_search->lnode;
                                m.rnode = it_search->rnode;
                                m.value = it_search->value;
                                found.insert(m);
                                l_auto[it_search->lnode].push_back(it_search->rnode);
                                r_auto[it_search->rnode].push_back(it_search->lnode);
                            }
                            it_search++;
                        } else if (it_search != inactive_pairs.end()) {
                            inactive_pairs.erase(it_search++);
                        } else { // finish with inactive seeds
                            break;
                        }

                    }

                    set<Match, CompareMatches> other_found;
                    for (pair_iterator it_found = found.begin(); it_found != found.end() && found.size() > 1; it_found++) {
                        for (pair_iterator it_search = inactive_pairs.begin(); it_search->value == marks_count;) {
                            // select the unmatched pair [i,j]
                            if (not_in_match(it_search->lnode, it_search->rnode)) {
                                // find overlapping
                                if (it_search->rnode == it_found->rnode && abs(_nEdges[it_search->lnode] - g._nEdges[it_search->rnode]) == 0) {
                                    Match m;
                                    m.lnode = it_search->lnode;
                                    m.rnode = it_search->rnode;
                                    m.value = it_search->value;
                                    other_found.insert(m);
                                    l_auto[it_search->lnode].push_back(it_search->rnode);
                                    r_auto[it_search->rnode].push_back(it_search->lnode);
                                }
                                it_search++;
                            } else if (it_search != inactive_pairs.end()) {
                                inactive_pairs.erase(it_search++);
                            } else { // finish with inactive seeds
                                break;
                            }

                        }
                    }

                    found.insert(other_found.begin(), other_found.end());

                    // Check authomorphism
                    bool should = false;
                    bool in_find_stage = true;
                    vector<int> l_keys;
                    for (auto mit : l_auto){
                        l_keys.push_back(mit.first);
                    }
                    vector<int> r_common; // common righter
                    vector<int> l_common; // resulting left
                    l_common.push_back(*l_keys.begin()); // start one element in resulting
                    while (in_find_stage){

                        for (auto mit : l_auto){
                            if (find(l_common.begin(), l_common.end(), mit.first) != l_common.end()){
                                vector<int> common;

                            }
                        }

                    }
//                    for (pair_iterator it_del = other_found.begin(); it_del != other_found.end(); it_del++){
//                        for (pair_iterator it_del = other_found.begin(); it_del != other_found.end(); it_del++){
//
//                        }
//                    }

                    // end check authomorphism




                    if (should) {
                        set<Match, CompareMatches> neigbors;
                        super_v_count++;
                        cout << "Super vertex set :" << endl;
                        for (pair_iterator it_found = found.begin(); it_found != found.end(); it_found++) {
                            cout << it_found->lnode << "," << it_found->rnode << "-" << it_found->value << " ";
                            int left_node = it_found->lnode;
                            int right_node = it_found->rnode;
                            // add to V1(M) and V2(M)
                            leftNodesM.insert(left_node);
                            rightNodesM.insert(right_node);
                            // add to M
                            Match m;
                            m.rnode = right_node;
                            m.lnode = left_node;
                            m.super_v = super_v_count;
                            matches.insert(m);


                            for (int i = 0; i < _nEdges[left_node]; i++) {
                                for (int j = 0; j < g._nEdges[right_node]; j++) {
                                    Match mn;
                                    mn.lnode = _edges[left_node][i];
                                    mn.rnode = g._edges[right_node][j];

                                    neigbors.insert(mn);
                                }
                            }

                        }

                        // add one marks to all of its neighbouring pairs
                        for (pair_iterator nm = neigbors.begin(); nm != neigbors.end();nm++){
                                int left_node = nm->lnode;
                                int right_node = nm->rnode;
                                string ID_neighbor = createMatchID(left_node, right_node);
                                // if [i,j] not in Z
                                if (not_in_match(left_node, right_node)) {
                                    if (score_map.find(ID_neighbor) == score_map.end()) {
                                        score_map[ID_neighbor] = 1;
                                    } else {
                                        int val = score_map[ID_neighbor];
                                        Match m;
                                        m.lnode = left_node;
                                        m.rnode = right_node;
                                        m.value = val;
                                        pair_iterator it_delete = inactive_pairs.find(m);
                                        if (it_delete != inactive_pairs.end())
                                            inactive_pairs.erase(it_delete);
                                        score_map[ID_neighbor] = val + 1;
                                        m.value = val + 1;
                                        inactive_pairs.insert(m);
                                    }
                                }
                        }

                        cout << endl;

                        continue;
                    }

                    ////////////////////
                    // End SUPER VERTEX
                    ////////////////////

                }


                int left_node = it_inctv->lnode;
                int right_node = it_inctv->rnode;
                string ID_not_active = createMatchID(left_node, right_node);
                if ((it_inctv->value > r) && not_in_match(left_node, right_node)) {
                    // add [i,j] to M
                    Match mtchd;
                    mtchd.lnode = left_node;
                    mtchd.rnode = right_node;
                    mtchd.value = 1;
                    matches.insert(mtchd);
                    leftNodesM.insert(left_node);
                    rightNodesM.insert(right_node);

                    // if [i,j] not in Z
                    if (seed_used.find(ID_not_active) == seed_used.end()) {
                        // add [i,j] to Z
                        seed_used.insert(ID_not_active);
                        // add one marks to all of its neighbouring pairs
                        for (int i = 0; i < _nEdges[left_node]; i++) {
                            for (int j = 0; j < g._nEdges[right_node]; j++) {
                                if (not_in_matched(left_node, i, right_node, j)) {
                                    string ID_neighbor = createMatchID(_edges[left_node][i], g._edges[right_node][j]);
                                    if (score_map.find(ID_neighbor) == score_map.end()) {
                                        score_map[ID_neighbor] = 1;
                                    } else {
                                        int val = score_map[ID_neighbor];
                                        Match m;
                                        m.lnode = _edges[left_node][i];
                                        m.rnode = g._edges[right_node][j];
                                        m.value = val;
                                        pair_iterator it_delete = inactive_pairs.find(m);
                                        if (it_delete != inactive_pairs.end())
                                            inactive_pairs.erase(it_delete);
                                        score_map[ID_neighbor] = val + 1;
                                        m.value = val + 1;
                                        inactive_pairs.insert(m); //ToDo YOU ARE STUPID IDIOT!!!!!
                                    }
                                }
                            }
                        }
                    }
                }

                if (matches.size() % 80000 == 0) {
                    showInterResults(matches, g);
                }

                if (it_inctv != inactive_pairs.end()) {
                    inactive_pairs.erase(it_inctv);
                }
//            cout << "Marks expanded" << endl;

                //###########################################################
                // Garbage collector. Numbers are chosen based on my memory #
                //###########################################################
                if ((((score_map.size() > 650000000 or inactive_pairs.size() > 100000000) and (matches.size() % 10000 == 0)) or
                     (matches.size() == 500 or matches.size() == 5000 or matches.size() == 50000)) and matchSize != matches.size()) {
                    cout << "erase!" << endl;

                    matchSize = matches.size();
                    showInterResults(matches, g);


                    cout << "size score_map: " << score_map.size() << endl;
                    int a, b;
                    for (score_iterator it = score_map.begin(); it != score_map.end(); ++it) {
                        untokenizeMatchID(it->first, a, b);
                        if (!not_in_match(a, b)) {
                            score_map.erase(it);
                        }
                    }
                    cout << "size score_map: " << score_map.size() << endl;

                    cout << "size inactive_pairs: " << inactive_pairs.size() << endl;
                    for (pair_iterator it = inactive_pairs.begin(); it != inactive_pairs.end(); ++it) {
                        if (!not_in_match(it->lnode, it->rnode)) {
                            inactive_pairs.erase(it);
                        }
                    }
                    cout << "size inactive_pairs: " << inactive_pairs.size() << endl;
                }

                if ((matches.size() % 10000 == 0 or matches.size() == 1) && matches.size() != 0 && matches.size() != matchSize) {
                    matchSize = matches.size();
                    if (matches.size() >= 50000) {
                        tie_break = false;  //ToDo decide why
                    }
                    bool flag_grbage = true;
                    if (flag_grbage and matches.size() % 100 == 0) {
                        int correct = 0, wrong = 0;
                        for (pair_iterator it = matches.begin(); it != matches.end(); ++it) {
                            if (_inttonodeID[it->lnode] == g._inttonodeID[it->rnode]) {
                                correct++;
                            }
                            else {
                                wrong++;
                            }
                        }
                        cout << "Correct: " << correct << " Wrong: " << wrong << endl;
                    }

                }
            }
            cout << "Finish with inactive_pairs" << endl;


            int seedCnt = 0;
            if (stopFlag) {
                cout << "++++++" << endl;

                push++;
                // A <- all neighbors of M [i,j] not in Z, i,j not in V_1,V_2(M)
                for (pair_iterator it = matches.begin(); it != matches.end(); ++it) {
                    int left_node = it->lnode;
                    int right_node = it->rnode;
                    // all neighbors of M
                    for (int i = 0; i < _nEdges[left_node]; i++) {
                        for (int j = 0; j < g._nEdges[right_node]; j++) {
                            // i,j not in V_1,V_2(M)
                            if (not_in_matched(left_node, i, right_node, j)) {
                                string ID = createMatchID(_edges[left_node][i], g._edges[right_node][j]);
                                // [i,j] not in Z
                                if (seed_used.find(ID) == seed_used.end() and seedCnt < push_size) {
                                    Match m;
                                    m.lnode = _edges[left_node][i];
                                    m.rnode = g._edges[right_node][j];
                                    m.value = 1;
                                    seed.push_back(m);
                                    stopFlag = false; //ToDo decide about this flag why it should stop
                                    seedCnt++;
                                }
                            }

                        }
                    }
                }

                cout << "Extended seed size: " << seed.size() << endl;
                if (push == push_level - 1 or seed.size() == 0) { //ToDo decide about this one

                    cout << "size inactive_pairs with value = 1: " << inactive_pairs.size() << endl;
                    for (pair_iterator it = inactive_pairs.begin(); it != inactive_pairs.end(); ++it) {
                        if (it->value == 1) {
                            inactive_pairs.erase(it);
                        }
                    }
                    cout << "size: " << inactive_pairs.size() << endl;
                    inactive_pairs.clear();
                    cout << "Finish with inactive_pairs" << endl;
                }
            }
        }


        // find unused pairs
        cout << "Iteration end " << endl;
//        temporal solution TODO
        set<int> left_seed;
        set<int> right_seed;
        for (map<string, int>::iterator it = score_map.begin(); it != score_map.end(); ++it) {
            if (it->second == 1) {
                int left_node, right_node;
                untokenizeMatchID(it->first, left_node, right_node);
                // i,j not in V_1,V_2(M)
                if (not_in_match(left_node, right_node) && left_seed.find(left_node) == left_seed.end() && right_seed.find(right_node) == right_seed.end()) {
                    cout << left_node << "," << right_node << " ";
                    Match m;
                    m.lnode = left_node;
                    m.rnode = right_node;
                    m.value = 1;
                    seed.push_back(m);
                    left_seed.insert(left_node);
                    right_seed.insert(right_node);
                }
            }
        }
        cout << "seed size" << seed.size() << endl;

    }

    score_map.clear();
    inactive_pairs.clear();
    seed.clear();
    leftNodesM.clear();
    rightNodesM.clear();
    return matches;
}