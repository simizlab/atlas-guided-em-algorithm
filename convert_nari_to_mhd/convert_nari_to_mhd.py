# coding:utf-8
import os, sys, time
import argparse, glob
import numpy as np
import utils.ioFunctions as IO
import SimpleITK as sitk

def save_mhd(img, dict, output_dir, filename):
    sitkImg = sitk.GetImageFromArray(img)
    sitkImg.SetSpacing(dict['ElementSpacing'])
    path_tmp = '{}/{}'.format(output_dir, filename)
    sitk.WriteImage(sitkImg, path_tmp)

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--input_dir', '-i', help='input directory')
    parser.add_argument('--output_dir', '-o', help='output directory')
    args = parser.parse_args()

    print('----- Save configs -----')
    result_dir = args.output_dir
    IO.save_args(result_dir, args)

    print('----- Read data -----')
    input_dir = args.input_dir
    fnames = glob.glob('{}/*.mhd'.format(input_dir))

    print('----- Start rename and change mhd -----')
    for fname in fnames:
        filename = os.path.splitext(os.path.basename(fname))[0]
        case_num = int(filename.split('_')[-1])
        img, dict = IO.read_mhd_and_raw_withoutSitk(fname)
        img = img.reshape(dict['DimSize'][2], dict['DimSize'][1], dict['DimSize'][0])
        save_mhd(img, dict, result_dir, filename='origin_{:03d}.mhd'.format(case_num))
