#
# Rscript for very preliminary studies of the ZipTrack scans done on May 22 2022
#
mD <- read.table("/home/lebrun/EMPHATIC/build/fcl/studyZipTrackData1_CentralScan.txt", h=T);
#
plot(mD$z, mD$ByP, type='b', pch=15, ylim=c(0., 1.8), xlab="Z [mm]", ylab="By [Tesla]", cex=0.75);
lines(mD$z, mD$ByC, type='b', pch=16, col="blue", cex=1.0);
legend(x="topright", c("Probe-C-H3W", "COMSOL"), pch=c(15,16), col=c("black", "blue"));
#
plot(mD$z, mD$ByP - mD$ByP[1], type='b', pch=15, ylim=c(0., 1.8), xlab="Z [mm]", ylab="By [Tesla]", cex=0.75);
lines(mD$z, mD$ByC, type='b', pch=16, col="blue", cex=1.0);
legend(x="topright", c("Probe-C-H3W", "COMSOL"), pch=c(15,16), col=c("black", "blue"));
#
plot(mD$z, ((mD$ByP - mD$ByP[1]) - mD$ByC) /(mD$ByC + 1.0e-10), type='b', pch=15, ylim=c(-0.4, 0.4), xlab="Z [mm]", ylab="Relative Diff, Probe-COMSOL", cex=0.75);
#
mD2 <- read.table("/home/lebrun/EMPHATIC/build/fcl/studyZipTrackData1_CentralScan_Ycentered_X358_ByUp.txt", h=T);
plot(mD2$z, mD2$ByP - mD2$ByP[1], type='b', pch=15, ylim=c(0., 1.8), xlab="Z [mm]", ylab="By [Tesla]", cex=0.75);
lines(mD2$z, mD2$ByC, type='b', pch=16, col="red", cex=1.0);
legend(x="topright", c("Probe-C-H3W", "COMSOL"), pch=c(15,16), col=c("black", "red"));

plot(mD$z, ((mD$ByP - mD$ByP[1]) - mD$ByC) /(mD$ByC + 1.0e-10), type='b', pch=15,
             ylim=c(-0.4, 0.4), xlab="Z [mm]", ylab="Relative Diff, Probe-COMSOL", col="blue", cex=0.75);
lines(mD2$z, ((mD2$ByP - mD2$ByP[1]) - mD2$ByC) /(mD2$ByC + 1.0e-10), type='b', pch=16, col="red");
legend(x="topright", c("Yc-348", "Yc-358"), pch=c(15,16), col=c("blue", "red"));

plot(mD$z, mD$ByC, type='b', pch=15, ylim=c(0., 1.8), xlab="Z [mm]", ylab="By [Tesla]", cex=0.75);
lines(mD$z, mD$ByP - mD$ByP[1] , type='b', pch=16, col="blue", cex=1.0);
lines(mD2$z, mD2$ByP - mD2$ByP[1], type='b', pch=17, col="red", cex=1.0);
legend(x="topright", c("COMSOL", "Probe-C-H3W, Yc-348", "Probe-C-H3W, Yc-358"), pch=c(15,16, 17), col=c("black", "blue", "red"));
