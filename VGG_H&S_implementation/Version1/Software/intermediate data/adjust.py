import sys
import os
from numpy import*
from num2fixed import*

def main():
    filename = sys.argv[1]
    inputa = int(sys.argv[2])
    output = int(sys.argv[3])
    try:
        if (inputa == 3):
            inp1 = 64
        else:
            inp1 = inputa
        if (inputa > 64):
            inp2 = 64
        else:
            inp2 = inputa
        record = []
        #correct = np.array(range(inputa*output*9), dtype=float)
        correct = []
        b = []
        f = open(filename)
        filename = filename.replace('.txt', '')
        #print(filename)
        line = f.readline()
        while line:
            num = list(map(float, line.split()))
            record.append(num)
            line = f.readline()
        f.close()
        #record = np.array(record)
        #print(record[1])
        '''
        for l in range(output/64):
            for k in range(inp1/64):
                for j in range(9*inp2):
                    for i in range(64):
                        correct.append(record[i*inputa*9 + j + k*64*9 + l*inputa*64*9])
        '''
        for l in range(output/64):
            for k in range(inp1/64):
                for m in range(9):
                    for j in range(inp2):
                        for i in range(64):
                            #b.append(int(i*inputa*9 + j + m*inputa + k*64 + l*inputa*64*9))
                            correct.append(record[i*inputa*9 + j + m*inputa + k*64 + l*inputa*64*9])   
        
        #correct = correct.reshape(-1,1)
        #print(len(correct))
        with open('new_'+filename+'.txt', 'wb+') as dl:
            for data in correct:
                dl.write(str(data).replace('[','').replace(']','') + '\n')

    except Exception as err:
        print(err)

if __name__ == '__main__':
    main()