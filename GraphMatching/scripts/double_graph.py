
FILE_PREF = "/home/ildar/projects/clion_projects/GraphMatching/data/"
FILE_IN = FILE_PREF + "rg_chung_lu_part.txt"
FILE_OUT = FILE_PREF + "rg_chung_lu_bridge.txt"

def copy_file(content):
    with open(FILE_OUT, 'w') as f_w:
        f_w.write(str(int(content[0]) * 2 + 1)+"\n")
        f_w.writelines(content[1:])

def update_data(content):
    with open(FILE_OUT, 'a') as f_w:
        for s in content[1:]:
            s = s.split()
            f_w.write("{0} {1}\n".format(int(s[0]) + 1000 , int(s[1]) + 1000))

f_r = open(FILE_IN, 'r')
content = f_r.readlines()
f_r.close()

copy_file(content)
update_data(content)
