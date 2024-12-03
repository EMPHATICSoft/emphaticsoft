/*  L++ plusORminus
 *  Created by Leo Bellantoni on 3 Apr 2023.
 *  Copyright 2023 FRA. All rights reserved.
 *
 *  Write a number plus its uncertainty, assuming the very
 *  common UTF-8 encoding, in engineering form
 *
 */
#pragma once



#include <string>



namespace plusORminus {
    enum engScale{femto,pico,nano,micro,milli,kilo,mega,giga,tera,peta, none};
    string formatted(double val, double err, engScale scale=none, string units="none") {
        // sprintf is better.
        char* temp = new char[256];
     
        switch (scale) {
        case femto :
            val *= 1.0e+15;         err *= 1.0e+15;
            if (units=="none") {
                sprintf(temp,"%7.2f %c%c %7.2f f%c%c", val, 0xC2,0xB1, err, 0xC2,0xBA);
            } else {
                sprintf(temp,"%7.2f %c%c %7.2f f%s",   val, 0xC2,0xB1, err, units.c_str());
            }
            break;
        case (pico) :
            val *= 1.0e+12;         err *= 1.0e+12;
            if (units=="none") {
                sprintf(temp,"%7.2f %c%c %7.2f p%c%c", val, 0xC2,0xB1, err, 0xC2,0xBA);
            } else {
                sprintf(temp,"%7.2f %c%c %7.2f p%s",   val, 0xC2,0xB1, err, units.c_str());
            }
            break;
        case (nano) :
            val *= 1.0e+9;          err *= 1.0e+9;
            if (units=="none") {
                sprintf(temp,"%7.2f %c%c %7.2f n%c%c", val, 0xC2,0xB1, err, 0xC2,0xBA);
            } else {
                sprintf(temp,"%7.2f %c%c %7.2f n%s",   val, 0xC2,0xB1, err, units.c_str());
            }
            break;
        case (micro) :
            val *= 1.0e+6;          err *= 1.0e+6;
            if (units=="none") {
                sprintf(temp,"%7.2f %c%c %7.2f %c%c%c%c", val, 0xC2,0xB1, err, 0xCE,0xBC, 0xC2,0xBA);
            } else {
                sprintf(temp,"%7.2f %c%c %7.2f %c%c%s",   val, 0xC2,0xB1, err, 0xCE,0xBC, units.c_str());
            }
            break;
        case (milli) :
            val *= 1.0e+3;          err *= 1.0e+3;
            if (units=="none") {
                sprintf(temp,"%7.2f %c%c %7.2f m%c%c", val, 0xC2,0xB1, err, 0xC2,0xBA);
            } else {
                sprintf(temp,"%7.2f %c%c %7.2f m%s",   val, 0xC2,0xB1, err, units.c_str());
            }
            break;
        case (kilo) :
            val /= 1.0e+3;          err /= 1.0e+3;
            if (units=="none") {
                sprintf(temp,"%7.2f %c%c %7.2f k%c%c", val, 0xC2,0xB1, err, 0xC2,0xBA);
            } else {
                sprintf(temp,"%7.2f %c%c %7.2f k%s",   val, 0xC2,0xB1, err, units.c_str());
            }
            break;
        case (mega) :
            val /= 1.0e+6;          err /= 1.0e+6;
            if (units=="none") {
                sprintf(temp,"%7.2f %c%c %7.2f M%c%c", val, 0xC2,0xB1, err, 0xC2,0xBA);
            } else {
                sprintf(temp,"%7.2f %c%c %7.2f M%s",   val, 0xC2,0xB1, err, units.c_str());
            }
            break;
        case (giga) :
            val /= 1.0e+9;          err /= 1.0e+9;
            if (units=="none") {
                sprintf(temp,"%7.2f %c%c %7.2f G%c%c", val, 0xC2,0xB1, err, 0xC2,0xBA);
            } else {
                sprintf(temp,"%7.2f %c%c %7.2f G%s",   val, 0xC2,0xB1, err, units.c_str());
            }
            break;
        case (tera) :
            val /= 1.0e+12;         err /= 1.0e+12;
            if (units=="none") {
                sprintf(temp,"%7.2f %c%c %7.2f T%c%c", val, 0xC2,0xB1, err, 0xC2,0xBA);
            } else {
                sprintf(temp,"%7.2f %c%c %7.2f T%s",   val, 0xC2,0xB1, err, units.c_str());
            }
            break;
        case (peta) :
            val /= 1.0e+15;         err /= 1.0e+15;
            if (units=="none") {
                sprintf(temp,"%7.2f %c%c %7.2f P%c%c", val, 0xC2,0xB1, err, 0xC2,0xBA);
            } else {
                sprintf(temp,"%7.2f %c%c %7.2f P%s",   val, 0xC2,0xB1, err, units.c_str());
            }
            break;
        default     :
            if (units=="none") {
                sprintf(temp,"%7.2f %c%c %7.2f %c%c ", val, 0xC2,0xB1, err, 0xC2,0xBA);
            } else {
                sprintf(temp,"%7.2f %c%c %7.2f %s ",   val, 0xC2,0xB1, err, units.c_str());
            }
        }
        string retval = temp;
        delete[] temp;
        return retval;
    }
}
