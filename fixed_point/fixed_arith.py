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

def fixed_mul(x1, x2, integer_x1=8, integer_x2=8, integer_result=16):
    """
    """
    pass

    # use 32 bit number to save the temporary result
    # then pick the digits w.r.t. integer_result

    # use fixed_add as infrastructure
def fixed_shift_right(x, shift_size, keep_len=True):
    """
    x: string of unsigned fixed point number
    keep_len: whether maintain the original length
    return:
        shifted unsigned fixed point number (a string)
    """
    length = len(x)
    
    if keep_len:
        result = shift_size * '0' + x[shift_size:]
    else:
        result = x[shift_size:]
        
    return result
    

def relu():
    pass

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