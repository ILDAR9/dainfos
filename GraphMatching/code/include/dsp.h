#ifndef GRAPHMATCHING_DSP_H
#define GRAPHMATCHING_DSP_H
struct RandomGraphProp {
    int n;     //total count (random graph)
    double p;  //vertex (random graph)
    double t;  //vertex (selection)
    double s;  //edges (selection)
};

unsigned int fact(unsigned int);

double a_c(RandomGraphProp &, unsigned int);

double b_c(RandomGraphProp &, unsigned int);

/*
 * Growing a Graph Matching from handful of seeds
 * G(n,p,s,t)
 * r - given threshold
 */
void showParams(double V_COUNT, double E_COUNT, double ps, double pt, int THRESHOLD_R);

unsigned int fact(unsigned int);


#endif
