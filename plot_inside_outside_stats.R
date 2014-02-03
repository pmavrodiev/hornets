library(scales)
library(ggplot2)
library(Cairo)
library(sfsmisc)

setwd("/home/pmavrodiev/Documents/data/hornets/code/output/")

ccdf=function(data) {
  data.sorted=sort(data)
  ccdf=1-ecdf(data.sorted)(unique(data.sorted))
}
#nest transitions
nest=read.table("nest_transitions.dat",header=FALSE,colClasses="character",
                sep="\t")
nest.minutes=as.numeric(nest[,4])
nest.trans.type=as.numeric(nest[,5])
nest.D_2D = nest.minutes[which(nest.trans.type==-1)]
nest.2D_D = nest.minutes[which(nest.trans.type==1)]
nest.D_2D.min=quantile(nest.D_2D)

#outside
outside = read.table("outside.dat",colClasses="character",sep="\t")
outside.minutes=as.numeric(outside[,4])
outside.min = quantile(outside.minutes,probs=c(0.025,0.975))[1]
outside.max = quantile(outside.minutes,probs=c(0.025,0.975))[2]
outside.subset = outside.minutes[outside.minutes >= outside.min &
                                   outside.minutes <= outside.max]
outside.subset2 = outside.minutes[outside.minutes >= 2 &
                                    outside.minutes <= outside.max]
##nest D
inside.D = read.table("inside_D.dat",colClasses="character",sep="\t")
inside.D.minutes=as.numeric(inside.D[,4])
inside.D.min = quantile(inside.D.minutes,probs=c(0.025,0.975))[1]
inside.D.max = quantile(inside.D.minutes,probs=c(0.025,0.975))[2]
inside.D.subset = inside.D.minutes[inside.D.minutes >= inside.D.min &
                                   inside.D.minutes <= inside.D.max]
midsD=hist(inside.D.subset,breaks=100,plot=FALSE)$mids
countsD=hist(inside.D.subset,breaks=100,plot=FALSE)$counts
##nest 2D
inside.2D = read.table("inside_2D.dat",colClasses="character",sep="\t")
inside.2D.minutes=as.numeric(inside.2D[,4])
inside.2D.min = quantile(inside.2D.minutes,probs=c(0.025,0.975))[1]
inside.2D.max = quantile(inside.2D.minutes,probs=c(0.025,0.975))[2]
inside.2D.subset = inside.2D.minutes[inside.2D.minutes >= inside.2D.min &
                                     inside.2D.minutes <= inside.2D.max]
mids2D=hist(inside.2D.subset,breaks=90,plot=FALSE)$mids
counts2D=hist(inside.2D.subset,breaks=90,plot=FALSE)$counts

CairoPDF(file="analysis.pdf",width=30,height=15)
par(mfrow=c(1,2))
par(mar=c(7,12,5,1),mgp=c(6,2,0))
{
#D
plot(midsD,countsD/sum(countsD),xaxt="n",yaxt="n",xlab="",ylab="",type="s",
     lwd=3)
lines(midsD,countsD/sum(countsD),type="h",lwd=2)
abline(v=mean(inside.D.subset),lwd=3,col="blue")
abline(v=median(inside.D.subset),lwd=3,col="green")
axis(1,at=seq(0,240,by=20),label=seq(0,240,by=20),cex.axis=3,cex.lab=3)
axis(2,at=seq(0,0.14,by=0.02),label=seq(0,0.14,by=0.02),las=2,
     cex.axis=3,cex.lab=3)
mtext("duration (minutes)",side=1,line=5,cex=3)
mtext("frequency",side=2,line=9,cex=3)
title("Nest D - inside",cex.main=3)
legend("topright",col=c("green","blue"),lwd=c(3,3),legend=c(round(median(inside.D.subset),1),round(mean(inside.D.subset),1)),bty="n",cex=3)
text(x=61,y=0.14,labels="median",cex=3)
text(x=61,y=0.132,labels="mean",cex=3)

#2D
plot(mids2D,counts2D/sum(counts2D),xaxt="n",yaxt="n",xlab="",ylab="",type="s",
     lwd=3)
lines(mids2D,counts2D/sum(counts2D),type="h",lwd=2)
abline(v=mean(inside.2D.subset),lwd=3,col="blue")
abline(v=median(inside.2D.subset),lwd=3,col="green")

axis(1,at=seq(0,240,by=40),label=seq(0,240,by=40),cex.axis=3,cex.lab=3)
axis(2,at=seq(0,0.26,by=0.02),label=seq(0,0.26,by=0.02),las=2,
     cex.axis=3,cex.lab=3)
mtext("duration (minutes)",side=1,line=5,cex=3)
mtext("frequency",side=2,line=9,cex=3)
title("Nest 2D - inside",cex.main=3)
legend("topright",col=c("green","blue"),lwd=c(3,3),legend=c(round(median(inside.2D.subset),1),round(mean(inside.2D.subset),1)),bty="n",cex=3)
text(x=175,y=0.2443,labels="median",cex=3)
text(x=175,y=0.232,labels="mean",cex=3)
}
####

## outside
{
mids=hist(outside.subset,breaks=100,plot=FALSE)$mids
counts=hist(outside.subset,breaks=100,plot=FALSE)$counts
mids2=hist(outside.subset2,breaks=100,plot=FALSE)$mids
counts2=hist(outside.subset2,breaks=100,plot=FALSE)$counts

plot(mids,counts/sum(counts),xaxt="n",yaxt="n",xlab="",ylab="",type="s",
     lwd=3)
lines(mids,counts/sum(counts),type="h",lwd=2)
abline(v=mean(outside.subset),lwd=3,col="blue")
abline(v=median(outside.subset),lwd=3,col="green")
axis(1,at=seq(0,80,by=20),label=seq(0,80,by=20),cex.axis=3,cex.lab=3)
axis(2,at=seq(0,0.22,by=0.04),label=seq(0,0.22,by=0.04),las=2,
     cex.axis=3,cex.lab=3)
mtext("duration (minutes)",side=1,line=5,cex=3)
mtext("frequency",side=2,line=9,cex=3)
title("outside [all data]",cex.main=3)
legend("topright",col=c("green","blue"),lwd=c(3,3),legend=c(round(median(outside.subset),1),round(mean(outside.subset),1)),bty="n",cex=3)
text(x=50,y=0.21,labels="median",cex=3)
text(x=50,y=0.1975,labels="mean",cex=3)
#outside data<120 seconds
plot(mids2,counts2/sum(counts2),xaxt="n",yaxt="n",xlab="",ylab="",type="s",
     lwd=3)
lines(mids2,counts2/sum(counts2),type="h",lwd=2)
abline(v=mean(outside.subset2),lwd=3,col="blue")
abline(v=median(outside.subset2),lwd=3,col="green")
axis(1,at=seq(0,80,by=20),label=seq(0,80,by=20),cex.axis=3,cex.lab=3)
axis(2,at=seq(0,0.04,by=0.01),label=seq(0,0.04,by=0.01),las=2,
     cex.axis=3,cex.lab=3)
mtext("duration (minutes)",side=1,line=5,cex=3)
mtext("frequency",side=2,line=9,cex=3)
title("outside [>=120 seconds]",cex.main=3)
legend("topright",col=c("green","blue"),lwd=c(3,3),legend=c(round(median(outside.subset2),1),round(mean(outside.subset2),1)),bty="n",cex=3)
text(x=50,y=0.0405,labels="median",cex=3)
text(x=50,y=0.0382,labels="mean",cex=3)
}


x=sort(unique(outside.subset2))
y=ccdf(outside.subset2)
par(mfrow=c(1,1))
plot(x,log10(y),type="l",xaxt="n",yaxt="n",xlab="",ylab="",lwd=3)
x2=rexp(100000,1/mean(outside.subset2))
lines(sort(unique(x2)),log10(ccdf(x2)),col="red",lwd=3)
axis(1,at=seq(0,70,by=10),label=seq(0,70,by=10),cex.axis=3)
source("/home/pmavrodiev/Programs/rank-experiment/rank-experiment/logarithmic_axes.R")
aY=seq(-4,0)
axis(2,at=aY,label=axTexpr(1,10^aY,drop.1=TRUE),cex.axis=3,las=2,hadj=0.9,
     tcl=3*par("tcl"))
axis(2,at=log10(w),label=FALSE,tcl=1.5*par("tcl"),cex=3,cex.lab=3)
mtext("D (minutes)",side=1,line=5,cex=3)
mtext("Probability [duration spent outside > D]",side=2,line=8,cex=3)
title("complementary cdf of time spent outside",cex.main=3)
legend("topright",legend=c("empirical data","exponential fit with rate 0.06"),
       lwd=c(3,3),col=c("black","red"),cex=3,bty="n")
dev.off()



readings = read.table("inside_outside_stats.dat",colClasses="numeric")

inside = readings[,1]
outside = readings[,2]

#inside
inside.1 = quantile(inside,probs=c(0.025,0.5,0.975))[1]
inside.2 = quantile(inside,probs=c(0.025,0.5,0.975))[3]
inside.95.percent=inside[inside>=inside.1 & inside<=inside.2]
hist(inside.95.percent,breaks=100)
plot(sort(unique(inside.95.percent)),
     log10(ccdf(inside.95.percent)),type="l")
x=rexp(100000,1/mean(inside.95.percent))
lines(sort(unique(x)),log10(ccdf(x)),col="blue")

ks.test(unique(inside.95.percent),"pexp",1/mean(inside.95.percent))

#outside

readings.df=data.frame(
  inside=readings[,1],
  outside=readings[,2]
  )
rm(readings)

min.inside=min(readings.df$inside,na.rm=TRUE)                 
max.inside=max(readings.df$inside,na.rm=TRUE)
min.outside=min(readings.df$outside,na.rm=TRUE)
max.outside=max(readings.df$outside,na.rm=TRUE)

plot(subset(readings.df,readings.df$inside<40000),type="p")
g=ggplot(subset(readings.df,readings.df$inside<40000), aes(x=inside,
                          y=..count../sum(..count..))) + 
  geom_histogram()+
  scale_x_datetime(limits=
                     as.POSIXct(c(min.inside,max.inside),
                                format="%H:%M:%S"))


plot(g)

  scale_y_datetime(breaks=date_breaks("4 hour"), labels=date_format("%H:%M")) + 
  theme(axis.text.x=element_text(angle=90))
