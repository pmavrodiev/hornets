library(scales)
library(ggplot2)

setwd("/home/pmavrodiev/Documents/data/hornets/code/output/")

ccdf=function(data) {
  data.sorted=sort(data)
  ccdf=1-ecdf(data.sorted)(unique(data.sorted))
}


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
