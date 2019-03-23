# MultiNet
A simple Neural Net framework with the ability to use heterogeneous layers. 

Over the holidays I had created a C++ framework for a simple 
heterogenous neural network in order to test my understanding of various underlying topics
in the field. However, many aspects of it were unrefined, so this will be a cleaned-up recreation.  

--Current Features--

Supports Linear, Sigmoid, Tanh, Relu, Gaussian, Bias, and SoftMax Neurons. 
Topology and Settings files can be customized to tailor individual runs. 
Training Data can be fed randomly to the net. A portion of that data can be set aside for testing. 
Gradient Clipping, both Hard and L2 Normalization variants. 
Xavier Weight Initialization, both Simple and Full variants. 
Error Display can be set to Log Loss or Mean Squared. 
Loss Function can be set to Log Loss (for Cross Entropy) or Mean Squared. 
Simple Recurrency can add basic memory to standard neurons. 

--Plans and Possible Features--

General Plans
>Saving/Loading, 
>Experimental Test Files, 
>Manual Test Input, 
>Gui(?), 
>Proper Error Handling.

DataHandler Plans
>Split Single File, 
>Clean and Copy File, Read (No Cleaning)(?), 
>Smush Inputs by their highest value, 
>Split Outputs by highest value, 
>Semi-Colon separation for multi-output(?), 
>Improve Randomizer Efficiency.

NN Plans
>Dropout(?), 
>LSTM Neurons(?).

