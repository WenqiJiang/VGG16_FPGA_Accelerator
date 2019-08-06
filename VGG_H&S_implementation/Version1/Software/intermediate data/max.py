import sys
import os
from numpy import*
from num2fixed import*

def main():
    filename = sys.argv[1]

    try:
        record = []
        f = open(filename)
        filename = filename.replace('.txt', '')
        #print(filename)
        line = f.readline()
        while line:
            num = list(map(float, line.split()))
            record.append(num)
            line = f.readline()
        f.close()
        record = np.array(record, dtype = float)
        
        a = max(abs(record))
        print(a)
    except Exception as err:
        print(err)


if __name__ == '__main__':
    main()