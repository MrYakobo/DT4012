void delay(int d){
    for(int i = 0; i < d; i++)
        asm("nop");
}

void delay_ms(int ms){
    for(int i = 0; i < ms*84*10; i++)
        asm("nop");
}