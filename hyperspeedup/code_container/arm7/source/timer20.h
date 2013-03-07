typedef u32 (*Functiondec)(u8 back);

void timerlen();
void initimer();
void timerlenadd(u8 chan,u32 val,Functiondec func);
#define timerotheradd(chan,val,func)  timerlenadd(chan + 4,val,func)