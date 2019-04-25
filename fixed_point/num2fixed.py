import numpy as np

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

def dec_to_bin(x, int_digit, decimal_digit, decimal_point=False):
    """
    :param x: original number
    :param int_digit: the number of digits of integer part.
    :param decimal_digit: the number of digits of decimal part.
    :return:
    """
    decimal = decimal_dec_to_bin(x, decimal_digit)
    if x >= 0:  # true form = complement
        integer = bin(int(x))[2:]
        if integer == "0":
            integer = ""
        add_zero_int = int_digit - len(integer)
        add_zero_integer_complement ="0" + "0" * add_zero_int + integer
    if x < 0:
        integer = bin(int(x))[3:]
        if integer == "0":
            integer = ""
        add_zero_int = int_digit - len(integer)
        add_zero_integer_true = "1" + "0" * add_zero_int + integer # true form
        
        add_zero_integer_reverse = "1" + "".join(str(0 if int(r) == 1 else 1) for r in add_zero_integer_true[1:]) # radix-minus-one complement
        add_zero_integer_complement = add_1(add_zero_integer_reverse)
        #print(add_zero_integer_complement)

         # radix-minus-one complement
    add_zero_dec = decimal_digit - len(decimal)
    add_zero_dec = decimal + "0" * add_zero_dec
    
    if decimal_point:
        return add_zero_integer_complement + "." + add_zero_dec
    else:
        return add_zero_integer_complement + add_zero_dec

def dec_to_bin_array(x_arr, int_digit, decimal_digit, decimal_point=False):
    """
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
        