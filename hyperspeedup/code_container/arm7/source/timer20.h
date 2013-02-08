typedef u32 (*Functiondec)(u8 back);

void timerlen();
void timerother();
void initimer();
void timerlenadd(u8 chan,u32 val,Functiondec func);
void timerotheradd(u8 chan,u32 val,Functiondec func);