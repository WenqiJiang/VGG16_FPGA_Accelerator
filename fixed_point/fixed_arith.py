import numpy as np

# from fixed_arith import fixed_mul, relu, fixed2float, unsigned_fixed_add, fixed_add, \
#      dec_to_bin, dec_to_bin_array, int_digit, fixed_add_array

####################################################
#            api functions for other files         #
####################################################

def fixed_mul(x1, x2, integer_x1=8, integer_x2=8, integer_result=8):
    """
    fixed point multiplication
    input: x1, x2 -> 16 bit fixed point
    integer_x1 / x2 : where is the input decimal point
    integer_result: where is the result decimal point
    return:
        result -> 16 bit fixed point
    """
    adders = []
    
    fill_x1 = _helper_filler1(x1)
    fill_x2 = _helper_filler1(x2)
    
    # save all adders
    for i in range(32):
        if fill_x1[i] == '1':
            before = i
            after = 31 - i
            filled_num = _helper_filler2(fill_x2, before, after)
            adders.append(filled_num)

    # add them up, a 30 bit number
    add_result = _helper_mul_add(adders)
    cut_result = add_result[32:]
    
    int_part = integer_x1 + integer_x2
    new_int_start = int_part - integer_result + 1 # sign 1 bit
    
    result = cut_result[new_int_start: new_int_start + 16]
    
    return result

def relu(x):
    if x[0] == '1':
        return '0' * len(x)
    else:
        return x

def fixed2float(x, integer_x=8):
    """fixed point to floating point
    x: string of signed fixed point number 
    integer_x: how many digits do we give to integer part
        e.g. sign 1 bit, integer 8 bits, decimal 7 bits
    return:
        a floating point number
    """
    if x[0] == '0':
        sign = 1
    else:
        sign = -1
    
#     print("x: {}".format(x))
    unsigned = 0
    shift_size = len(x) - 1 - integer_x # == demical digits number
    for i in range(len(x) - 1): # e.g. 15
        if x[i + 1] == '1': # ignore sign 
            new_fl = 2 ** (len(x) - 2 - i -shift_size)
            unsigned += new_fl
    
    if sign == 1:
        result = unsigned
    else:
        result = unsigned - 32768 * (2 ** (-shift_size))
    
    return result

def unsigned_fixed_add(x1, x2):
    """ unsigned fixed point add
    x1, x2: string of signed fixed point number,
            they must have the same length, e.g. 15.
    return:
        a tuple (carry_flag, fixed_result)
        carry_flag: 0 or 1
        fixed_result: string which has same bit as inputs
    """
    assert len(x1) == len(x2)
    carry = 0
    fixed_result = ''
    
    for i in reversed(range(len(x1))):
        if x1[i] == '0' and x2[i] == '0':
            if carry == 1:
                fixed_result += '1'
            else:
                fixed_result += '0'
            carry = 0
        elif x1[i] == '1' and x2[i] == '1':
            if carry == 1:
                fixed_result += '1'
            else:
                fixed_result += '0'
            carry = 1
        else: # one 1 and one 0
            if carry == 1:
                fixed_result += '0'
                carry = 1
            else:
                fixed_result += '1'
                carry = 0
    
    return (carry, fixed_result[::-1])

def fixed_add_array(adder_array, integer=8):
    """ given a list of fixed point 16, return the sum
    x_arr: list of fp16 strings
    integer: number of digits
    return:
        a single fp16
    """
    if len(adder_array) == 0:
        return '0' * 16
    elif len(adder_array) == 1:
        return adder_array[0]
    else:
        last_ele = adder_array[0]
        
        for i in range(1, len(adder_array)):
            this_carry, temp_result = unsigned_fixed_add(adder_array[i], last_ele)
#             if this_carry:
#                 raise Exception("overflow detected in function 'fixed_add_array'")
            last_ele = temp_result
        
        return last_ele
    
def fixed_add(x1, x2, integer_x1=8, integer_x2=8):
    """fixed point add 
    x1, x2: string of signed fixed point number 
    integer_xi: how many digits do we give to integer part
        e.g. sign 1 bit, integer 8 bits, decimal 7 bits
    return:
        a tuple (signed_fixed_result, integer_result)
        integer_result means how many digits does the integer part have
    """
    assert len(x1) == len(x2) and len(x1) == 16
    sign1 = x1[0]
    sign2 = x2[0]
    unsigned1 = x1[1:]
    unsigned2 = x2[1:]
    
    if integer_x1 > integer_x2:
        delta = integer_x1 - integer_x2
        # shift x2
        unsigned2 = fixed_shift_right(unsigned2, shift_size=delta)
    elif integer_x1 < integer_x2:
        delta = integer_x2 - integer_x1
        # shift x1
        unsigned1 = fixed_shift_right(unsigned1, shift_size=delta)
    else:
        pass
    
    integer_result = max(integer_x1, integer_x2)
    
    adjusted1 = sign1 + unsigned1
    adjusted2 = sign2 + unsigned2
    
#     print("adjusted:{}\t{}".format(adjusted1, adjusted2))
    # result is a fixed point number with a size of 17
    overflow, result = unsigned_fixed_add(adjusted1, adjusted2)
    if overflow:
        raise Exception("overflow detected")
    
    return result

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
            raise Exception("negative numbers doesn't support decimal point currently")
        else:
            temp_result = add_zero_integer_complement + add_zero_dec
            temp_result_reverse = "".join(str(0 if int(r) == 1 else 1) for r in temp_result) # radix-minus-one complement
#             print("reverse: ", temp_result_reverse)
            result = add_1(temp_result_reverse)
            return result

def dec_to_bin_array(x_arr, int_digit=8, decimal_digit=7, decimal_point=False):
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

def _helper_mul_add(adder_array):
    """ add an array of 63-bit fixed point number
    return:
        a 64-bit fixed point number
    """
    if len(adder_array) == 0:
        return '0' * 64
    elif len(adder_array) == 1:
        return '0' + adder_array[0]
    else:
        carry = 0
        last_ele = adder_array[0]
        
        for i in range(1, len(adder_array)):
            this_carry, temp_result = unsigned_fixed_add(adder_array[i], last_ele)
            carry += this_carry
            last_ele = temp_result
            
        if carry == 0:
            result = '0' + last_ele
        else:
            result = '1' + last_ele
        assert len(result) == 64
        
        return result

def _helper_filler1(x):
    """
    fill a 16 bit fixed point to 32 bit,
    0 or 1 depends on the sign bit
    x: input 16bit str
    return:
        a 32 bit str
    """
    if x[0] == '0':
        return 16 * '0' + x
    else: # '1'
        return 16 * '1' + x
    
def _helper_filler2(x, int_before=15, int_after=16):
    """
    fill 32 bit number to 61 bits, fill 0s
    x: 32bit str
    return:
        63 bit str
    """
    return int_before * '0' + x + int_after * '0'


def reduct_one(x):
    """given a fixed point number, reduct one from it
    x: input string of fixed point
    return:
        string of fixed point
    """
    result = '' 
    carry = 0
    
    # reduct 1 and if carry == 0, break
    for i in reversed(range(len(x))):
        if x[i] == '0':
            if carry == 0:
                result = '1' + result
                carry = 1
            else: # carry == 1
                result = '0' + result
                carry = 1
        elif x[i] == '1':
            if carry == 0:
                result = '0' + result
                # copy rest
                for j in reversed(range(i)):
                    result = x[j] + result
                break
            else: # carry == 1
                result = '0' + result
                # copy rest
                for j in reversed(range(i)):
                    result = x[j] + result
                break
#     print("result:\t\t{}".format(result))
    return result

def reverse(x):
    """given a fixed point number, reverse 0 and 1
    x: input string of fixed point
    return:
        string of fixed point
    """
    result = ''
    for i in range(len(x)):
        if x[i] == '0':
            result = result + '1'
        else:
            result = result + '0'
    
#     print("reversed:\t{}".format(result))
    return result
        
def fixed_shift_right(x, shift_size, keep_len=True):
    """
    x: string of unsigned fixed point number
    keep_len: whether maintain the original length
    return:
        shifted unsigned fixed point number (a string)
    """
    length = len(x)
    
    if keep_len:
        if x[0] == '0':
            result = shift_size * '0' + x[shift_size:]
        else:
            result = shift_size * '1' + x[shift_size:]
    else:
        result = x[shift_size:]
        
    return result
