# Network Design and Performance Evaluation with Simulations
This is the repository for the LinkedIn Learning course Network Design and Performance Evaluation with Simulations. The full course is available from [LinkedIn Learning][lil-course-url].

![Network Design and Performance Evaluation with Simulations][lil-thumbnail-url] 

Regardless of the size, form, or implementation of a network, a fundamental goal of any network design is to meet the key performance metrics. In this course, Ryan Hu shows you the principles of network design, topologies, and performance evaluation, and how to test your design with network simulations. Ryan shows you how to create and evaluate simulated networks with ns-3 scripts, leveraging network technologies like Ethernet, Wi-Fi, and 5G, and provides hands-on exercises and challenges so you can apply the basic principles of network performance evaluation in your custom simulations.

_See the readme file in the main branch for updated instructions and information._
## Instructions
This repository has all files used in the course. Specifically, two packages are used: ns-allinone-3.35 package used for most videos and ns3-mmwave-4.0 package used only for 03_08 video. The exercises files can be found in ns-allinone-3.35->Exercises. 

## Installing
1. To use these exercise files, you must check the prerequisites for ns-3:
	- ns-3 documentation: https://www.nsnam.org/wiki/Installation
	- Optionally, to check the PCAP trace files, [WireShark](https://www.wireshark.org) was used. 
2. Clone this repository into your local machine using the terminal (Mac), CMD (Windows), or a GUI tool like GitHub Desktop.
3. Build the ns-3.35 pacakge and ns3-mmwave 4.0 package, respectively.

The packages can be built on different OS's following the ns-3 documentation. For the course videos, the pacakges were tested to be able to be built on macOS Big Sur. Suppose the pacakges are available under <code>Downloads/ns-allinone-3.35/</code> and <code>Downloads/ns3-mmwave-4.0/</code> directories. The following commands were used to build the ns-3.35 package on macOS Big Sur:
- <code>$ cd ns-3.35 </code>
- <code>$ ./waf configure </code>
- <code>$ ./waf --enable-examples --enable-tests </code>

To build the ns3-mmwave-4.0 pacakage, the following commands were used:
- <code>$ cd ns-3.35 </code>
- <code>$ CXXFLAGS="-Wall" ./waf configure </code>
- <code>$ ./waf --enable-examples --enable-tests </code>

The usage of each exercise file can be seen from the course videos, as well as the source code of the files.

Users may use a future release of ns-3 or ns-3 mmWave extension to run the exercise files. In this case, the Exercises folder will need to be copied from the <code>ns-allinone-3.35/Exercises</code> folder.

### Instructor

Ryan Hu 
                            
Professor and Professional Engineer

                            

Check out my other courses on [LinkedIn Learning](https://www.linkedin.com/learning/instructors/ryan-hu).

[lil-course-url]: https://www.linkedin.com/learning/network-design-and-performance-evaluation-with-simulations
[lil-thumbnail-url]: https://cdn.lynda.com/course/2479117/2479117-1655494151120-16x9.jpg
