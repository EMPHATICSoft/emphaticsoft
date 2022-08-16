#
# Rscript for studying data integrity for run 511, 508, 536, 549..
#  Currently being worked on, do not run blindly, it probably will produce plenty of R errors. 
# Some code segments are valid, though..
#
library(ggplot2)
#
source("/home/lebrun/NEXT/relJan2017/Releases/NEXT_HEAD/RScripts/FuncForFittedClustTl_V3.r"); # just a bunch a simple functions
# set my screen such table are more readable. 
wideScreen();
#
effErrEff <- function(naI, nbI) {
  n1 <- as.double(naI);
  nb <- as.double(nbI);
  n2 <- nb - as.double(naI); # doing in two ways.. 
  ratio <- as.double(naI)/nb;
  ratio21 <- n2/n1;
  errratio21 <- ratio21 * sqrt(1.0/n1 + 1.0/n2);
  errRatio <- errratio21 / ((1.0 + ratio21) * (1.0 + ratio21))    ;
  print(sprintf(" Ratio %5.2f +- %5.2f r21 %8.3f errf21 %8.3f", 100.0*ratio, 100.0*errRatio, 100.0*ratio21, 100.0*errratio21 ));
}
mDW9_428_0 <- read.table("/home/lebrun/EMPHATIC/build/TriggerWaveForms/TrigADC0_Spill9_evt428.txt", h=T);
mDW9_428_1 <- read.table("/home/lebrun/EMPHATIC/build/TriggerWaveForms/TrigADC1_Spill9_evt428.txt", h=T);
plot(mDW9_428_0$k, mDW9_428_0$adc, type='b',pch=15, xlab=" 4 ns Bin #", cex=0.7, ylab="adc value", ylim=c(1750., 3750.));
lines(mDW9_428_1$k, mDW9_428_1$adc - 200, type='b',pch=16, cex=0.7, col="blue");
legend(x="bottomright", c("Run 511, Spill 9, evt 428, chan 0", "Run 511, Spill 9, evt 428, chan 1 - 200"), lty=c(1,1), pch=c(15,16), col=c("black", "blue"));
mD <- read.table("/home/lebrun/EMPHATIC/build/SummaryTrigT0RPC_V1_511_V1d_60GeV.txt", h=T); # 
mDTrig <- read.table("/home/lebrun/EMPHATIC/build/TrigTuple_V1_511_V1d_60GeV.txt", h=T); # 
mDW10_92_0 <- read.table("/home/lebrun/EMPHATIC/build/TriggerWaveForms/TrigADCMp0_Spill10_evt92.txt", h=T);
mDW10_92_1 <- read.table("/home/lebrun/EMPHATIC/build/TriggerWaveForms/TrigADCMp1_Spill10_evt92.txt", h=T);
plot(mDW10_92_0$k, mDW10_92_0$adc, type='b',pch=15, xlab=" 4 ns Bin #", cex=0.7, ylab="adc value", ylim=c(1750., 3750.));
lines(mDW10_92_1$k, mDW10_92_1$adc - 200, type='b',pch=16, cex=0.7, col="blue");
legend(x="bottomright", c("Run 511, Spill 10, evt 92, chan 0", "Run 511, Spill 10, evt 92, chan 1 - 200"), lty=c(1,1), pch=c(15,16), col=c("black", "blue"));
#
mDW10_158_0 <- read.table("/home/lebrun/EMPHATIC/build/TriggerWaveForms/TrigADCMp0_Spill10_evt158.txt", h=T);
mDW10_158_1 <- read.table("/home/lebrun/EMPHATIC/build/TriggerWaveForms/TrigADCMp1_Spill10_evt158.txt", h=T);
plot(mDW10_158_0$k, mDW10_158_0$adc, type='b',pch=15, xlab=" 4 ns Bin #", cex=0.7, ylab="adc value", ylim=c(-1000., 3750.));
lines(mDW10_158_1$k, mDW10_158_1$adc - 200, type='b',pch=16, cex=0.7, col="blue");
legend(x="bottomright", c("Run 511, Spill 10, evt 158, chan 0", "Run 511, Spill 10, evt 158, chan 1 - 200"), lty=c(1,1), pch=c(15,16), col=c("black", "blue"));
#
mDW7_1876_0 <- read.table("/home/lebrun/EMPHATIC/build/TriggerWaveForms/TrigADC0_Spill7_evt1876.txt", h=T);
mDW7_1876_1 <- read.table("/home/lebrun/EMPHATIC/build/TriggerWaveForms/TrigADC1_Spill7_evt1876.txt", h=T);
plot(mDW7_1876_0$k, mDW7_1876_0$adc, type='b',pch=15, xlab=" 4 ns Bin #", cex=0.7, ylab="adc value", ylim=c(-1000., 3750.));
lines(mDW7_1876_1$k, mDW7_1876_1$adc - 200, type='b',pch=16, cex=0.7, col="blue");
legend(x="bottomright", c("Run 511, Spill 7, evt 1876, chan 0", "Run 511, Spill 7, evt 1876, chan 1 - 200"), lty=c(1,1), pch=c(15,16), col=c("black", "blue"));
#
mD <- read.table("/home/lebrun/EMPHATIC/build/SummaryTrigT0RPC_V1_511_V1e_60GeV.txt", h=T); # 
mDTrig <- read.table("/home/lebrun/EMPHATIC/build/TrigTuple_V1_511_V1e_60GeV.txt", h=T); # 

#
mDOK <- subset(mD, ((mD$T0OK == 1) & (mD$RPCOK == 1)));
# re-wrote the C++.. now, all Trigger Ntuple do have a T0 and RPC tree. 
#mDTrig$OK <- rep(0, length(mDTrig$evt));
#for (k in 1:length(mDTrig$evt)) {
#  mDTmp <- subset(mDOK, ((mDOK$evt == mDTrig$evt[k]) & (mDOK$spill == mDTrig$subRun[k])));
#  if (length(mDTmp$evt == 1)) mDTrig$OK[k] = 1;
#}
#
mDTrigHigh0 <- subset(mDTrig, (mDTrig$Sadc0 > 2000.))
mDTrigHigh0123 <- subset(mDTrig, ((mDTrig$Sadc0 > 2000.) & (mDTrig$Sadc1 > 1500.) 
                               & (mDTrig$Sadc2 > 1500.) & (mDTrig$Sadc3 > 1500.) & (mDTrig$Width0 > 0.) & (mDTrig$Width1 > 0.)));
hist(sqrt(mDTrigHigh0123$Mean0), 100);
hhTrigHigh0123Width0 <- hist(sqrt(mDTrigHigh0123$Width0), 1000, plot=F)
hhTrigHigh0123Width1 <- hist(sqrt(mDTrigHigh0123$Width1), 1000, plot=F)
plot(hhTrigHigh0123Width0$mids, hhTrigHigh0123Width0$counts, type='h', log='y', ylim=c(0.5, 4000.), xlim=c(0., 20.))
plot(hhTrigHigh0123Width1$mids, hhTrigHigh0123Width1$counts, type='h', log='y', ylim=c(0.5, 4000.), xlim=c(0., 20.))
#
mDTrigHigh0123Sp0 <- subset(mDTrigHigh0123, ((sqrt(mDTrigHigh0123$Width0) < 12.5) & (sqrt(mDTrigHigh0123$Width1) < 12.5)));
mDTrigHigh0123Mp0 <- subset(mDTrigHigh0123, ((sqrt(mDTrigHigh0123$Width0) >= 15.) & (sqrt(mDTrigHigh0123$Width1) >= 15.)));
#
# Repeat at 4 GeV 
#
ieCut <- 60;
if (ieCut == 4) { 
  mD <- read.table("/home/lebrun/EMPHATIC/build/SummaryTrigT0RPC_V1_549_V1d_4GeV.txt", h=T); # 
#
  mDOK <- subset(mD, ((mD$T0OK == 1) & (mD$RPCOK == 1)));
  mDTrig <- read.table("/home/lebrun/EMPHATIC/build/TrigTuple_V1_549_V1e_4GeV.txt", h=T); # 
}
#
# Repeat at 8 GeV 
#
if (ieCut == 8) { 
  mD <- read.table("/home/lebrun/EMPHATIC/build/SummaryTrigT0RPC_V1_536_V1d_8GeV.txt", h=T); # 
#
  mDOK <- subset(mD, ((mD$T0OK == 1) & (mD$RPCOK == 1)));
  mDTrig <- read.table("/home/lebrun/EMPHATIC/build/TrigTuple_V1_536_V1e_8GeV.txt", h=T); # 
}
#
# Repeat at 120 GeV 
#
if (ieCut == 120) { 
  mD <- read.table("/home/lebrun/EMPHATIC/build/SummaryTrigT0RPC_V1_508_V1e_120GeV.txt", h=T); # 
#
  mDOK <- subset(mD, ((mD$T0OK == 1) & (mD$RPCOK == 1)));
  mDTrig <- read.table("/home/lebrun/EMPHATIC/build/TrigTuple_V1_508_V1e_120GeV.txt", h=T); # 
}
mDOKTrig <- subset(mDOK, ((mDOK$trigSAdc0 > -10000) & (mDOK$trigSAdc1 > -10000) & (mDOK$trigSAdc2 > -10000) & (mDOK$trigSAdc3 > -10000)));
hhTrigAc0 <- hist(mDOKTrig$trigSAdc0, seq(from=-50000., to=150000., by=50), plot=F);
hhTrigAc1 <- hist(mDOKTrig$trigSAdc1, seq(from=-50000., to=150000., by=50), plot=F);
hhTrigAc2 <- hist(mDOKTrig$trigSAdc2, seq(from=-50000., to=150000., by=50), plot=F);
hhTrigAc3 <- hist(mDOKTrig$trigSAdc3, seq(from=-50000., to=150000., by=50), plot=F);
plot(hhTrigAc0$mids, hhTrigAc0$counts, type='b', pch=15, col="black", cex=0.5, 
          xlim=c(-10., 15000.), ylim=c(0., 15000.), xlab="Trigger ADC pulse height", ylab=c("counts per 20 ADC counts bins"));
lines(hhTrigAc1$mids, hhTrigAc1$counts + 3000., type='b', pch=16, cex=0.5, col="blue");
lines(hhTrigAc2$mids, hhTrigAc2$counts + 6000., type='b',pch=17, cex=0.5, col="magenta");
lines(hhTrigAc3$mids, hhTrigAc3$counts + 9000., type='b',pch=18, cex=0.5, col="red");
legend(x="topright", c("Trig ADC 0", "Trig ADC 1", "Trig ADC 2", "Trig ADC 3"), lty=c(1,1,1,1), pch=c(15,16,17, 18), col=c("black", "blue", "magenta", "red"));
#
ggplot(mDOKTrig, aes(x=trigSAdc0, y=trigSAdc1)) + geom_point(aes(color=trigSAdc2), size=0.5) + scale_fill_distiller() + xlim(0., 16000.) + ylim(0., 16000.)
ggplot(mDOKTrig, aes(x=trigSAdc0, y=trigSAdc2)) + geom_point(aes(color=trigSAdc3), size=0.5) + scale_fill_distiller()
mDLowTrig <- subset(mDTrig, ((mDTrig$Sadc0 < 2000) &  (mDTrig$Sadc1 < 2000)));
mDHighTrig <- subset(mDTrig, ((mDTrig$Sadc0 >= 2000) &  (mDTrig$Sadc1 >= 2000)));
ggplot(mDHighTrig, aes(x=Padc0, y=Padc1)) + geom_point(aes(color=Padc2), size=0.5) + scale_fill_distiller() + xlim(0., 4000.) + ylim(0., 4000.)
mDHighTrigNoSat <- subset(mDHighTrig, ((mDHighTrig$Padc0 < 3750.) &  (mDHighTrig$Padc1 < 3820)));
mDHighTrigNoSatOKSkew <- subset(mDHighTrigNoSat, (abs(mDHighTrigNoSat$Skew0) < 20000.));
#
# detailed study of the trigger multi-peak reconstruction.. 
#
mDTrigA <- subset(mDTrig, ((mDTrig$Sadc0 > 0. ) & (mDTrig$Padc0 <  3800. ) & (mDTrig$Sadc1 > 0. ) & (mDTrig$Padc1 <  3800. ) & 
                          (mDTrig$Sadc2 > 0. ) & (mDTrig$Padc2 <  3800. ) & (mDTrig$Sadc3 > 0. ) & (mDTrig$Padc3 <  3800. ))); 
mDTrigAPerfect <- subset(mDTrigA, ((mDTrigA$numPeak0 == 1) & (mDTrigA$numPeak1 == 1) & (mDTrigA$numPeak2 == 1) & (mDTrigA$numPeak3 == 1) ));
mDTrigA3Sp <- subset(mDTrigA, ((mDTrigA$numPeak0 == 1) & (mDTrigA$numPeak1 == 1) & (mDTrigA$numPeak2 == 1) |
                             (mDTrigA$numPeak1 == 1) & (mDTrigA$numPeak2 == 1) & (mDTrigA$numPeak3 == 1) |
			     (mDTrigA$numPeak2 == 1) & (mDTrigA$numPeak3 == 1) & (mDTrigA$numPeak0 == 1) |
			     (mDTrigA$numPeak3 == 1) & (mDTrigA$numPeak0 == 1) & (mDTrigA$numPeak1 == 1)));
mDTrigA3Mp <- subset(mDTrigA, ((mDTrigA$numPeak0 > 1) & (mDTrigA$numPeak1 > 1) & (mDTrigA$numPeak2 > 1) |
                             (mDTrigA$numPeak1 > 1) & (mDTrigA$numPeak2 > 1) & (mDTrigA$numPeak3 > 1) |
			     (mDTrigA$numPeak2 > 1) & (mDTrigA$numPeak3 > 1) & (mDTrigA$numPeak0 > 1) |
			     (mDTrigA$numPeak3 > 1) & (mDTrigA$numPeak0 > 1) & (mDTrigA$numPeak1 > 1)));
mDTrigA4Mp <- subset(mDTrigA, ((mDTrigA$numPeak0 > 1) & (mDTrigA$numPeak1 > 1) & (mDTrigA$numPeak2 > 1) & (mDTrigA$numPeak3 > 1)));
			     
hhTrigA3Sp_0 <- hist(mDTrigA3Sp$Sadc0, seq(from=-50000., to=150000., by=10), plot=F);
plot(hhTrigA3Sp_0$mids, hhTrigA3Sp_0$counts, type='h', log='y', xlim=c(0., 50000.), ylim=c(0.5, 2000.), 
     xlab="Integral of Waveform", ylab="Counts per 10 adc Cnt" )
hhTrigA3Mp_0 <- hist(mDTrigA3Mp$Sadc0, seq(from=-50000., to=150000., by=10), plot=F);
plot(hhTrigA3Mp_0$mids, hhTrigA3Mp_0$counts, type='h', log='y', xlim=c(0., 50000.), ylim=c(0.5, 2000.), 
     xlab="Integral of Waveform", ylab="Counts per 10 adc Cnt" );
mDTrigA3Mp_dt0 <- subset( mDTrigA3Mp, ( (mDTrigA3Mp$numPeak0 > 1) & ( mDTrigA3Mp$deltaT0 > -1.) ));  
mDTrigA3Mp_dt01 <- subset( mDTrigA3Mp, ( (mDTrigA3Mp$numPeak1 > 1) & ( mDTrigA3Mp$deltaT1 > -1.) & (mDTrigA3Mp$numPeak1 > 1) & ( mDTrigA3Mp$deltaT1 > -1.)));
dt01CoordX <- seq(from=2., to = 404, by=4.);
dt01CoordY <- seq(from=2., to = 404, by=4.);
dt01MatrixData <- rep(0., length(dt01CoordX) * length(dt01CoordY));
for (ik in 1:length(dt01CoordX)) { 
 for (jk in 1:length(dt01CoordX)) {
   mDTmp <- subset(mDTrigA3Mp_dt01, ((abs(mDTrigA3Mp_dt01$deltaT0 - dt01CoordX[ik]) < 3.) &  (abs(mDTrigA3Mp_dt01$deltaT1 - dt01CoordY[jk]) < 3.)));
   dt01MatrixData[jk*length(dt01CoordX) + ik] <- min(5, length(mDTmp$evt));
  } 
 }
dt0q1Matrix <- matrix(dt01MatrixData, nrow=length(dt01CoordX), ncol=length(dt01CoordY));
dt01MatrixTab <- as.table(dt01Matrix);
image(dt01Matrix, col = gray.colors(5, start= 0, end=1.0, rev=T), axes=F);
axis(side=1, at=seq(from=0., to=1.0, by=0.25), labels=c(0., 100., 200., 300., 400.))
axis(side=2, at=seq(from=0., to=1.0, by=0.25), labels=c(0., 100., 200., 300., 400.))
#
# Tallies.. 
#
numTTrig <- length(mDTrig$evt);
numATrig <- length(mDTrigA$evt);
effErrEff(numATrig, numTTrig);
numA4Sp <- length(mDTrigAPerfect$evt);
effErrEff(numA4Sp, numTTrig);
numA3Sp <- length(mDTrigA3Sp$evt);
effErrEff(numA3Sp, numTTrig);
numA3Mp <- length(mDTrigA3Mp$evt);
effErrEff(numA3Mp, numTTrig);
numHalo <- length(mDLowTrig$evt);
effErrEff(numHalo, numTTrig);
  
mDTrigA3MpBad_dt0 <- subset( mDTrigA3Mp, ( (mDTrigA3Mp$numPeak0 > 1) & (abs(mDTrigA3Mp$deltaT0) < 1.)));  
hhTrigA3Mp_dt <- hist(mDTrigA3Mp_dt0$deltaT0, seq(from=-2., to=400., by=4), xlim=c(0., 400.), main="Histogram of deltaT", xlab="nanosecond", ylab="Counts/4 ns");

hhTrigAPerfect_S <- hist(0.25*(mDTrigAPerfect$Sadc0 + mDTrigAPerfect$Sadc1 +  mDTrigAPerfect$Sadc2 + mDTrigAPerfect$Sadc3), 
                                seq(from=-50000., to=150000., by=10), plot=F);
plot(hhTrigAPerfect_S$mids, hhTrigAPerfect_S$counts, type='h', log='y', xlim=c(0., 50000.), ylim=c(0.5, 2000.), 
     xlab="Integral of Sum Waveform", ylab="Counts per 10 adc Cnt" );

hhTrigA4Mp_S <- hist(0.25*(mDTrigA4Mp$Sadc0 + mDTrigA4Mp$Sadc1 +  mDTrigA4Mp$Sadc2 + mDTrigA4Mp$Sadc3), 
                                seq(from=-50000., to=150000., by=10), plot=F);
plot(hhTrigA4Mp_S$mids, 10.0*hhTrigA4Mp_S$counts, type='h', log='y', xlim=c(0., 50000.), ylim=c(0.5, 2000.), 
     xlab="Integral of Sum Waveform", ylab="Counts per 10 adc Cnt" );

#
mDTrigA3Mp0 <- subset(mDTrigA3Mp, (mDTrigA3Mp$deltaT0 > 0.));
mDTrigA3Mp01 <- subset(mDTrigA3Mp, ((mDTrigA3Mp$deltaT0 > 0.) & (mDTrigA3Mp$deltaT1 > 0.))); 
hist(mDTrigA3Mp0$deltaT0, 200)
ggplot(mDTrigA3Mp01, aes(x=deltaT0, y=deltaT1)) + geom_point(aes(color=Sadc0), size=0.5) + scale_fill_distiller() + xlim(0., 400.) + ylim(0., 400.)

#
# T0 analysis.. 
#
mDWT0508_10_4_4 <- read.table("/home/lebrun/EMPHATIC/build/T0WaveForms/T0ADCBipolar_4_Run_508_Spill10_evt_4.txt", h=T);
mDWT0508_10_4_5 <- read.table("/home/lebrun/EMPHATIC/build/T0WaveForms/T0ADCBipolar_5_Run_508_Spill10_evt_4.txt", h=T);
plot(mDWT0508_10_4_4$k, mDWT0508_10_4_4$adc, type='b',pch=15, xlab=" 4 ns Bin #", cex=0.7, ylab="adc value", ylim=c(3400., 3950.));
lines(mDWT0508_10_4_5$k, mDWT0508_10_4_5$adc - 200, type='b',pch=16, cex=0.7, col="blue");
legend(x="bottomright", c("Run 508, Spill 10, evt 4, chan 4", "Run 508, Spill 10, evt 4, chan 4 - 200"), lty=c(1,1), pch=c(15,16), col=c("black", "blue"));
#
# 


mDOKLowTrig <- subset(mDOK, ((mDOK$trigSAdc0 < 2000) &  (mDOK$trigSAdc1 < 2000) & (mDOK$trigSAdc0 > 0.) & (mDOK$trigSAdc1 > 0.)));
mDOKHighTrig <- subset(mDOK, ((mDOK$trigSAdc0 >= 2000) &  (mDOK$trigSAdc1 >= 2000) & (mDOK$trigSAdc0 > 0.) & (mDOK$trigSAdc1 > 0.)));

mDOKHighTrigT0Adc5OK <- subset(mDOKHighTrig, ((mDOKHighTrig$T0adc5 >= 9999)));
mDOKLowTrigT0Adc5OK <- subset(mDOKLowTrig, ((mDOKLowTrig$T0adc5 >= 9999)));
mDOKHighTrigT0Adc5OKSpill96 <- subset(mDOKHighTrigT0Adc5OK, ((mDOKHighTrigT0Adc5OK$spill == 96)));

mDOKHighTrigT0Adc5T0adc1OK <-  subset(mDOKHighTrigT0Adc5OK,(mDOKHighTrigT0Adc5OK$T0adc1 >= 9999));
mDOKHighTrigT0Adc5T0adc2OK <-  subset(mDOKHighTrigT0Adc5OK,(mDOKHighTrigT0Adc5OK$T0adc2 >= 9999));
mDOKHighTrigT0Adc5T0adc3OK <-  subset(mDOKHighTrigT0Adc5OK,(mDOKHighTrigT0Adc5OK$T0adc3 >= 9999));
mDOKHighTrigT0Adc5T0adc4OK <-  subset(mDOKHighTrigT0Adc5OK,(mDOKHighTrigT0Adc5OK$T0adc4 >= 9999));
mDOKHighTrigT0Adc5T0adc5OK <-  subset(mDOKHighTrigT0Adc5OK,(mDOKHighTrigT0Adc5OK$T0adc5 >= 9999));
mDOKHighTrigT0Adc5T0adc6OK <-  subset(mDOKHighTrigT0Adc5OK,(mDOKHighTrigT0Adc5OK$T0adc6 >= 9999));
mDOKHighTrigT0Adc5T0adc7OK <-  subset(mDOKHighTrigT0Adc5OK,(mDOKHighTrigT0Adc5OK$T0adc7 >= 9999));
mDOKHighTrigT0Adc5T0adc8OK <-  subset(mDOKHighTrigT0Adc5OK,(mDOKHighTrigT0Adc5OK$T0adc8 >= 9999));
mDOKHighTrigT0Adc5T0adc9OK <-  subset(mDOKHighTrigT0Adc5OK,(mDOKHighTrigT0Adc5OK$T0adc9 >= 9999));
mDOKHighTrigT0Adc5T0adc10OK <- subset(mDOKHighTrigT0Adc5OK,(mDOKHighTrigT0Adc5OK$T0adc10 >= 9999));
mDOKHighTrigT0Adc5T0adc11OK <- subset(mDOKHighTrigT0Adc5OK,(mDOKHighTrigT0Adc5OK$T0adc11 >= 9999));
mDOKHighTrigT0Adc5T0adc12OK <- subset(mDOKHighTrigT0Adc5OK,(mDOKHighTrigT0Adc5OK$T0adc12 >= 9999));
mDOKHighTrigT0Adc5T0adc13OK <- subset(mDOKHighTrigT0Adc5OK,(mDOKHighTrigT0Adc5OK$T0adc13 >= 9999));
mDOKHighTrigT0Adc5T0adc14OK <- subset(mDOKHighTrigT0Adc5OK,(mDOKHighTrigT0Adc5OK$T0adc14 >= 9999));
mDOKHighTrigT0Adc5T0adc15OK <- subset(mDOKHighTrigT0Adc5OK,(mDOKHighTrigT0Adc5OK$T0adc15 >= 9999));
mDOKHighTrigT0Adc5T0adc16OK <- subset(mDOKHighTrigT0Adc5OK,(mDOKHighTrigT0Adc5OK$T0adc16 >= 9999));
mDOKHighTrigT0Adc5T0adc17OK <- subset(mDOKHighTrigT0Adc5OK,(mDOKHighTrigT0Adc5OK$T0adc17 >= 9999));
mDOKHighTrigT0Adc5T0adc18OK <- subset(mDOKHighTrigT0Adc5OK,(mDOKHighTrigT0Adc5OK$T0adc18 >= 9999));
#
mDOKLowTrigT0Adc5T0adc1OK <-  subset(mDOKLowTrigT0Adc5OK,(mDOKLowTrigT0Adc5OK$T0adc1 >= 9999));
mDOKLowTrigT0Adc5T0adc2OK <-  subset(mDOKLowTrigT0Adc5OK,(mDOKLowTrigT0Adc5OK$T0adc2 >= 9999));
mDOKLowTrigT0Adc5T0adc3OK <-  subset(mDOKLowTrigT0Adc5OK,(mDOKLowTrigT0Adc5OK$T0adc3 >= 9999));
mDOKLowTrigT0Adc5T0adc4OK <-  subset(mDOKLowTrigT0Adc5OK,(mDOKLowTrigT0Adc5OK$T0adc4 >= 9999));
mDOKLowTrigT0Adc5T0adc5OK <-  subset(mDOKLowTrigT0Adc5OK,(mDOKLowTrigT0Adc5OK$T0adc5 >= 9999));
mDOKLowTrigT0Adc5T0adc6OK <-  subset(mDOKLowTrigT0Adc5OK,(mDOKLowTrigT0Adc5OK$T0adc6 >= 9999));
mDOKLowTrigT0Adc5T0adc7OK <-  subset(mDOKLowTrigT0Adc5OK,(mDOKLowTrigT0Adc5OK$T0adc7 >= 9999));
mDOKLowTrigT0Adc5T0adc8OK <-  subset(mDOKLowTrigT0Adc5OK,(mDOKLowTrigT0Adc5OK$T0adc8 >= 9999));
mDOKLowTrigT0Adc5T0adc9OK <-  subset(mDOKLowTrigT0Adc5OK,(mDOKLowTrigT0Adc5OK$T0adc9 >= 9999));
mDOKLowTrigT0Adc5T0adc10OK <- subset(mDOKLowTrigT0Adc5OK,(mDOKLowTrigT0Adc5OK$T0adc10 >= 9999));
mDOKLowTrigT0Adc5T0adc11OK <- subset(mDOKLowTrigT0Adc5OK,(mDOKLowTrigT0Adc5OK$T0adc11 >= 9999));
mDOKLowTrigT0Adc5T0adc12OK <- subset(mDOKLowTrigT0Adc5OK,(mDOKLowTrigT0Adc5OK$T0adc12 >= 9999));
mDOKLowTrigT0Adc5T0adc13OK <- subset(mDOKLowTrigT0Adc5OK,(mDOKLowTrigT0Adc5OK$T0adc13 >= 9999));
mDOKLowTrigT0Adc5T0adc14OK <- subset(mDOKLowTrigT0Adc5OK,(mDOKLowTrigT0Adc5OK$T0adc14 >= 9999));
mDOKLowTrigT0Adc5T0adc15OK <- subset(mDOKLowTrigT0Adc5OK,(mDOKLowTrigT0Adc5OK$T0adc15 >= 9999));
mDOKLowTrigT0Adc5T0adc16OK <- subset(mDOKLowTrigT0Adc5OK,(mDOKLowTrigT0Adc5OK$T0adc16 >= 9999));
mDOKLowTrigT0Adc5T0adc17OK <- subset(mDOKLowTrigT0Adc5OK,(mDOKLowTrigT0Adc5OK$T0adc17 >= 9999));
mDOKLowTrigT0Adc5T0adc18OK <- subset(mDOKLowTrigT0Adc5OK,(mDOKLowTrigT0Adc5OK$T0adc18 >= 9999));
T0Indices <- seq(from=1, to=18, by=1);
T0adcYield <- rep(0., time=length(T0Indices));
T0adcYield[1] <- length(mDOKHighTrigT0Adc5T0adc1OK$evt);
T0adcYield[2] <- length(mDOKHighTrigT0Adc5T0adc2OK$evt);
T0adcYield[3] <- length(mDOKHighTrigT0Adc5T0adc3OK$evt);
T0adcYield[4] <- length(mDOKHighTrigT0Adc5T0adc4OK$evt);
T0adcYield[5] <- length(mDOKHighTrigT0Adc5T0adc5OK$evt);
T0adcYield[6] <- length(mDOKHighTrigT0Adc5T0adc6OK$evt);
T0adcYield[7] <- length(mDOKHighTrigT0Adc5T0adc7OK$evt);
T0adcYield[8] <- length(mDOKHighTrigT0Adc5T0adc8OK$evt);
T0adcYield[9] <- length(mDOKHighTrigT0Adc5T0adc9OK$evt);
T0adcYield[10] <- length(mDOKHighTrigT0Adc5T0adc10OK$evt);
T0adcYield[11] <- length(mDOKHighTrigT0Adc5T0adc11OK$evt);
T0adcYield[12] <- length(mDOKHighTrigT0Adc5T0adc12OK$evt);
T0adcYield[13] <- length(mDOKHighTrigT0Adc5T0adc13OK$evt);
T0adcYield[14] <- length(mDOKHighTrigT0Adc5T0adc14OK$evt);
T0adcYield[15] <- length(mDOKHighTrigT0Adc5T0adc15OK$evt);
T0adcYield[16] <- length(mDOKHighTrigT0Adc5T0adc16OK$evt);
T0adcYield[17] <- length(mDOKHighTrigT0Adc5T0adc17OK$evt);
T0adcYield[18] <- length(mDOKHighTrigT0Adc5T0adc18OK$evt);


#
# Not sure what the story here is.. Skip 
#mDOKHighTrigT0Adc5UpDownOK <- subset(mDOKHighTrigT0Adc5OK, (((mDOKHighTrigT0Adc5OK$T0tdc5 < 1.0e5)) & (mDOKHighTrigT0Adc5OK$T0adc13 >= 9999) 
#                                                            & (mDOKHighTrigT0Adc5OK$T0tdc13 < 1.0e5)));
#mDOKHighTrigT0Adc5UpDownOKT <- subset(mDOKHighTrigT0Adc5UpDownOK, ((abs(mDOKHighTrigT0Adc5UpDownOK$T0tdc5 - 4.9) < 1) & 
#                                                                   (abs(mDOKHighTrigT0Adc5UpDownOK$T0tdc13 - 4.9) < 1)));
#mDOKHighTrigT0Adc5T0tdc5OK <- subset(mDOKHighTrigT0Adc5OK, ((mDOKHighTrigT0Adc5OK$T0tdc5 < 1.0e5)));
#

#
hhTADC5LowTrigadc0 <- hist(mDOKLowTrigT0Adc5OK$T0adc5 - 10000, seq(from=0., to=2500., by=50.), plot=F)
hhTADC5HighTrigadc0 <- hist(mDOKHighTrigT0Adc5OK$T0adc5 - 10000, seq(from=0., to=2500., by=50.), plot=F)
plot(hhTADC5HighTrigadc0$mids, 1.0*hhTADC5HighTrigadc0$counts, type='b', pch=15, col="magenta", xlab="T0ADC5", 
           ylab="Counts per 50 ADC counts bin", log='y', ylim=c(0.7, 1.0e5));
lines(hhTADC5LowTrigadc0$mids, 10.0*hhTADC5LowTrigadc0$counts, type='b', pch=16, col="blue");
arrows(hhTADC5HighTrigadc0$mids, (hhTADC5HighTrigadc0$counts - sqrt(hhTADC5HighTrigadc0$counts)), 
         hhTADC5HighTrigadc0$mids, (hhTADC5HighTrigadc0$counts + sqrt(hhTADC5HighTrigadc0$counts)), code=3, length=0.05, angle=90., col="magenta");
legend (x="topright", c("Trigger counter high ", "Trigger counter low, X10 "), pch=c(15,16), lty=c(1,1), col=c("magenta", "blue"))
#
#TDC 
#

mDOKHighTrigT0Adc5OKTDCOK <- subset(mDOKHighTrigT0Adc5OK, (mDOKHighTrigT0Adc5OK$T0tdc5 < 10000.))
mDOKHighTrigT0Adc5OKTDCNOK <- subset(mDOKHighTrigT0Adc5OK, (mDOKHighTrigT0Adc5OK$T0tdc5 > 10000.))
hh
	 
#hhTADC4LowTrigadc0 <- hist(mDOKLowTrig0$T0adc4, seq(from=0., to=2500., by=50.), plot=F)
#hhTADC4HighTrigadc0 <- hist(mDOKHighTrig0$T0adc4, seq(from=0., to=2500., by=50.), plot=F)
#plot(hhTADC4HighTrigadc0$mids, 1.0*hhTADC4HighTrigadc0$counts, type='b', pch=15, col="magenta", xlab="T0ADC4", 
#           ylab="Counts per 50 ADC counts bin", log='y', ylim=c(0.7, 2.0e5));
#lines(hhTADC4LowTrigadc0$mids, 10.0*hhTADC4LowTrigadc0$counts, type='b', pch=16, col="blue");
#arrows(hhTADC4LowTrigadc0$mids, 10.0*(hhTADC4LowTrigadc0$counts - sqrt(hhTADC4LowTrigadc0$counts)), 
#         hhTADC4LowTrigadc0$mids, 10.0*(hhTADC4LowTrigadc0$counts + sqrt(hhTADC4LowTrigadc0$counts)), code=3, length=0.05, angle=90., col="blue");
#legend(x="topright", c("Trigger ADC 4 > 1000", "Trigger ADC 4 < 1000, X 10"), pch=c(15,16), lty=c(1,1), col=c("magenta", "blue"));	 
#plot(mD2$evt, type='p', pch='.', col="black", xlab="Sequential # into the file", ylab="Event #", ylim=c(0., 5000.) xlim=c(0., 5000.)  );
#lines(mD10$evt, type='p', pch='*', cex=0.25, col="blue");
#lines(mD11$evt, type='p', pch='*', cex=0.35, col="red");
#
# Back to the DAQ efficiency problems.. 
#

hhMDSp <- hist(mD$spill, seq(from=-1., to=105., by=1.), plot=F);
hhMDSpOK <- hist(mDOK$spill, seq(from=-1., to=105., by=1.), plot=F);
plot(hhMDSp$mids+1.25, hhMDSp$counts, type='h', lwd=1, lty=1, xlab="Spill number ", ylab="Count per spill", col="red", ylim=c(0., 6000.));
lines(hhMDSpOK$mids+0.75, hhMDSpOK$counts, type='h',lwd=2, lty=2, col="blue");
legend(x="topright", c("All subruns", "subRuns with T0 & RPC"), lty=c(1,2), lwd=c(1,2), col=c("red", "blue"))
#
hhTrigAdc0OK <- hist(mDOK$trigAdc0, seq(from=0., to= 4000., by=60.), plot=F);
hhTrigAdc0nOK <- hist(mDnOK$trigAdc0, seq(from=0., to= 4000., by=60.), plot=F);
#
plot(hhTrigAdc0nOK$mids+20., hhTrigAdc0nOK$counts, type='h', lwd=1, lty=1, xlab="Adc 0 ", ylab="Count per 60.", col="red", ylim=c(0., 20000.));
lines(hhTrigAdc0OK$mids+40, hhTrigAdc0OK$counts, type='h',lwd=2, lty=2, col="blue");
legend(x="topright", c("Subruns no T0, no RPC", "SubRuns with T0 & RPC"), lty=c(1,2), lwd=c(1,2), col=c("red", "blue"))
#
ggplot(mDOK, aes(x=trigAdc0, y=trigAdc1)) + geom_point(aes(color=trigAdc2), size=0.5) + scale_fill_distiller()
#
mDOKTDCAdc0_Ok <- subset(mDOK, (mDOK$T0adc0 > 0.01));
mDOKTDCAdc0_nOk <- subset(mDOK, (mDOK$T0adc0 < 0.01));
mDOKTDCAdc1_Ok <- subset(mDOK, (mDOK$T0adc1 > 0.01));
mDOKTDCAdc1_nOk <- subset(mDOK, (mDOK$T0adc1 < 0.01));
mDOKTDCAdc1_Tdc1_nOk <- subset(mDOK, ((mDOK$T0adc1 < 0.01) & (mDOK$T0tdc1 > 1.5e308)));
mDOKTDCAdc1_Tdc1_Ok <- subset(mDOK, ((mDOK$T0adc1 > 0.01) & (mDOK$T0tdc1 < 1000.) ));
mDOKTDCAdc1a2_Tdc1a2_Ok <- subset(mDOK, ((mDOK$T0adc1 > 0.01) & (mDOK$T0tdc1 < 1000.) & (mDOK$T0adc2 > 0.01) & (mDOK$T0tdc2 < 1000.) ));

ggplot(mDOKTDCAdc1a2_Tdc1a2_Ok, aes(x=T0adc1, y=T0adc2)) + geom_point(aes(color=T0tdc1), size=0.5) + scale_fill_distiller() +  xlim(0.,150.) + ylim(0., 150.);
#
mDOKTDCAdc1a2_Tdc1a2_OkP <- subset(mDOK, ((mDOK$T0adc1 > 25.0) & (mDOK$T0tdc1 < 1000.) & 
                                 (mDOK$T0adc2 > 25.) & (mDOK$T0tdc2 < 1000.) & ( abs(1.0 - mDOK$T0adc1/mDOK$T0adc2) < 0.25  )));
				 
ggplot(mDOKTDCAdc1a2_Tdc1a2_OkP, aes(x=T0tdc1, y=T0tdc2)) + geom_point(aes(color=T0adc1), size=0.5) + 
                  scale_fill_distiller() +  xlim(4.625, 5.50) + ylim(4.625, 5.50);
#
mDOKTDCAdc1a2_Tdc1a2_OkPRPC_5 <- subset(mDOKTDCAdc1a2_Tdc1a2_OkP, (abs(mDOKTDCAdc1a2_Tdc1a2_OkP$RPCtdc5 - 49.) < 5.));
hhRPC_OKP_5 <- hist(mDOKTDCAdc1a2_Tdc1a2_OkPRPC_5$RPCtdc5, seq(from=44., to=52., by=0.25));
mDOKTDCAdc1a2_Tdc1a2_OkPRPC_6 <- subset(mDOKTDCAdc1a2_Tdc1a2_OkP, (abs(mDOKTDCAdc1a2_Tdc1a2_OkP$RPCtdc6 - 49.) < 5.));
hhRPC_OKP_6 <- hist(mDOKTDCAdc1a2_Tdc1a2_OkPRPC_6$RPCtdc6, seq(from=44., to=52., by=0.25));
mDOKTDCAdc1a2_Tdc1a2_OkPRPC_7 <- subset(mDOKTDCAdc1a2_Tdc1a2_OkP, (abs(mDOKTDCAdc1a2_Tdc1a2_OkP$RPCtdc7 - 49.) < 5.));
hhRPC_OKP_7 <- hist(mDOKTDCAdc1a2_Tdc1a2_OkPRPC_7$RPCtdc7, seq(from=44., to=52., by=0.25));
plot(hhRPC_OKP_5$mids+0.1, hhRPC_OKP_5$counts, type='h', lty=1, lwd=2, col="blue", xlab=" RPC ", ylab="Counts/250 ps");
lines(hhRPC_OKP_6$mids-0.1, hhRPC_OKP_6$counts, type='h', lty=2, lwd=2, col="red");
legend(x="topright", c("RPC 5", "RPC 6"), lty=c(1,2), lwd=c(1,2), col=c("blue", "red"))


#
