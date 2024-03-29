# MultiNet
A simple Neural Net framework with the ability to use heterogeneous layers. 

Over the holidays I had created a C++ framework for a simple 
heterogenous neural network in order to test my understanding of various underlying topics
in the field. However, many aspects of it were unrefined, so this is the "cleaned-up" recreation.

It's still mostly a testbed for ideas and improving my programming, and is far from actually being "cleaned-up," but it does hold a special place in my heart as my first full-fledged NN App. 
If you've found it, I hope it helps you as much as it helped me.

--Current Features--

Supports Linear, Sigmoid, Tanh, Relu, Gaussian, Bias, Sine, and SoftMax Neurons. 
Topology and Settings files can be customized to tailor individual runs. 
Training Data can be fed randomly to the net. A portion of that data can be set aside for testing. 
After Training and Testing, Manual Inputs can be entered to use the newly trained net. 
Input/Ouput Expansion can be used to expand a single-column input or output into a one-hot vector. 
Input Compression ("Smushing") can be used to scale inputs to a value between 0 and 1. Compression cannot be used with expanded inputs. 
Gradient Clipping, both Hard and Gradient Normalization variants.
Xavier Weight Initialization, both Simple and Full variants. 
Error Display can be set to Log Loss or Mean Squared. 
Loss Function can be set to Log Loss (for Cross Entropy) or Mean Squared. 
Simple Recurrency can add basic memory to standard neurons (Single Neuron remembers it's last output). 
Restrict Linear Memory can help prevent NANs by stopping Linear and Relu Neurons from having memory, which can balloon to infinity. 
Simple Save/Loading

--Plans and Possible Features--

General Plans
>Experimental Test Files, 
>Gui(?), 
>Proper Error Handling.

DataHandler Plans
>Split Single File, 
>Clean and Copy File, Read (No Cleaning)(?), 
>Semi-Colon separation for multi-output(?), 
>Improve Randomizer.

NN Plans
>LSTM Neurons(?).

