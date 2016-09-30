#!/usr/bin/env python
import pylab as pl
import os

# IN_FOLDER = "_erdos_renyi_s_1"
# IN_FOLDER = "_barabasi_s_1"
# IN_FOLDER = '_wattz_strogatz_s_1'
# IN_FOLDER = '_chung_lu_100000_s_1'
IN_FOLDER = '_facebook_s_1'
# IN_FOLDER = 'case_3clusters_s_1'

DATA_FOLDER_NAME = os.path.join('data_plot', IN_FOLDER)
OUT_FOLDER_NAME = 'out'
# PLOT_TITLE = 'Erdos Renyi model n=10^6 s=1'
# PLOT_TITLE = 'Barabasi-Albert model n=10^6 s=1'
# PLOT_TITLE = 'Wattz-Strogatz model n=10^6 s=1'
# PLOT_TITLE = 'Chung-Lu model n=100 000 s=1'
# PLOT_TITLE = '3 clusters n=900 000 s=1'
# PLOT_TITLE = 'Fasebook n=4039 s=1'
FLG_SAVE_IMG = True
# MAX_X = 250
# MAX_X = 350
# MAX_X = 20001
# MAX_X = 100
MAX_X =200

# MAX_Y = 1000000
MAX_Y = 3

def read_data(filename):
    data = []
    seed = {}
    with open(filename, 'r') as f:
        for row in f:
            a = tuple(float(num) for num in row.split())
            if a[0] in  seed:
                if a[1] > seed[a[0]]:
                    seed[a[0]] = a[1]
            else:
                seed[a[0]] = a[1]
    for item in sorted(seed.iteritems()):
        if item[0] <= MAX_X: data.append(item)
    return data


def set_up():
    def set_up_folders():
        check_and_create_foldeer = lambda dname: os.makedirs(dname) if not os.path.exists(dname) else None
        check_and_create_foldeer(OUT_FOLDER_NAME)
        check_and_create_foldeer(os.path.join(OUT_FOLDER_NAME, DATA_FOLDER_NAME))
    # show just bottom and left border
    fig, ax = pl.subplots()
    ax.spines['right'].set_visible(False)
    ax.spines['top'].set_visible(False)
    ax.xaxis.set_ticks_position('bottom')
    ax.yaxis.set_ticks_position('left')
    if FLG_SAVE_IMG: set_up_folders()


def plot_data():
    pref_heuristic = 'expand_when_stuck_'
    pref_once = 'expand_once_'
    pref_pgm = 'pgm_'
    pref_top = 'expand_top_stack_'
    d = {pref_heuristic + 'label': 'ExpandWhenStuck',
         pref_pgm + 'label': 'PGM r=2',
         pref_once + 'label': 'ExpandOnce',
         pref_top + 'label': 'ExpandTopStack',
         pref_heuristic + 'color': 'red',
         pref_pgm + 'color': 'blue',
         pref_once + 'color': 'green',
         pref_top + 'color': 'purple',
         pref_heuristic + 'marker': 'o',
         pref_pgm + 'marker': '^',
         pref_once + 'marker': 'x',
         pref_top + 'marker': 'D',
         pref_heuristic + 'ms': 40,
         pref_once + 'ms': 60,
         pref_pgm + 'ms': 70,
         pref_top + 'ms': 40,
         'xlabel': 'Initial seeds'}

    def set_up_plot():
        axes = pl.axes()
        axes.autoscale_view()
        # axes.set_title(PLOT_TITLE)
        pl.legend(loc='lower right', fancybox=True, shadow=True)
        pl.ylim(ymin=0)
        pl.xlim(xmin=0)
        pl.xlim(xmax=MAX_X)
        pl.ylim(ymax=1.015)
        pl.legend(bbox_to_anchor=(0., 1.02, 1., .102), loc=3, prop={'size':14},
           ncol=3, mode="expand", borderaxespad=0.)

    def plot_of():
        pl.xlabel(d['xlabel'])
        pl.ylabel(d['ylabel'])
        for prefix in  (pref_heuristic,pref_once, pref_pgm):# pref_top):
            data_file = os.path.join(DATA_FOLDER_NAME, d[prefix + 'fname'])
            pl.plot(*zip(*read_data(data_file)), color=d[prefix + 'color'])
            pl.scatter(*zip(*read_data(data_file)), marker=d[prefix + 'marker'], label=d[prefix + 'label'], s = d[prefix + 'ms'],
                       color=d[prefix + 'color'], linewidths=1, facecolors='none')
        set_up_plot()
        # pl.ylim(ymax=d['ymax'])

        pl.gcf().canvas.set_window_title(d['ylabel'])
        if FLG_SAVE_IMG: pl.savefig(os.path.join(OUT_FOLDER_NAME, DATA_FOLDER_NAME, d['img_name']))
        pl.show()

    def plot_precision():
        d.update({
            pref_heuristic + 'fname': 'expandwhenstuck_precision.txt',
            pref_pgm + 'fname': 'pgm_precision.txt',
            pref_once + 'fname': 'expandonce_precision.txt',
            'ylabel': 'precision',
            'img_name': 'precision.pdf',
            'ymax': 1})
        plot_of()

    def plot_recall():
        d.update({
            pref_heuristic + 'fname': 'expandwhenstuck_recall.txt',
            pref_pgm + 'fname': 'pgm_recall.txt',
            pref_once + 'fname': 'expandonce_recall.txt',
            pref_top + 'fname': 'expandtopstack_recall.txt',
            'ylabel': 'recall',
            'img_name': 'recall.pdf',
            'ymax': 1})
        plot_of()

    def plot_count():
        d.update({
            pref_heuristic + 'fname': 'expandwhenstuck_correct matches.txt',
            pref_pgm + 'fname': 'pgm_correct matches.txt',
            pref_once + 'fname': 'expandonce_correct matches.txt',
            'ylabel': 'count',
            'img_name': 'count.pdf',
            'ymax': MAX_Y})
        plot_of()

    def plot_matches():
        d.update({
            pref_heuristic + 'fname': 'expandwhenstuck_matched.txt',
            pref_pgm + 'fname': 'pgm_matched.txt',
            pref_once + 'fname': 'expandonce_matched.txt',
            'ylabel': 'matches',
            'img_name': 'matches.pdf',
            'ymax': MAX_Y})
        plot_of()

    def plot_f1_score():
        d.update({
            pref_heuristic + 'fname': 'expandwhenstuck_F1-score.txt',
            pref_pgm + 'fname': 'pgm_F1-score.txt',
            pref_once + 'fname': 'expandonce_F1-score.txt',
            pref_top + 'fname': 'expandtopstack_F1-score.txt',
            'ylabel': 'F1-score',
            'img_name': 'F1-score.pdf',
            'ymax': 1})
        plot_of()

    # plot_precision()
    # plot_count()
    # plot_matches()
    plot_f1_score()
    # plot_recall()

######
# MAIN
######
set_up()
plot_data()