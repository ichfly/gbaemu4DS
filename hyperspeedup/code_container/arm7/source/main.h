#ifdef __cplusplus
extern "C" {
#endif

extern void checkstart();
extern void updatevol();
extern void updatetakt();
extern void enableSound();
extern void vcount_handler();
extern void vblank_handler();

extern u32 power;
extern u32 ie_save;
extern void lid_closing_handler(u32 WAKEUP_IRQS);
extern void lid_open_irq_handler();


#ifdef __cplusplus
}
#endif
