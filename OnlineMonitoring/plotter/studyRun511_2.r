#
# Rscript for studying data integrity for run 511
#
library(ggplot2)
#
source("/home/lebrun/NEXT/relJan2017/Releases/NEXT_HEAD/RScripts/FuncForFittedClustTl_V3.r"); # just a bunch a simple functions
# set my screen such table are more readable. 
wideScreen();
#
mD <- read.table("/home/lebrun/EMPHATIC/build/SummaryTrigT0RPC_V1_511_V1c.txt", h=T); # 
#
mD10 <- subset(mD, (mD$spill == 10))
mD11 <- subset(mD, (mD$spill == 11))
mD12 <- subset(mD, (mD$spill == 12))
mD2 <- subset(mD, (mD$spill == 2))
mD14 <- subset(mD, (mD$spill == 14))
mDOK <- subset(mD, ((mD$T0OK == 1) & (mD$RPCOK == 1)));
hhTrigAc0 <- hist(mDOK$trigSAdc0, seq(from=-50000., to=150000., by=50), plot=F);
hhTrigAc1 <- hist(mDOK$trigSAdc1, seq(from=-50000., to=150000., by=50), plot=F);
hhTrigAc2 <- hist(mDOK$trigSAdc2, seq(from=-50000., to=150000., by=50), plot=F);
hhTrigAc3 <- hist(mDOK$trigSAdc3, seq(from=-50000., to=150000., by=50), plot=F);
plot(hhTrigAc0$mids, hhTrigAc0$counts, type='b', pch=15, col="black", 
          xlim=c(-10., 10000.), ylim=c(0., 30000.), xlab="Trigger ADC pulse height", ylab=c("counts per 20 ADC counts bins"));
lines(hhTrigAc1$mids, hhTrigAc1$counts + 7500., type='b', pch=16, col="blue");
lines(hhTrigAc2$mids, hhTrigAc2$counts + 15000., type='b',pch=17, col="magenta");
lines(hhTrigAc3$mids, hhTrigAc3$counts + 22500., type='b',pch=18, col="red");
legend(x="topleft", c("Trig ADC 0", "Trig ADC 1", "Trig ADC 2", "Trig ADC 3"), lty=c(1,1,1,1), pch=c(15,16,17, 18), col=c("black", "blue", "magenta", "red"));
#
ggplot(mDOK, aes(x=trigAdc0, y=trigAdc1)) + geom_point(aes(color=trigAdc2), size=0.5) + scale_fill_distiller()
ggplot(mDOK, aes(x=trigAdc0, y=trigAdc2)) + geom_point(aes(color=trigAdc3), size=0.5) + scale_fill_distiller()
ggplot(mDOK, aes(x=trigAdc0, y=trigAdc3)) + geom_point(aes(color=trigAdc1), size=0.5) + scale_fill_distiller()
ggplot(mDOK, aes(x=trigAdc1, y=trigAdc2)) + geom_point(aes(color=trigAdc3), size=0.5) + scale_fill_distiller()
ggplot(mDOK, aes(x=trigAdc1, y=trigAdc3)) + geom_point(aes(color=trigAdc0), size=0.5) + scale_fill_distiller()
ggplot(mDOK, aes(x=trigAdc2, y=trigAdc3)) + geom_point(aes(color=trigAdc1), size=0.5) + scale_fill_distiller()
mDOKLowTrig0 <- subset(mDOK, ((mDOK$trigSAdc0 < 2000)));
mDOKHighTrig0 <- subset(mDOK, ((mDOK$trigSAdc0 >= 2000)));
mDOKHighTrigT0Adc5OK <- subset(mDOKHighTrig0, ((mDOKHighTrig0$T0adc5 >= 9999)));
mDOKLowTrigT0Adc5OK <- subset(mDOKLowTrig0, ((mDOKLowTrig0$T0adc5 >= 9999)));
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



mDOKHighTrigT0Adc5UpDownOK <- subset(mDOKHighTrigT0Adc5OK, (((mDOKHighTrigT0Adc5OK$T0tdc5 < 1.0e5)) & (mDOKHighTrigT0Adc5OK$T0adc13 >= 9999) 
                                                            & (mDOKHighTrigT0Adc5OK$T0tdc13 < 1.0e5)));
mDOKHighTrigT0Adc5UpDownOKT <- subset(mDOKHighTrigT0Adc5UpDownOK, ((abs(mDOKHighTrigT0Adc5UpDownOK$T0tdc5 - 4.9) < 1) & 
                                                                   (abs(mDOKHighTrigT0Adc5UpDownOK$T0tdc13 - 4.9) < 1)));
mDOKHighTrigT0Adc5T0tdc5OK <- subset(mDOKHighTrigT0Adc5OK, ((mDOKHighTrigT0Adc5OK$T0tdc5 < 1.0e5)));


ggplot(mDOKLowTrig0, aes(x=trigSAdc2, y=trigSAdc3)) + geom_point(aes(color=trigSAdc1), size=0.5) + scale_fill_distiller()
mDOKLowTrig01 <- subset(mDOK, ((mDOK$trigAdc0 < 1000) & (mDOK$trigAdc1 < 1000)));
ggplot(mDOKLowTrig01, aes(x=trigAdc2, y=trigAdc3)) + geom_point(aes(color=trigAdc1), size=0.5) + scale_fill_distiller()
mDOKRLowTrig0 <- subset(mDOK, ((mDOK$trigAdc0 < 25)));
ggplot(mDOKRLowTrig0, aes(x=trigAdc1, y=trigAdc2)) + geom_point(aes(color=trigAdc3), size=0.5) + scale_fill_distiller()
mDOKRLowTrig01 <- subset(mDOK, ((mDOK$trigAdc0 < 25)) & (mDOK$trigAdc1 < 25)));
#
hhTADC5LowTrigadc0 <- hist(mDOKLowTrig0$T0adc5, seq(from=0., to=2500., by=50.), plot=F)
hhTADC5HighTrigadc0 <- hist(mDOKHighTrig0$T0adc5, seq(from=0., to=2500., by=50.), plot=F)
plot(hhTADC5HighTrigadc0$mids, 1.0*hhTADC5HighTrigadc0$counts, type='b', pch=15, col="magenta", xlab="T0ADC5", 
           ylab="Counts per 50 ADC counts bin", log='y', ylim=c(0.7, 2.0e5));
lines(hhTADC5LowTrigadc0$mids, 10.0*hhTADC5LowTrigadc0$counts, type='b', pch=16, col="blue");
arrows(hhTADC5LowTrigadc0$mids, 10.0*(hhTADC5LowTrigadc0$counts - sqrt(hhTADC5LowTrigadc0$counts)), 
         hhTADC5LowTrigadc0$mids, 10.0*(hhTADC5LowTrigadc0$counts + sqrt(hhTADC5LowTrigadc0$counts)), code=3, length=0.05, angle=90., col="blue");
hhTADC4LowTrigadc0 <- hist(mDOKLowTrig0$T0adc4, seq(from=0., to=2500., by=50.), plot=F)
hhTADC4HighTrigadc0 <- hist(mDOKHighTrig0$T0adc4, seq(from=0., to=2500., by=50.), plot=F)
plot(hhTADC4HighTrigadc0$mids, 1.0*hhTADC4HighTrigadc0$counts, type='b', pch=15, col="magenta", xlab="T0ADC4", 
           ylab="Counts per 50 ADC counts bin", log='y', ylim=c(0.7, 2.0e5));
lines(hhTADC4LowTrigadc0$mids, 10.0*hhTADC4LowTrigadc0$counts, type='b', pch=16, col="blue");
arrows(hhTADC4LowTrigadc0$mids, 10.0*(hhTADC4LowTrigadc0$counts - sqrt(hhTADC4LowTrigadc0$counts)), 
         hhTADC4LowTrigadc0$mids, 10.0*(hhTADC4LowTrigadc0$counts + sqrt(hhTADC4LowTrigadc0$counts)), code=3, length=0.05, angle=90., col="blue");
legend(x="topright", c("Trigger ADC 4 > 1000", "Trigger ADC 4 < 1000, X 10"), pch=c(15,16), lty=c(1,1), col=c("magenta", "blue"));	 
#plot(mD2$evt, type='p', pch='.', col="black", xlab="Sequential # into the file", ylab="Event #", ylim=c(0., 5000.) xlim=c(0., 5000.)  );
#lines(mD10$evt, type='p', pch='*', cex=0.25, col="blue");
#lines(mD11$evt, type='p', pch='*', cex=0.35, col="red");
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
