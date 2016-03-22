#-------------------------------------------------------------------------------
# Name:        module1
# Purpose:
#
# Author:      James
#
# Created:     16/03/2016
# Copyright:   (c) James 2016
# Licence:     <your licence>
#-------------------------------------------------------------------------------

import argparse
import json
import glob
import os

def main(mini_caffe_folder):

    json_folder = './json'
    obj_folder = './obj'
    errors = 0
    for mini_caffe_prediction in glob.glob(os.path.join(mini_caffe_folder, '*.json')):
        file_name = mini_caffe_prediction.split('\\')[-1]
        file_name = os.path.splitext(file_name)[0]
        with open(mini_caffe_prediction, 'r') as mini_caffe_file:
            mini_caffe_data = json.load(mini_caffe_file)

        mini_caffe_yaw = float(mini_caffe_data['FaceData'][0]['HeadPose']['Yaw'])
        mini_caffe_pitch = float(mini_caffe_data['FaceData'][0]['HeadPose']['Pitch'])
        mini_caffe_roll = float(mini_caffe_data['FaceData'][0]['HeadPose']['Roll'])

        mini_caffe_landmarks = []
        for i in range(32):
            mini_caffe_landmarks.append(float(mini_caffe_data['FaceData'][0]['Landmark32']['Pt_'+str(i).zfill(3)]['X']))
            mini_caffe_landmarks.append(float(mini_caffe_data['FaceData'][0]['Landmark32']['Pt_'+str(i).zfill(3)]['Y']))

        # Read obj file to get gt landmarks
        gt_landmarks = []
        with open(os.path.join(obj_folder, file_name+'.obj')) as obj_file:
            for line in obj_file:
                words = line.split()
                if words[0] == 'v':
                    gt_landmarks.append(float(words[1]))
                    gt_landmarks.append(float(words[2]))

        # Read json file for gt heapose
        with open(os.path.join(json_folder, file_name+'.json')) as gt_json_file:
            gt_data = json.load(gt_json_file)

        gt_yaw = float(gt_data['FaceData'][0]['HeadPose']['Yaw'])
        gt_pitch = float(gt_data['FaceData'][0]['HeadPose']['Pitch'])
        gt_roll = float(gt_data['FaceData'][0]['HeadPose']['Roll'])

        # Check data
        if abs(mini_caffe_yaw-gt_yaw) > 0.00001:
            print 'Problem with the yaw prediction for file ' + mini_caffe_prediction
            errors += 1

        if abs(mini_caffe_pitch-gt_pitch) > 0.00001:
            errors += 1
            print 'Problem with the pitch prediction for file ' + mini_caffe_prediction

        if abs(mini_caffe_roll-gt_roll) > 0.00001:
            errors += 1
            print 'Problem with the roll prediction for file ' + mini_caffe_prediction

        # Check landmark
        for mini_pred, gt_pred in zip(mini_caffe_landmarks, gt_landmarks):
            if abs(mini_pred-gt_pred) > 0.00001:
                errors += 1
                print 'Problem with the landmark prediction for file ' + mini_caffe_prediction

    if errors == 0:
        print 'No errors found !'
    else:
        print str(errors) + ' errors found.'

if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument("input_folder")
    args = parser.parse_args()

    main(args.input_folder)
