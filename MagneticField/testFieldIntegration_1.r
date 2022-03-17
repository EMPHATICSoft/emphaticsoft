#
# Rscript to document the tracking & field precision 
#
mD <- read.table("./EmphMagField_v3.txt", h=T);
plot(mDS2$y, mDS2$dslxLin_E, type='b', pch=15);
mDS21 <- subset(mD, (mD$stepZ > 21.55));
mDS2 <- subset(mD, (mD$stepZ < 2.2));
plot(mDS2$y, mDS2$dslxLin_E, type='b', pch=15, ylim=c(0.01089, 0.01090));
lines(mDS2$y, mDS2$dSlx3DR_E, type='b', pch=16, col="blue");
#
mDYm2 <- subset(mD, (abs(mD$y + 2.) < 0.01));
plot(mDYm2$stepZ, mDYm2$dslxLin_E, type='b', pch=15)
mDp10 <- read.table("./EmphMagField_p10_v3.txt", h=T);
mDp11 <- subset(mDp10, (mDP10$p < 2.))
mDp11Ym2 <- subset(mDp11, (abs(mDp11$y + 2.) < 0.01));
plot(mDp11Ym2$stepZ, mDp11Ym2$dslxLin_E - mDp11Ym2$dslxLin_RK, type='b', pch=15)

