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
mD11OK <- subset(mDOK, (mDOK$spill == 11))
mD12OK <- subset(mDOK, (mDOK$spill == 12))
mDnOK <- subset(mD, ((mD$T0OK == 0) | (mD$RPCOK == 0)));
mD11nOK <- subset(mDOK, (mDOK$spill == 11))
mD12nOK <- subset(mDOK, (mDOK$spill == 12))

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
