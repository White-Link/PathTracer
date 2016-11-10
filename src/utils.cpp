/**
 * \file utils.cpp
 * \brief Implements useful functions.
 */

#include "utils.hpp"


void show_progress(double progress) {
    std::cout << std::fixed << std::setprecision(2) << "[";
    int position_progress = 70*progress;
    for (unsigned i=0; i<70; i++) {
    	if (i < position_progress) {
 			std::cout << "=";
 		}
        else if (i == position_progress) {
 			std::cout << ">";
 		}
        else {
 			std::cout << " ";
 		}
     }
    std::cout << "] " << progress*100 << "%\r";
    std::cout.flush();
}
