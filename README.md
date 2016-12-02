# ailawd     

[![Language](https://img.shields.io/badge/Language-GCC-green.svg)](https://gcc.gnu.org/) 
[![Author](https://img.shields.io/badge/author-Nicol%20TAO-blue.svg)](https://taozj.org/) 
[![License](https://img.shields.io/badge/license-BSD-red.svg)](http://yanyiwu.mit-license.org)


This is mostly focus on general http web server development based on Boost.Asio!   
For some purpose, c++0x branch is actively recently.   

The performance worked as:    
Core(TM) i5-4460, 4 worker thread(hardware_concurrency), siege -c 1200    
BuildOptimizeLevel: -O2    
![siege](https://raw.githubusercontent.com/taozhijiang/ailawd/c%2B%2B0x/ailawd_performance.jpg)
