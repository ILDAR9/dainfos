//
// Created by ildar on 08.04.16.
//

#ifndef GRAPHMATCHING_TEST_H
#define GRAPHMATCHING_TEST_H



struct Match
{
    int lnode;
    int rnode;
    int value;
};

struct CompareMatches
{
    bool operator()(const Match & lhs, const Match & rhs) {
        if(lhs.value == rhs.value)
        {
            if(lhs.lnode == rhs.lnode)
            {
                return ((((lhs.rnode * 0xf7f7f7f7) ^ 0x8364abf7) * 0xf00bf00b) ^ 0xf81bc437) > ((((rhs.rnode * 0xf7f7f7f7) ^ 0x8364abf7) * 0xf00bf00b) ^ 0xf81bc437);
            }
            return ((((lhs.lnode ^ 0xf7f7f7f7) * 0x8364abf7) ^ 0xf00bf00b) * 0xf81bc437) < ((((rhs.lnode ^ 0xf7f7f7f7) * 0x8364abf7) ^ 0xf00bf00b) * 0xf81bc437 );
        }
        else
        {
            return lhs.value > rhs.value;
        }
    }
};



#endif //GRAPHMATCHING_TEST_H
