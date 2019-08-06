import os

filedir = os.getcwd() + 'weight'
filenames = os.listdir(filedir)
f = open('weight.txt', 'w')

for filename in filenames:
    filepath = filedir + '/' + filename
    for line in open(filepath):
        f.writelines(line)

f.close()