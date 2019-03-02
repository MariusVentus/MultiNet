# MultiNet
A simple Neural Net framework with the ability to use heterogeneous layers. 

Over the holidays I had created a rather elaborate C++ framework for a simple 
heterogenous neural network in order to test my understanding of various underlying topics
in the field. However, many aspects of it were unrefined, so this will be a cleaned up recreation.  

DataHandler Plans
>Split Single File
>Clean and Copy File, Read (No Cleaning)
>Random Shuffle, Synced Shuffle

Cortex
>Make the Main Tests a General Function with parameters, so less fixing between test changes.
>Timekeeping

Settings 
>Read in file. 

NN
Gradient Clipping <L2, Hard > 
Xavier Init
Cross Entropy, Log Loss, Softmax
Simple Recurrency
LSTM <Pain the first time, may not do it again given some of the structural changes>