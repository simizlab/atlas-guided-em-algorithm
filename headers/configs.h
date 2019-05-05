#pragma once

#include<iostream>

const int NDIMS = 3;

const std::string LABEL_NAMES[] = {"liver", 
								  "spleen", 
								  "kidneyL", 
								  "kidneyR", 
								  "heart", 
								  "gallbladder", 
								  "pancreas", 
								  "aorta", 
								  "IVC", 
								  "portal", 
								  "stomachWall",
								  "oesophagus",
								  "other" };

const int NUM_LABELS = sizeof(LABEL_NAMES) / sizeof(LABEL_NAMES[0]);

// This label means the stomach contents.
const int REMOVE_LABEL_NUM = 14;

