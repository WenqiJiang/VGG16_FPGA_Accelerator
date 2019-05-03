import numpy as np

####################################################
#            api functions for other files         #
####################################################

def dec_to_bin(x, int_digit, decimal_digit, decimal_point=False):
    """ given a float / int -> return string of fixed point
    
    :param x: original number
    :param int_digit: the number of digits of integer part.
    :param decimal_digit: the number of digits of decimal part.
    :return:
    """
    decimal = decimal_dec_to_bin(x, decimal_digit)
    
    if x >= 0:
        sign = 1
    else:
        sign = -1
        x = -x
        
    # suppose x > 0
    integer = bin(int(x))[2:]
    if integer == "0":
        integer = ""
    add_zero_int = int_digit - len(integer)
    add_zero_integer_complement ="0" + "0" * add_zero_int + integer
        
    add_zero_dec = decimal_digit - len(decimal)
    add_zero_dec = decimal + "0" * add_zero_dec
    
    if sign == 1: # x > 0
        if decimal_point:
            return add_zero_integer_complement + "." + add_zero_dec
        else:
            return add_zero_integer_complement + add_zero_dec
    else: # x < 0
        if decimal_point:
            temp_result = add_zero_integer_complement + add_zero_dec
            temp_result_reverse = "".join(str(0 if int(r) == 1 else 1) for r in temp_result) # radix-minus-one complement
            result = add_1(temp_result_reverse)
            
            return result[0:int_digit + 1] + "." + result[int_digit + 1:]
        else:
            temp_result = add_zero_integer_complement + add_zero_dec
            temp_result_reverse = "".join(str(0 if int(r) == 1 else 1) for r in temp_result) # radix-minus-one complement
#             print("reverse: ", temp_result_reverse)
            result = add_1(temp_result_reverse)
            return result

def dec_to_bin_array(x_arr, int_digit, decimal_digit, decimal_point=False):
    """ given array of float / int -> return string of fixed point array
    
    :param x_arr: an numpy nd array of number
    :param int_digit: the number of digits of integer part.
    :param decimal_digit: the number of digits of decimal part.
    
    return: flatten list of corresponding fixed points
    """
    fixed_point = []
    x_flatten = np.ndarray.flatten(x_arr)
    for i in range(x_flatten.shape[0]):
        fixed_point.append(dec_to_bin(x_flatten[i], int_digit, decimal_digit, decimal_point))
        
    return fixed_point
        
def int_digit(x):
    """given an input x, detect how much integer digit it needs
    x: a arbitary number
    """
    if x >= 32767 or x <= -32768:
        raise Exception("digit out of range for fixed point 16")
    else:
        pass
    
    exp_range = [1, 2, 4, 8, 16, 32, 64, 128, 256, 512, 1024, 2048,
             4096, 8192, 16384,32768]
    
    for i, r in enumerate(exp_range):
        if abs(int(x)) <= r - 1:
            return i
    raise Exception("digit out of range for fixed point 16")
    
    return 0
                        
#########################################
#            internel functions         #
#########################################

def add_1(complement):
    n = len(complement)
    carry = 1
    i = n - 1
    complement = list(complement)

    while i > 0 and carry:
        if complement[i] == '0' and carry == 1:
            complement[i] = '1'
            carry = 0
        if complement[i] == '1' and carry == 1:
            complement[i] = '0'
        i -= 1
        #print(carry)

    return "".join(complement)
    
def decimal_dec_to_bin(x, decimal_digit):
    """
    :param x: is the original number
    :param decimal_digit: the number of digits of decimal part.
    :return:
    """
    x = abs(x)
    x -= int(x)
    bins = ""
    i = 0
    while x and i < decimal_digit:
        x *= 2
        bins += "1" if x >= 1. else "0"
        x -= int(x)
        i += 1
    return bins
