#
# Rscript to document the tracking & field precision, 
# test3,crude estimation of the uncertainty in the acceptance for a Yaw angle of 2 mRad between beam axis and spectrometer axis.    
#
dirTex <- "/home/lebrun/tex/EMPHATIC/MagFieldMarch2022/";
mD <- read.table("./EmphMagField_test3_p5_Acceptance_v1.txt", h=T);
mDAcc4mR <- subset(mD, (abs(mD$xFS2) < 40.));
mDAccPerfect <- subset(mD, (abs(mD$xF) < 40.));
deflXPerfect <- mDAccPerfect$slxF - mDAccPerfect$slxI;
hhdeflXPerfect <- hist(deflXPerfect, seq(from=-20., to= 20., by=0.01), plot='F');
plotFName <- sprintf("%s%s", dirTex, "XDeflection_5GeV_Pt100MeV_1.png");
png(filename=plotFName, width=420, height = 420);
plot(hhdeflXPerfect$mids, hhdeflXPerfect$counts, type ='h', xlim=c(10., 16.), ylim=c(0.1, 50000.), log='y', xlab="Deflection along X [mRad] ", ylab="counts/ 10 mRad bin");
text(x=10.1, y=2000., sprintf("%s%6.2f%s%5.2f%s", " Mean Deflection ", mean(deflXPerfect), " +-", sqrt(var(deflXPerfect)), " mRad"), cex=1.0, pos=4);
dev.off();
#
deflXWrong <- mDAccPerfect$slxFS2 - mDAccPerfect$slxI;
hhdeflXWrong <- hist(deflXWrong, seq(from=-20., to= 20., by=0.01), plot='F');
plotFName <- sprintf("%s%s", dirTex, "XDeflection_Wrong_5GeV_Pt100MeV_1.png");
png(filename=plotFName, width=420, height = 420);
plot(hhdeflXWrong$mids, hhdeflXWrong$counts, type ='h', xlim=c(10., 18.), ylim=c(0.1, 50000.), log='y', xlab="Deflection along X [mRad] ", ylab="counts/ 10 mRad bin");
text(x=10.1, y=10000., sprintf("%s%6.2f%s%5.2f%s", " Mean Deflection ", mean(deflXWrong), " +-", sqrt(var(deflXWrong)), " mRad"), cex=1.0, pos=4);
dev.off();
