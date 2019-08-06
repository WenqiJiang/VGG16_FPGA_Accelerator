from num2fixed import*
import os
import numpy as np
import sys 

def read(filename):
    with open(filename, 'rb') as fl:
        while True:
            chunk = fl.read()
            if not chunk:
                break
            yield chunk

def main():
    filename = []
    filename.append('new_block1_conv1_kernel.txt')
    filename.append('new_block1_conv2_kernel.txt')
    filename.append('new_block2_conv1_kernel.txt')
    filename.append('new_block2_conv2_kernel.txt')
    filename.append('new_block3_conv1_kernel.txt')
    filename.append('new_block3_conv2_kernel.txt')
    filename.append('new_block3_conv3_kernel.txt')
    filename.append('new_block4_conv1_kernel.txt')
    filename.append('new_block4_conv2_kernel.txt')
    filename.append('new_block4_conv3_kernel.txt')
    filename.append('new_block5_conv1_kernel.txt')
    filename.append('new_block5_conv2_kernel.txt')
    filename.append('new_block5_conv3_kernel.txt')
    #filename = sys.argv[1]
    #integer = int(sys.argv[2])
    try:
        for name in range(13):
            record = []
            f = open(filename[name])
            print(filename[name])
            #filename[0] = filename[0].replace('new_', '')
            #filename = [c.replace('new_', '') for c in filename]
            line = f.readline()
            while line:
                num = list(map(float, line.split()))
                
                judge = str(num).replace('[','').replace(']','')
                if 'e' in judge:
                    num = list(str(0))
                    #print(num)
                    '''
                    last0 = judge[len(judge)-1]
                    last1 = judge[len(judge)-2]
                    exp = int(last1)*10+int(last0)
                    judge = judge[:-4].replace('.', '')
                    judgelist = list(judge)
                    if '-' in judge:
                        for i in range(exp):
                            judgelist.insert(1,'0')
                        judgelist.insert(2,'.')
                    else:
                        for j in range(exp):
                            judgelist.insert(0,'0')
                        judgelist.insert(1,'.')
                    judge = ''.join(judgelist)
                    judgelist = list(judge.split('a'))
                    num = judgelist
                    '''
                record.append(num)
                line = f.readline()
            f.close()
            #record = np.array(record)
            #record = dec_to_bin_array(record, integer, 15-integer)
            #print(record)
            temp = str(filename[name]).replace('new_', '')
            print(temp)
            with open('per_'+ temp, 'wb+') as dl:
                for data in record:
                    dl.write(str(data).replace('[','').replace(']','').replace('\'','') + '\n')

    except Exception as err:
        print(err)

if __name__ == '__main__':
    main()