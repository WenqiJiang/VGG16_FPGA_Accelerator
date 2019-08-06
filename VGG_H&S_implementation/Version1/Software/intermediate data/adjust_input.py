import sys
import os
from numpy import*
from num2fixed import*

def main():
    filename = sys.argv[1]
    try:
        record = []
        #correct = np.array(range(input*output*9), dtype=float)
        correct = []
        f = open(filename)
        filename = filename.replace('.txt', '')
        line = f.readline()
        while line:
            num = list(map(float, line.split()))
            record.append(num)
            line = f.readline()
        f.close()
        #record = np.array(record)
        #print(record[1])
        for j in range(224*224):
            for i in range(3):
                correct.append(record[i*224*224+j])
                        
        #correct = correct.reshape(-1,1)
        print(len(correct))
        with open('new_'+filename+'.txt', 'wb+') as dl:
            for data in correct:
                dl.write(str(data).replace('[','').replace(']','') + '\n')

    except Exception as err:
        print(err)

if __name__ == '__main__':
    main()