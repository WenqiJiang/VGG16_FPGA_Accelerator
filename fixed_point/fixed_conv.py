import numpy as np

from fixed_arith import fixed_mul, fixed_add_array

def conv_forward(x, w, b, pad, stride):
    """
    A Numpy implementation of 2-D image convolution.
    By 'convolution', simple element-wise multiplication and summation will suffice.
    The border mode is 'valid' - Your convolution only happens when your input and your filter fully overlap.
    Another thing to remember is that in TensorFlow, 'padding' means border mode (VALID or SAME). For this practice,
    'pad' means the number rows/columns of zeroes to to concatenate before/after the edge of input.

    Inputs:
    :param x: Input data. Should have size (batch, height, width, channels).
    :param w: Filter. Should have size (num_of_filters, filter_height, filter_width, channels).
    :param b: Bias term. Should have size (num_of_filters, ).
    :param pad: Integer. The number of zeroes to pad along the height and width axis.
    :param stride: Integer. The number of pixels to move between 2 neighboring receptive fields.

    :return: A 4-D array. Should have size (batch, new_height, new_width, num_of_filters).

    Note:
    To calculate the output shape of your convolution, you need the following equations:
    new_height = ((height - filter_height + 2 * pad) // stride) + 1
    new_width = ((width - filter_width + 2 * pad) // stride) + 1
    For reference, visit this website:
    https://adeshpande3.github.io/A-Beginner%27s-Guide-To-Understanding-Convolutional-Neural-Networks-Part-2/
    """
    batch, height, width, channels = x.shape
    num_of_filters, filter_height, filter_width, channels_f = w.shape
    assert channels == channels_f
    
    new_height = int(np.floor((height - filter_height + 2 * pad) / stride) + 1)
    new_width = int(np.floor((width - filter_width + 2 * pad) / stride) + 1)
    A = np.zeros((batch, new_height, new_width, num_of_filters))
    x_pad = np.zeros((batch, height + 2*pad, width+2*pad, channels))
    
    for bt in range(batch):
        for i in range(height):
            for j in range(width):
                for cn in range(channels):
                    x_pad[bt,i+pad,j+pad,cn] = x[bt,i,j,cn]
                    
    for bt in range(batch):
        for ft in range(num_of_filters):
            for i in range(new_height):
                for j in range(new_width):
                    A[bt,i,j,ft] = b[ft] + np.sum(w[ft,:,:,:] * 
                                   x_pad[bt, i*stride: i*stride + filter_height,j * stride: j*stride + filter_width,:])
    
    return A            

def fixed_conv(x, w, b, int_x=8, int_w=8, int_b=8):
    """ fixed point conv
    pad width = 1 -> for vgg only
    stride = 1 -> for vgg only
    
    x: 3d array of 16fp strings (height, width, channels)
    w: 4d array of 16fp strings (num_of_filters, filter_height, filter_width, channels),
        where filter_height = filter_width = 3
    b: 1d array (num_of_filters, )
    
    int_x/w/b: how many digits for integer parts
    
    result:
        out: 3d array, has the same shape as x
    """
    height, width, channels = x.shape
    num_filters, filter_height, filter_width, _ = w.shape
    
    # create result list
    result = a = np.zeros((height, width, num_filters), dtype='<U16')
    
    # padding x
    x_pad = np.zeros((height + 2, width + 2, channels), dtype='<U16')
    x_pad[1:-1,1:-1,:] = x
    x_pad[0,:] = '0' * 16
    x_pad[-1,:] = '0' * 16
    x_pad[:,0] = '0' * 16
    x_pad[:,-1] = '0' * 16
    
    for i in range(num_filters):
        for j in range(height):
            for k in range(width):
                # now this corresponding to result[i][j][k]
                adder_array = []
                for fh in range(filter_height):
                    for fw in range(filter_width):
                        for ch in range(channels):
#                             print(w[i, fh, fw, ch], x_pad[j + fh, k + fw, ch])
                            adder_array.append(fixed_mul(w[i, fh, fw, ch], x_pad[j + fh, k + fw, ch], integer_x1=8, integer_x2=8, integer_result=8))
                ############## not implemented yet ###############
                result[j,k,i] = fixed_add_array(adder_array, integer=8)
    
    return result