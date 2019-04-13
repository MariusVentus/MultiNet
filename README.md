# MultiNet
A simple Neural Net framework with the ability to use heterogeneous layers. 

Over the holidays I had created a C++ framework for a simple 
heterogenous neural network in order to test my understanding of various underlying topics
in the field. However, many aspects of it were unrefined, so this will be a cleaned-up recreation.  

--Current Features--

Supports Linear, Sigmoid, Tanh, Relu, Gaussian, Bias, and SoftMax Neurons. 
Topology and Settings files can be customized to tailor individual runs. 
Training Data can be fed randomly to the net. A portion of that data can be set aside for testing. 
After Training and Testing, Manual Inputs can be entered to use the newly trained net. 
Input/Ouput Expansion can be used to expand a single-column input or output into a one-hot vector. 
Input Compression ("Smushing") can be used to scale inputs to a value between 0 and 1. Compression cannot be used with expanded inputs. 
Gradient Clipping, both Hard and L2 Normalization variants. 
Xavier Weight Initialization, both Simple and Full variants. 
Error Display can be set to Log Loss or Mean Squared. 
Loss Function can be set to Log Loss (for Cross Entropy) or Mean Squared. 
Simple Recurrency can add basic memory to standard neurons. 

--Plans and Possible Features--

General Plans
>Saving/Loading, 
>Experimental Test Files, 
>Gui(?), 
>Proper Error Handling.
> Display On-hot outs as a number
> Unify Column max, Manual Max

DataHandler Plans
>Split Single File, 
>Clean and Copy File, Read (No Cleaning)(?), 
>Semi-Colon separation for multi-output(?), 
>Improve Randomizer Efficiency.

NN Plans
>Dropout(?), 
>LSTM Neurons(?).

