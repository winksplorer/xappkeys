#ifndef XAK_INPUT_H
#define XAK_INPUT_H

int xak_input_init(void);
int xak_input_handle(KeyBinding binds[], int num_binds);
void xak_input_close(void);
void xak_input_child_close(void);

#endif