#pragma once

#include<iostream>

const int NDIMS = 3;

//#define _DEBUG

#ifndef _DEBUG
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

// Ignored label i.e. hearts
const int IGNORED_LABEL_NUM = 5;

const std::string TESTTIME_LABEL_NAMES[] = { "liver",
											"spleen",
											"kidneyL",
											"kidneyR",
											"gallbladder",
											"pancreas",
											"aorta",
											"IVC",
											"portal",
											"stomachWall",
											"oesophagus",
											"other" };

const int TESTTIME_NUM_LABELS = sizeof(TESTTIME_LABEL_NAMES) / sizeof(TESTTIME_LABEL_NAMES[0]);

#else
const std::string LABEL_NAMES[] = { "class2", "class1"};

const int NUM_LABELS = sizeof(LABEL_NAMES) / sizeof(LABEL_NAMES[0]);

// This label means the stomach contents.
const int REMOVE_LABEL_NUM = 14;

// Ignored label i.e. hearts
const int IGNORED_LABEL_NUM = 5;

const std::string TESTTIME_LABEL_NAMES[] = { "class2", "class1" };

const int TESTTIME_NUM_LABELS = sizeof(TESTTIME_LABEL_NAMES) / sizeof(TESTTIME_LABEL_NAMES[0]);
#endif

