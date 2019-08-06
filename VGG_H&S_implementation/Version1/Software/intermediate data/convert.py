from num2fixed import*
import os
from numpy import*
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
    filename.append('per_block1_conv1_kernel.txt')
    filename.append('per_block1_conv2_kernel.txt')
    filename.append('per_block2_conv1_kernel.txt')
    filename.append('per_block2_conv2_kernel.txt')
    filename.append('per_block3_conv1_kernel.txt')
    filename.append('per_block3_conv2_kernel.txt')
    filename.append('per_block3_conv3_kernel.txt')
    filename.append('per_block4_conv1_kernel.txt')
    filename.append('per_block4_conv2_kernel.txt')
    filename.append('per_block4_conv3_kernel.txt')
    filename.append('per_block5_conv1_kernel.txt')
    filename.append('per_block5_conv2_kernel.txt')
    filename.append('per_block5_conv3_kernel.txt')
    #filename = sys.argv[1]
    #integer = int(sys.argv[2])
    integer = 1
    try:
        for name in range(13):
            record = []
            f = open(filename[name])
            print(filename[name])
            #filename = filename.replace('.txt', '').replace('new_', '')
            #print(filename)
            line = f.readline()
            while line:
                num = list(map(float, line.split()))
                record.append(num)
                line = f.readline()
            f.close()
            record = np.array(record)
            record = dec_to_bin_array(record, integer, 15-integer)
            #print(record)
            temp = str(filename[name].replace('per_', ''))
            print(temp)
            with open('bin_'+temp, 'wb+') as dl:
                for data in record:
                    dl.write(data + '')

    except Exception as err:
        print(err)

if __name__ == '__main__':
    main()