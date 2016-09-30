from os import listdir, makedirs, rename
from os.path import isfile, join, exists
import re
import time

rg_name = 'facebook'
# rg_name = 'barabasi'
# rg_name = 'wattz_strogatz'
# rg_name = 'erdos_renyi'

out_folder = "data_plot/_%s_s_1" % (rg_name,)
# define algorithm here
# algo_suffix = "pgm"
# algo_suffix  = "expandwhenstuck"
algo_suffix = "expandonce"

ext = ".txt"
in_folder = 'data_plot/from_terminal/%s_s_1/%s_%s' % (rg_name, rg_name, algo_suffix)

param_list = ["correct matches", "matched", "precision", "F1-score", "recall"]

def retrieve(f):
    param_dict = {}
    fr = open(f, 'r')
    for row in fr:
        for k in param_list:
            m = re.search('%s = ([\d.\de-]+)' % k, row)
            if m: param_dict[k] = float(m.group(1))
    fr.close()
    return param_dict


def retrieve_seed_count(f):
    m = re.search('seed_([\d]+)_', f)
    return int(m.group(1))


def print_res(res_dict):
    if not exists(out_folder):
        makedirs(out_folder)
    for k, v_list in res_dict.items():
        with open(join(out_folder, algo_suffix + "_" + k + ext), 'w') as f:
            for v in v_list: f.write(v)


def rename_files():
    for pat in ('seed_', 'ac_'):
        files = [join(in_folder, f) for f in listdir(in_folder) if isfile(join(in_folder, f))]
        for f in files:
            m = re.search(pat + "([\d]+)", f)
            if m:
                f_new = f.replace(pat + m.group(1), pat + m.group(1).zfill(5))
                rename(f, f_new)
                print("%s -> %s" % (f.rsplit('/', 1)[-1], f_new.rsplit('/', 1)[-1]))
        time.sleep(1)  # sleeps for 1 second

# MAIN

rename_files()
files = [join(in_folder, f) for f in listdir(in_folder) if isfile(join(in_folder, f))]
files = sorted(files)

res_dict = dict((key, []) for key in param_list)
for i, f in enumerate(files):
    print("%d: %s" % (i + 1, f))
    seed_count = retrieve_seed_count(f)
    param_dict = retrieve(f)
    if len(param_dict) == len(param_list):
        precision = param_dict["correct matches"] / param_dict["matched"]
        recall = param_dict["recall"]
        param_dict['precision'] = precision
        param_dict['F1-score'] = 2*(precision * recall) / (precision + recall)
        for param in param_list:
            res_dict[param].append("%d %s\n" % (seed_count, param_dict[param] if param in param_dict else 0))

print_res(res_dict)