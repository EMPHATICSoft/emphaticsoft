

This directory consists of multiple Root Macros useful for the alignment of the SSDs

main_alignment.C 
	reads in the ssd data and filters fines accpeted tracks.
	contains a number of functions for analyzing the effect of roations and plotting beam profile

alignmentx.C 
	contains functions for aligning the x-SSD stations using the method of residuals
	contains functions for plotting x-tracks and other analysis

xMinimization.C 
	can be used for aligning the x-SSD stations using a total chi squared

alignmenty.C 
	contains functions for aligning the y-SSD stations using the method of residuals
	contains functions for plotting y-tracks and other analysis


yMinimization.C 
	can be used for aligning the y-SSD stations using a total chi squared


minimization.C 
	can be used for finding the angle by which each ssd-station is rotated about its center


data directory
	a couple ready to use ssd data files
