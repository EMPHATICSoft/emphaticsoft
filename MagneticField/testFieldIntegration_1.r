#
# Rscript to document the tracking & field precision 
#
dirTex <- "/home/lebrun/tex/EMPHATIC/MagFieldMarch2022/";
mD <- read.table("./EmphMagField_v2.txt", h=T);
mDxm2y6 <- subset(mD, (abs(mD$x + 2.06) < 0.01) & (abs(mD$y - 6.42) < 0.01)); 
plotFName <- sprintf("%s%s", dirTex, "ZipAtXm2p06y6p42_1.png");
png(filename=plotFName, width=780, height = 420);
plot(mDxm2y6$z, mDxm2y6$B0y, type ='b', pch=15, cex=0.75, xlim=c(-10., 350.), xlab="Z [mm]", ylab="By [kGauss]");
lines(mDxm2y6$z, mDxm2y6$B1y, type ='b', pch=16, col="blue");
legend(x="topright", c("Int. 3D Radial", "Linear Cartesian"), pch=c(15, 16), col=c("black", "blue"));
text(x=120., y=5., " x= -2.06 mm, y = 6.42 mm", cex=1.5);
dev.off();
#
plotFName <- sprintf("%s%s", dirTex, "ZipAtXm2p06y6p42_2.png");
png(filename=plotFName, width=780, height = 420);
plot(mDxm2y6$z, mDxm2y6$divB0, type ='b', pch=15, cex=0.75, xlim=c(-10., 350.), xlab="Z [mm]", ylab="div B[kGauss/mm]");
lines(mDxm2y6$z, mDxm2y6$divB1, type ='b', pch=16, col="blue");
lines(c(-10., 350.), c(0., 0.), type='l', lty=2, col="darkgrey", lwd=2);
legend(x="topright", c("Int. 3D Radial", "Linear Cartesian"), pch=c(15, 16), col=c("black", "blue"));
text(x=150., y=0.05, " x= -2.06 mm, y = 6.42 mm", cex=1.5, pos=4);
dev.off();

mD <- read.table("./EmphMagField_v2b.txt", h=T);
mDz130y6 <- subset(mD, (abs(mD$z -132.578) < 0.01) & (abs(mD$y - 6.42) < 0.01)); 
plotFName <- sprintf("%s%s", dirTex, "ByZipAtZ130Y6_1.png");
png(filename=plotFName, width=420, height = 420);
plot(mDz130y6$x, mDz130y6$B0y, type ='b', pch=15, cex=0.75, xlim=c(-60., 60.), ylim=c(14., 15), xlab="X [mm]", ylab="By [kGauss]");
lines(mDz130y6$x, mDz130y6$B1y, type ='b', pch=16, col="blue");
legend(x="topright", c("By, Int. 3D Radial", "By, Linear Cartesian"), pch=c(15, 16), col=c("black", "blue"));
text(x=-40., y=14.2, " z= 132.578 mm, y = 6.42 mm", cex=1.5, pos=4);
dev.off();
plotFName <- sprintf("%s%s", dirTex, "BxZipAtZ130Y6_1.png");
png(filename=plotFName, width=420, height = 420);
plot(mDz130y6$x, mDz130y6$B0x, type ='b', pch=15, cex=0.75, xlim=c(-60., 60.), ylim=c(-0.18, 0.18), xlab="X [mm]", ylab="Bx [kGauss]");
lines(mDz130y6$x, mDz130y6$B1x, type ='b', pch=16, col="blue");
legend(x="topright", c("By, Int. 3D Radial", "By, Linear Cartesian"), pch=c(15, 16), col=c("black", "blue"));
text(x=-40., y=0.1, " z= 132.578 mm, y = 6.42 mm", cex=1.5, pos=4);
dev.off();

mD <- read.table("./EmphMagField_p10_v3.txt", h=T);
mDS21 <- subset(mD, (mD$stepZ > 21.55));
mDS2 <- subset(mD, (mD$stepZ < 2.2));
plotFName <- sprintf("%s%s", dirTex, "XDeviationvsY_10GeV_1.png");
png(filename=plotFName, width=420, height = 420);
plot(mDS2$y, mDS2$dSlx3DR_E, type='b', pch=15, ylim=c(0.0107, 0.011), xlim=c(-35., 35), xlab="Y [mm]", ylab="Deviation along X [radian]");
lines(mDS2$y, mDS2$dslxLin_E, type='b', pch=16, col="blue");
legend(x="topright", c("By, Int. 3D Radial", "By, Linear Cartesian"), pch=c(15, 16), col=c("black", "blue"));
text(x=-20., y=0.010775, " Euler Step size = 2 mm ", cex=1.25, pos=4);
text(x=-20., y=0.010725, " p = 10 GeV  ", cex=1.25, pos=4);
dev.off();
#
mDYm2 <- subset(mD, (abs(mD$y + 2.) < 0.01) & (abs(mD$p - 10.) < 0.05));
plotFName <- sprintf("%s%s", dirTex, "XDeviationvsY_10GeV_StepSize_1.png");
png(filename=plotFName, width=420, height = 420);
plot(mDYm2$stepZ, mDYm2$dSlx3DR_E, type='b', pch=15, ylim=c(0.0107, 0.0115), xlab="Step size [mm]", ylab="Deviation along X [radian]")
lines(mDYm2$stepZ, mDSYm2$dslxLin_E, type='b', pch=16, col="blue");
legend(x="topright", c("By, Int. 3D Radial", "By, Linear Cartesian"), pch=c(15, 16), col=c("black", "blue"));
text(x=7.5, y=0.010775, " Y = -2 mm ", cex=1.25, pos=4);
text(x=7.5, y=0.010725, " p = 10 GeV  ", cex=1.25, pos=4);

