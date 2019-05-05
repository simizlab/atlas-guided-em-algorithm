#coding:utf-8
"""
* FileName : ioFunction_version_4_3.py
*
* Read mhd or raw image
* Correspond to SaitoSan "function.h"
* Requistes
*   SimpleITK(1.0.1)
*   numpy(1.11.1)
*   matplotlib (1.5.3)
* Usage:
*   import ioFunction_/*Input version*/ as IO  #Recommend
*   from ioFunction import * #You can use All function
* Usage: sitk
*       GetSize     : image.GetSize()
*       GetOrigin   : image.GetOrigin()
*       GetSpacing  : image.GetSpacing()
*                     image.GetDirection()
*                     image.GetNumberOfComponentsPerPixel()
*           etc....
*
* @auther tozawa
* @history
* 20171201 Change function name
*          from 'write_mhd_and_raw_fromNumpyArray' to 'write_mhd_and_raw_withoutSitk'
* 20171130 Modify code and Add function
* 20171128 Add function(read_raw_to_numpy_ColMajor())
* 20171123 Bug fix( Pluto can read mhd)
*          Change newline code from 'os.linesep' to '\n'
* 20171121 VersionUp !! Version4 Fhooooooooooooooooouuuuuuurururururu(^^)/~~~~~~~~~~~
* 20171121 Bug fix by MiyagawaSan
* 20171024 Insert comment
* 20170920 Change defaulte constractor
* 20170919 "reso" is updated to "float" for python3.5
* 20170712 Make!
"""

import sys, os, copy, time, yaml
import numpy as np
import SimpleITK as sitk
import matplotlib.pyplot as plt

def read_raw(path, dtype):
    """
    path : input image name
    ex. /hoge.raw
    dtype : type of data
    ex. 'float' or 'np.float32' or 'MET_FLOAT'
    return : numpy array
    ----------------------
    np.int8      or char
    np.uint8     or uchar
    np.int16     or short
    np.int32     or int
    np.uint32    or long
    np.float32   or float
    np.float64   or double
    ----------------------
    """
    type = __change2NP(dtype)
    data = np.fromfile(path, type)

    return data

def write_raw(Data, path):
    """
    data : save data as 1D numpy array
    path :  directories + save_name
    ex. /hoge.raw
    """
    data_dir, file_name = os.path.split(path)
    if not os.path.isdir(data_dir):
        os.makedirs(data_dir)
    #Data = Data.astype(self.type)
    if Data.ndim != 1 :
        print("Please check Array dimensions")
        return False
    with open(path, 'wb') as fid:
        fid.write(Data)

    # Origin code
    #fid = open(path, 'wb')
    #fid.write(Data)
    #fid.close()
    return True

def read_mhd_and_raw(path, numpyFlag=True):
    """
    This function use sitk
    path : Meta data path
    ex. /hogehoge.mhd
    numpyFlag : Return numpyArray or sitkArray
    return : numpyArray(numpyFlag=True)
    Note ex.3D :numpyArray axis=[z,y,x], sitkArray axis=(z,y,x)
    """
    img = sitk.ReadImage(path)
    if not numpyFlag:
        return img

    nda = sitk.GetArrayFromImage(img) #(img(x,y,z)->numpyArray(z,y,x))
    return nda

def write_mhd_and_raw(Data,path):
    """
    This function use sitk
    Data : sitkArray
    path : Meta data path
    ex. /hogehoge.mhd
    """
    if not isinstance(Data, sitk.SimpleITK.Image):
        print('Please check your ''Data'' class')
        return False

    data_dir, file_name = os.path.split(path)
    if not os.path.isdir(data_dir):
        os.makedirs(data_dir)

    sitk.WriteImage(Data, path)

    return True

class metaImageHeader(object):
    def __init__(self, **kwarg):
        """
        * This metaImageHeader is a minimum standard
        *
        Sample Code1 :
            a = metaImageHeader(
                    ObjectType = 'Image',
                    NDims = 3,
                    BinaryDataByteOrderMSB = 'False',
                    ElementSpacing = [1.,1.,1.],    # Recommend list(Assmue list)
                    DimSize = [100,100,100],        # Recommend list
                    ElementType = 'MET_SHORT',
                    ElementDataFile = 'hogehoge.raw'
            )

        Sample Code2 : 'When you want to use dictionary object as argument'
            dict = {'hoge' : 'hogehoge'}
            a = metaImageHeader( **dict)

        * Regacy Code
        #self.objectType = {'ObjectType':'Image'}
        #self.nDims = {'NDims':3}
        #self.binaryDataByteOrderMSB = {'BinaryDataByteOrderMSB':False} # Omake
        #self.elementSpacing ={'ElementSpacing':[1.,1.,1.]} # Omake
        #self.dimSize = {'DimSize':[100,100,100]}
        #self.elementType = {'ElementType':'MET_SHORT'}
        #self.elementDataFile = {'ElementDataFile':'hogehoge.raw'}
        """
        self.metaImageHeader = kwarg

    def write_metaImageHeader(self, path):
        """
        path : Meta Data path
        ex. /hogehoge.mhd
        * MEMO : About Cross platform
        'Newline Code' : os.linesep(Dont use this function when you open file as "txt mode")
        'https://www.pythonweb.jp/tutorial/dictionary/index7.html'
        """
        # Write mhd
        data_dir, file_name = os.path.split(path)
        if os.path.isdir(data_dir) == False:
            os.makedirs(data_dir)

        # Check metaImageHeader Config.
        # If not, Set default value.
        if 'ObjectType' not in self.metaImageHeader.keys():
            self.metaImageHeader['ObjectType'] = 'Image'
        if 'NDims' not in self.metaImageHeader.keys():
            self.metaImageHeader['NDims'] = 3
        if 'BinaryDataByteOrderMSB' not in self.metaImageHeader.keys():
            self.metaImageHeader['BinaryDataByteOrderMSB'] = 'False'
        if 'ElementSpacing' not in self.metaImageHeader.keys():
            self.metaImageHeader['ElementSpacing'] = [1.,1.,1.]
        if 'DimSize' not in self.metaImageHeader.keys():
            self.metaImageHeader['DimSize'] = [100,100,100]
        if 'ElementType' not in self.metaImageHeader.keys():
            self.metaImageHeader['ElementType'] = 'MET_SHORT'
        if 'ElementDataFile' not in self.metaImageHeader.keys():
            self.metaImageHeader['ElementDataFile'] = os.path.splitext(file_name)[0] + '.raw'

        if self.metaImageHeader['NDims'] != len(self.metaImageHeader['ElementSpacing']) \
                    or self.metaImageHeader['NDims'] != len(self.metaImageHeader['DimSize']):
            return False

        with open(path, 'w') as fid:
            fid.write('ObjectType = ' + self.metaImageHeader['ObjectType'])
            fid.write('\nNDims = ' + str(self.metaImageHeader['NDims']))
            fid.write('\nBinaryDataByteOrderMSB = ' + str(self.metaImageHeader['BinaryDataByteOrderMSB']))
            fid.write('\nElementSpacing =')
            for i in self.metaImageHeader['ElementSpacing']:
                fid.write(' ' + str(i))
            fid.write('\nDimSize =')
            for i in self.metaImageHeader['DimSize']:
                fid.write(' ' + str(i))
            fid.write('\nElementType = ' + str(self.metaImageHeader['ElementType']))
            fid.write('\nElementDataFile = ' + str(self.metaImageHeader['ElementDataFile']))

        return True

def write_mhd_and_raw_withoutSitk(Data, path, ndims, size=[], space=[]):
    """
    Data : numpy 1D Array
    path : Meta data path
    ex. /hogehoge.mhd
    ndims : Dimensions
    size : Image Size (Assume list)
    ex. [x_size, y_size, z_size]
    space : Image resolution (Assume list)
    If not, Set all resolution as 1.mm
    ex. [x_reso, y_reso, z_reso]
    """
    if not isinstance(Data, np.ndarray):
        print('Please check your ''Data'' class')
        return False

    if ndims != len(size) \
        or not isinstance(size, list) or not isinstance(space, list):
        print('Please check ''Ndims, # of size'' ')
        print('Please check size and space data type(list type only) ')
        return False

    _space = [1. if len(space) != ndims else space[i] for i in range(ndims)]

    type = __change2Ele(Data.dtype)
    a = metaImageHeader(
            NDims=ndims,
            ElementSpacing=_space,
            DimSize=size,
            ElementType=type)

    if not a.write_metaImageHeader(path):
        print('Write metaImageHeader Error')
        return False

    data_dir, file_name = os.path.split(path)
    write_raw(Data.copy(), data_dir + '/'+ os.path.splitext(file_name)[0] + '.raw')

    return True


def read_mhd_and_raw_withoutSitk(path):
    """
    This function dont use sitk
    "https://stackoverflow.com/questions/14505898/how-to-make-a-dictionary-from-a-text-file-with-python"
    path : Meta data path
    ex. /hogehoge.mhd
    return : numpyArray, metaImageHeader(dictionary)
    ex. You want to extract 'DimSize' from dictionary
    >>> npArray, dict = read_mhd_and_raw_withoutSitk("./hogehoge.mhd")
    >>> print(dict['DimSize'])
    [x_size, y_size, z_size]
    # you want to get x_size
    >>> print(dict['DimSize'][0])
    x_size

    """
    with open(path, 'rt') as f:
        dict = {}
        for line in f:
            line = line.split()
            if not line : continue #empty line
            dict[line[0]] = line[2:]

    if 'ObjectType' in dict.keys():
        dict['ObjectType'] = str(dict['ObjectType'][0])
    if 'BinaryDataByteOrderMSB' in dict.keys():
        dict['BinaryDataByteOrderMSB'] = str(dict['BinaryDataByteOrderMSB'][0])
    if 'ElementType' in dict.keys():
        dict['ElementType'] = str(dict['ElementType'][0])
    if 'ElementDataFile' in dict.keys():
        dict['ElementDataFile'] = str(dict['ElementDataFile'][0])
    if 'NDims' in dict.keys():
        dict['NDims'] = int(dict['NDims'][0])
    if 'DimSize' in dict.keys():
        dict['DimSize'] = list(map(int, dict['DimSize']))
    if 'ElementSpacing' in dict.keys():
        dict['ElementSpacing'] = list(map(float, dict['ElementSpacing']))

    type = __change2NP(dict['ElementType'])
    data_dir, file_name = os.path.split(path)
    nda = read_raw(data_dir + '/'+ os.path.splitext(file_name)[0] + '.raw', type)

    return nda, dict




def __change2NP(type):
    """
    return : numpy data type
    type : type of data
    ----------------------
    np.int8      or char   or MET_CHAR
    np.int16     or short  or MET_SHORT
    np.int32     or int    or MET_INT
    np.float32   or float  or MET_FLOAT
    np.float64   or double or MET_DOUBLE
    np.uint8     or uchar  or MET_UCHAR
    np.uint16    or ushort or MET_USHORT
    np.uint32    or uint   or MET_UINT
    ----------------------
    """
    if isinstance(type, str):
        if (type == "char") or (type == "MET_CHAR"):
            return np.int8
        elif (type == "short") or (type == "MET_SHORT"):
            return np.int16
        elif (type == "int") or (type == "MET_INT"):
            return np.int32
        elif (type == "float")  or (type == "MET_FLOAT"):
            return np.float32
        elif (type == "double")  or (type == "MET_DOUBLE"):
            return np.float64
        elif (type == "uchar")  or (type == "MET_UCHAR"):
            return np.uint8
        elif (type == "ushort")  or (type == "MET_USHORT"):
            return np.uint16
        elif (type == "uint")  or (type == "MET_UINT"):
            return np.uint32
        else:
            print ("korakora!")
            quit()
    else:
        if  (type == np.int8) :
            return np.int8
        elif (type == np.int16):
            return np.int16
        elif (type == np.int32) :
            return np.int32
        elif (type == np.float32) :
            return np.float32
        elif  (type == np.float64) :
            return np.float64
        elif (type == np.uint8):
            return np.uint8
        elif (type == np.uint16):
            return np.uint16
        elif (type == np.uint32) :
            return np.uint32
        else:
            print ("korakora!")
            quit()

def __change2Ele(type):
    """
    return : MHD data type
    type : type of data
    ----------------------
    np.int8      or char   or MET_CHAR
    np.int16     or short  or MET_SHORT
    np.int32     or int    or MET_INT
    np.float32   or float  or MET_FLOAT
    np.float64   or double or MET_DOUBLE
    np.uint8     or uchar  or MET_UCHAR
    np.uint16    or ushort or MET_USHORT
    np.uint32    or uint   or MET_UINT
    ----------------------
    """
    if isinstance(type, str):
        if (type == "char")  or (type == "MET_CHAR"):
            return "MET_CHAR"
        elif (type == "short")  or (type == "MET_SHORT"):
            return "MET_SHORT"
        elif (type == "int")  or (type == "MET_INT"):
            return "MET_INT"
        elif (type == "float")  or (type == "MET_FLOAT"):
            return "MET_FLOAT"
        elif (type == "double")  or (type == "MET_DOUBLE"):
            return "MET_DOUBLE"
        elif (type == "uchar")  or (type == "MET_UCHAR"):
            return "MET_UCHAR"
        elif (type == "ushort")  or (type == "MET_USHORT"):
            return "MET_USHORT"
        elif (type == "uint")  or (type == "MET_UINT"):
            return "MET_UINT"
        else:
            print ("korakora!")
            quit()
    else:
        if (type == np.int8):
            return "MET_CHAR"
        elif (type == np.int16):
            return "MET_SHORT"
        elif (type == np.int32):
            return "MET_INT"
        elif (type == np.float32):
            return "MET_FLOAT"
        elif (type == np.float64):
            return "MET_DOUBLE"
        elif (type == np.uint8) :
            return "MET_UCHAR"
        elif (type == np.uint16):
            return "MET_USHORT"
        elif (type == np.uint32):
            return "MET_UINT"
        else:
            print ("korakora!")
            quit()

def read_raw_to_numpy(path, dtype, row_size):
    """
    Note One Col correspond to one patch.
    It is same shape as the SaitoSan's "function.h( load_raw_to_eigen() )"
    path :  file name path(full path)
        ex. /hoge.raw
    dtype : data type
        ex. 'float' or np.float32 or 'MET_FLOAT'
    row_size : Patch Size(!!! One Patch Volume Size !!!)
    return : numpyArray(row_size, col_size)
        example: 2 Patches, PatchSize=8
        >>> x.shape
        (8,2)
        >>> x
        [[  0.   8.]
         [  1.   9.]
         [  2.  10.]
         [  3.  11.]
         [  4.  12.]
         [  5.  13.]
         [  6.  14.]
         [  7.  15.]]
        If you want to get one patch
        >>> a = x[:,0]
        >>> a
        [0, 1,  2,  3,  4,  5,  6,  7]

    """
    nda = read_raw(path, dtype)
    col_size = int(nda.size/row_size)

    nda = nda.reshape(row_size, col_size)

    return nda

def read_raw_to_numpy_ColMajor(path, dtype, row_size):
    """
    Note One Col correspond to one patch.
    It is same shape as the SaitoSan's "function.h( load_raw_to_eigen_ColMajor() )"
    path :  file name path(full path)
        ex. /hoge.raw
    dtype : data type
        ex. 'float' or np.float32 or 'MET_FLOAT'
    row_size : Patch Size(!!! One Patch Volume Size !!!)
    return : numpyArray(row_size, col_size)
    """
    nda = read_raw(path, dtype)
    col_size = int(nda.size/row_size)

    nda = np.reshape(nda, (row_size, col_size), order='F')

    return nda

def save_zSlice(z, Data, path, showFlag=False):
    """
    z : Slice number
    Data : 3 dimension array(recommend sitk array)
    path :
    ex . /hoge.png
    ##"http://python-remrin.hatenadiary.jp/entry/2017/05/27/114816#"
    """
    if isinstance(Data, sitk.SimpleITK.Image):
        #print("Please check your 'Data' class")
        if(Data.GetDimension() != 3):
            print("Please check your 'Data' Dimensions")
            return False
        slice = sitk.GetArrayViewFromImage(Data)[z,:,:]
    else:
        if Data.ndim != 3:
            print("Please check your 'Data' Dimensions")
            return False
        slice = Data[z,:,:].copy()

    data_dir, file_name = os.path.split(path)
    if os.path.isdir(data_dir) == False:
        os.makedirs(data_dir)

    plt.imshow(slice, cmap='gray', interpolation='none')
    plt.axis('off')
    plt.savefig(path, dpi=300)
    if showFlag:
        plt.show()

    return True

def save_args(output_dir, args):
    if not os.path.exists(output_dir):
        os.makedirs(output_dir)

    with open('{}/config_{}.yml'.format(output_dir, time.strftime('%Y-%m-%d_%H-%M-%S')), 'w') as f:
        f.write(yaml.dump(vars(args), default_flow_style=False))
