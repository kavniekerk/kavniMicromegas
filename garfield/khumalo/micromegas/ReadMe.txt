So first things first I made a solid box with the dimensions similar to that of the anticipated detector (80cm x 40 cm x 10cm). 
This is defined under the Medium class using the SolidBox function. 3 gas files with the gas pressure at 20 mbar,0.5 bar and 1 bar were created.
Making these gas files took about 12 hours CPU time. The range of the electric field used in these gases is 100-100000V/cm. The class MediumMagboltz 
which is an interface to the program Magboltz is used to access the gas files together with the ion mobilities. These are important for the drift velocity 
calculation.

Coming to the components of the detector, that is the wires, planes, strips etc. 
The Component class was used to define two planes one HV and another grounded plane.
The voltage used was -3500 V for the HV plane . 
Some voltage was also put on the wire (600 V ) so as to act as an amplifier (still reading on this so as to understand better how it really works).


Three types of components are defined: planes,a wire and strips. There are 3 strips that are defined in the grounded Y plane.
 Defining these strips will help to see how the signal forms on these strips which will help in understanding
 how the signal will form in the segmented cathode pads which are considered to be used for signal readout in the new detector. 

There is the sensor class which is responsible for telling the program,for which defined component should the weighting 
field be calculated. It is also responsible for the calculation of the signal that is formed on that component due to the drift
of ions or electrons. In this case 3 different sensor functions are defined for the 3 different strips.

For the drift of electrons or ions the DriftLineRKF class is used which uses the Runge-Kutta-Fehlberg integration. The 
position at which the charged particles start drifting is specified by the user.I tried to use the AvalancheMC class because it also 
serves the same purpose by using Monte Carlo integration,  but everytime i used this class GARFIELD would crash. 
The AvalancheMC is good for microscopic tracking of the electrons in the detector. So I am still trying to find out why GARFIELD crashed whenever
I used this class. Another thing I hope to look into is using the GetMedium function so I can see where the ionizable medium begins and ends which 
will be very useful when defining the strips and the starting point of the drift of an electron or ion.

Another class which is important is the Track class which I used to track a particle through the detector. For now I have only put 2 particles,
alpha and proton. Unfortunately there are few particles that are defined in the program and there are only two I can use for the simulations.
The TrackHeed class is the one that used, which is an interface to the program Heed. For the track , I use the function GetCluster to get the cluster
that is left behind by the ionizing incoming particle. To understand the ionization left behind  by the particle as it traverses the gas, that is along its 
trajectory a probability distribution of the number of electrons that are released must be known. To have an idea of the distribution, the energy loss spectra 
must be known hence I plot this under the Track section. The probability of producing electrons for a certain number of events must also be known hence i plot
the number of electrons by the particle in the track class. 

Currently Iam having  a problem with plotting the drift lines of the electrons/ ions an empty Canvas shows up. I dont know what is wrong but am sure I will 
figure it as time goes on. This might just a plotting problem because if the electrons were not drifting then there will not be any signal plotting. 


The AvalancheMicroscopic section is included but am not sure if it is doing anything, because commenting it does not make any difference in the plots. I think its inclusion after the DriftLineRKF class nullifies it ( not sure for now ).
I have 3 signals now, for the 3 strips and I attached them. 


































 




