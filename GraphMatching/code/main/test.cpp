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

Match randMatch() {
    Match m;
    m.lnode = rand() % 100;
    m.rnode = rand() % 100;
    m.value = 1;
    return m;
}

int mainTest(void) {

    set<Match, CompareMatches> matches;
    for (int i = 0; i < 15; ++i) {
        Match m = randMatch();
        printf("(%d:%d)\n", m.lnode, m.rnode);
        matches.insert(m);
    }

    printf("----------------------------\n");

    for (set<Match, CompareMatches>::iterator it = matches.begin(); it != matches.end(); ++it) {
        printf("(%d:%d)\n", it->lnode, it->rnode);
    }


    return 0;
}
