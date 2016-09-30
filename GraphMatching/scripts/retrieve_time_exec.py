import re
import os
from os import listdir
from os.path import join, isfile
from dateutil.parser import *


def retrieve_start_time(line):
    m = re.search('START algorithm time: ([^\n]+)', line)
    if m : return m.group(1)

def retrieve_end_time(line):
    m = re.search('END algorithm time: ([^\n]+)', line)
    if m : return m.group(1)

IN_FOLDER = "data_plot/from_terminal"
# RG_NAME = "barabasi"
RG_NAME = "erdos_renyi"
# RG_NAME = "wattz_strogatz"
# RG_NAME = "facebook"

ALGO_NAME = "expandwhenstuck"
# ALGO_NAME = "pgm"
# ALGO_NAME = "expandonce"
in_folder = "%s/%s_s_1/rg_%s_%s" % (IN_FOLDER, RG_NAME,RG_NAME , ALGO_NAME)

def show_time(file_name):
    a = None
    b = None
    with open(file_name, 'r') as f:
        for l in  f:
            c = None
            c = retrieve_start_time(l)
            if c: a = parse(c)
            c = None
            c = retrieve_end_time(l)
            if c: b = parse(c)
    if a and b:
        print("%s\n\t%s\n\t%s\n\t%s" % (file_name, str(a), str(b), str(b-a)))

files = [join(in_folder, f) for f in listdir(in_folder) if isfile(join(in_folder, f))]
print(files)
for f in  files:
    show_time(f)