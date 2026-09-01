#include <stdint.h>

#define RCC_BASE    0x40021000UL
#define RCC_APB2ENR (*(volatile uint32_t *)(RCC_BASE + 0x18))
#define RCC_IOPAEN  (1u << 2)
#define RCC_IOPCEN  (1u << 4)

#define GPIOA_BASE 0x40010800UL
#define GPIOA_CRL  (*(volatile uint32_t *)(GPIOA_BASE + 0x00))
#define GPIOA_IDR  (*(volatile uint32_t *)(GPIOA_BASE + 0x08))
#define GPIOA_ODR  (*(volatile uint32_t *)(GPIOA_BASE + 0x0C))
#define GPIOA_BSRR (*(volatile uint32_t *)(GPIOA_BASE + 0x10))

#define GPIOC_BASE 0x40011000UL
#define GPIOC_CRH  (*(volatile uint32_t *)(GPIOC_BASE + 0x04))
#define GPIOC_BSRR (*(volatile uint32_t *)(GPIOC_BASE + 0x10))

/* BSRR sets a pin from its low half and clears it from its high half, so a pin
   is driven with a single write and no read-modify-write. Two pieces of code
   touching different pins of the same port therefore cannot lose each other's
   change, which ODR does not guarantee. */
#define BSRR_SET(pin)   (1u << (pin))
#define BSRR_CLEAR(pin) (1u << ((pin) + 16))

void led_init(void)
{
    RCC_APB2ENR |= RCC_IOPCEN | RCC_IOPAEN;

    /* PC13 drives the LED fitted to the board. CRH covers pins 8 to 15, so
       pin 13 sits at bits 20 to 23. 0x2 is output push-pull at 2 MHz. */
    GPIOC_CRH &= ~(0xFu << 20);
    GPIOC_CRH |=  (0x2u << 20);

    /* PA1 drives the LED wired on the breadboard. CRL covers pins 0 to 7, so
       pin 1 sits at bits 4 to 7. */
    GPIOA_CRL &= ~(0xFu << 4);
    GPIOA_CRL |=  (0x2u << 4);
}

/* The board LED is wired between 3.3 V and PC13, so the pin sinks the current
   and a low level is what lights it. The breadboard LED is wired the ordinary
   way round and lights on a high level. Both are driven from one call so an
   exercise does not have to remember which is which. */
void led_set(int on)
{
    GPIOC_BSRR = on ? BSRR_CLEAR(13) : BSRR_SET(13);
    GPIOA_BSRR = on ? BSRR_SET(1)    : BSRR_CLEAR(1);
}

void button_init(void)
{
    RCC_APB2ENR |= RCC_IOPAEN;

    /* PA0 reads the push button. 0x8 is input with the pull resistor enabled;
       ODR then chooses which way it pulls, and a 1 selects pull-up. The button
       connects the pin to ground, so a press reads as 0. */
    GPIOA_CRL &= ~(0xFu << 0);
    GPIOA_CRL |=  (0x8u << 0);
    GPIOA_ODR |= (1u << 0);
}

int button_pressed(void)
{
    return (GPIOA_IDR & (1u << 0)) == 0;
}
