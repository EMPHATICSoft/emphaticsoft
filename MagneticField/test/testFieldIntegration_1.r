#
# Rscript to document the tracking & field precision 
#
dirTex <- "/home/lebrun/tex/EMPHATIC/MagFieldMarch2022/";
mDV <- read.table("./EmphMagField_StlVector_v2.txt", h=T);
mDM <- read.table("./EmphMagField_StlMap_v2.txt", h=T);
mDxm2y6 <- subset(mDV, (abs(mDV$x + 2.06) < 0.01) & (abs(mDV$y - 6.42) < 0.01)); 
mDMxm2y6 <- subset(mDM, (abs(mDM$x + 2.06) < 0.01) & (abs(mDM$y - 6.42) < 0.01)); 
plotFName <- sprintf("%s%s", dirTex, "ZipAtXm2p06y6p42_0.png");
png(filename=plotFName, width=420, height = 420);
plot(mDMxm2y6$z-150., 0.1*mDMxm2y6$B0y, type ='b', pch=17, cex=0.75, xlim=c(-200., -50.), ylim=c(0., 0.1), xlab="Z [mm] Shifted by 150 mm ", ylab="By [Tesla]", col="magenta");
text(x=-200., y=0.05, " x= -2.06 mm, y = 6.42 mm", cex=1.2, pos=4);
dev.off();
plotFName <- sprintf("%s%s", dirTex, "ZipAtXm2p06y6p42_1.png");
png(filename=plotFName, width=780, height = 420);
plot(mDxm2y6$z, 0.1*mDxm2y6$B0y, type ='b', pch=15, cex=0.75, xlim=c(-200., 200.), xlab="Z [mm]", ylab="By [Tesla]");
lines(mDxm2y6$z, 0.1*mDxm2y6$B1y, type ='b', pch=16, col="blue");
lines(mDMxm2y6$z-155., mDMxm2y6$B1y, type ='b', pch=17, col="magenta");
legend(x="topright", c("Interpolation 3D Radial", "Interpolation Linear Cartesian", "Int. Linear, from stl Map, Zoff 5 mm"), pch=c(15, 16, 17), col=c("black", "blue", "magenta"));
text(x=40., y=1.25, " x= -2.06 mm, y = 6.42 mm", cex=1.2, pos=4);
dev.off();
plotFName <- sprintf("%s%s", dirTex, "ZipAtXm2p06y6p42_2.png");
png(filename=plotFName, width=780, height = 420);
plot(mDxm2y6$z, 0.1*mDxm2y6$B0y, type ='b', pch=15, cex=0.75, xlim=c(-90., 90.), ylim=c(1.0, 1.75),  xlab="Z [mm]", ylab="By [Tesla]");
lines(mDxm2y6$z,0.1* mDxm2y6$B1y, type ='b', pch=16, col="blue");
lines(mDMxm2y6$z-155., mDMxm2y6$B1y, type ='b', pch=17, col="magenta");
legend(x="topright", c("Interpolation 3D Radial", "Interpolation Linear Cartesian", "Int. Linear, from stl Map, Zoff 5 mm"), pch=c(15, 16, 17), col=c("black", "blue", "magenta"));
text(x=40., y=1.6, " x= -2.06 mm, y = 6.42 mm", cex=1.0);
dev.off();
#stop("For checks.. ");
#
plotFName <- sprintf("%s%s", dirTex, "ZipAtXm2p06y6p42_2.png");
png(filename=plotFName, width=780, height = 420);
plot(mDxm2y6$z, 0.1*mDxm2y6$divB0, type ='b', pch=15, cex=0.75, xlim=c(-200., 200.), xlab="Z [mm]", ylab="div B[Tesla/mm]");
lines(mDxm2y6$z, 0.1*mDxm2y6$divB1, type ='b', pch=16, col="blue");
lines(c(-10., 350.), c(0., 0.), type='l', lty=2, col="darkgrey", lwd=2);
legend(x="topright", c("Interpolation 3D Radial", "Interpolation Linear Cartesian"), pch=c(15, 16), col=c("black", "blue"));
text(x=0., y=0.03, " x= -2.06 mm, y = 6.42 mm", cex=1.25, pos=4);
dev.off();

mD <- read.table("./EmphMagField_StlVector_v2b.txt", h=T);
mDz130y6 <- subset(mD, (abs(mD$z - 18.312) < 0.01) & (abs(mD$y - 6.42) < 0.01)); 
plotFName <- sprintf("%s%s", dirTex, "ByZipAtZ130Y6_1.png");
png(filename=plotFName, width=420, height = 420);
plot(mDz130y6$x, 0.1*mDz130y6$B0y, type ='b', pch=15, cex=0.75, xlim=c(-35., 35.), ylim=c(1.2, 1.5), xlab="X [mm]", ylab="By [Tesla]");
lines(mDz130y6$x, 0.1*mDz130y6$B1y, type ='b', pch=16, col="blue");
legend(x="topright", c("By, Int. 3D Radial", "By, Linear Cartesian"), pch=c(15, 16), col=c("black", "blue"));
text(x=-40., y=14.2, " z= 18.312 mm, y = 6.42 mm", cex=1.5, pos=4);
dev.off();
plotFName <- sprintf("%s%s", dirTex, "BxZipAtZ130Y6_1.png");
png(filename=plotFName, width=420, height = 420);
plot(mDz130y6$x, 0.1*mDz130y6$B0x, type ='b', pch=15, cex=0.75, xlim=c(-60., 60.), ylim=c(-0.5, 0.5), xlab="X [mm]", ylab="Bx [Tesla]");
lines(mDz130y6$x, 0.1*mDz130y6$B1x, type ='b', pch=16, col="blue");
legend(x="topleft", c("Bx, Int. 3D Radial", "Bx, Linear Cartesian"), pch=c(15, 16), col=c("black", "blue"));
text(x=0., y=-0.35, " z= 18.312 mm, y = 6.42 mm", cex=1.25, pos=4);
dev.off();

mD <- read.table("./EmphMagField_p10_v3.txt", h=T);
mDS21 <- subset(mD, (mD$stepZ > 21.55));
mDS2 <- subset(mD, ((mD$stepZ < 2.2) & (mD$p < 4.)));
plotFName <- sprintf("%s%s", dirTex, "XDeviationvsY_10GeV_1.png");
png(filename=plotFName, width=420, height = 420);
plot(mDS2$y, mDS2$dSlx3DR_E, type='b', pch=15, ylim=c(0.04, 0.08), xlim=c(-50., 50), xlab="Y [mm]", ylab="Deflection along X [radian]");
lines(mDS2$y, mDS2$dslxLin_E, type='b', pch=16, col="blue");
legend(x="topright", c("By, Int. 3D Radial", "By, Linear Cartesian"), pch=c(15, 16), col=c("black", "blue"));
text(x=-20., y=0.010775, " Euler Step size = 2 mm ", cex=1.25, pos=4);
text(x=-20., y=0.010725, " p = 1 GeV  ", cex=1.25, pos=4);
dev.off();
#
mDYm2 <- subset(mD, (abs(mD$y + 2.) < 0.01) & (abs(mD$p - 10.) < 0.05));
plotFName <- sprintf("%s%s", dirTex, "XDeviationvsY_10GeV_StepSize_1.png");
png(filename=plotFName, width=420, height = 420);
plot(mDYm2$stepZ, 1000.*mDYm2$dSlx3DR_E, type='b', pch=15, ylim=c(7.2, 7.4), xlab="Step size [mm]", ylab="Deflection along X [mRadian]")
lines(mDYm2$stepZ, 1000.*mDYm2$dslxLin_E, type='b', pch=16, col="blue");
legend(x="topright", c("By, Int. 3D Radial", "By, Linear Cartesian"), pch=c(15, 16), col=c("black", "blue"));
text(x=7.5, y=0.010775, " Y = -2 mm ", cex=1.25, pos=4);
text(x=7.5, y=0.010725, " p = 10 GeV  ", cex=1.25, pos=4);
dev.off();

