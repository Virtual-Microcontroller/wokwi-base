#include <stdint.h>

/* Reset and clock control. Every peripheral on this part leaves reset with its
   clock switched off, and registers of an unclocked peripheral read as zero and
   ignore writes. The enable below is therefore not optional setup: without it
   uart_init appears to succeed, TXE never becomes set, and uart_putc spins
   forever on a flag that cannot change. */
#define RCC_BASE     0x40021000UL
#define RCC_APB2ENR  (*(volatile uint32_t *)(RCC_BASE + 0x18))
#define RCC_IOPAEN   (1u << 2)
#define RCC_USART1EN (1u << 14)

#define GPIOA_BASE 0x40010800UL
#define GPIOA_CRH  (*(volatile uint32_t *)(GPIOA_BASE + 0x04))

#define USART1_BASE 0x40013800UL
#define USART_SR    (*(volatile uint32_t *)(USART1_BASE + 0x00))
#define USART_DR    (*(volatile uint32_t *)(USART1_BASE + 0x04))
#define USART_BRR   (*(volatile uint32_t *)(USART1_BASE + 0x08))
#define USART_CR1   (*(volatile uint32_t *)(USART1_BASE + 0x0C))

#define SR_TXE (1u << 7)
#define CR1_RE (1u << 2)
#define CR1_TE (1u << 3)
#define CR1_UE (1u << 13)

void uart_init(void)
{
    RCC_APB2ENR |= RCC_IOPAEN | RCC_USART1EN;

    /* PA9 carries USART1_TX. CRH holds four configuration bits for each of
       pins 8 to 15, so pin 9 sits at bits 4 to 7. 0xB is alternate-function
       push-pull at 50 MHz, which is what a peripheral driving the pin needs;
       leaving it as a plain output would let the GPIO block win and the UART
       signal would never reach the pin. */
    GPIOA_CRH &= ~(0xFu << 4);
    GPIOA_CRH |=  (0xBu << 4);

    /* PA10 carries USART1_RX. 0x4 is floating input. */
    GPIOA_CRH &= ~(0xFu << 8);
    GPIOA_CRH |=  (0x4u << 8);

    /* The part boots on its internal 8 MHz oscillator and nothing here
       reconfigures the clock tree, so APB2 also runs at 8 MHz.
       8000000 / (16 * 115200) = 4.34, giving a mantissa of 4 and a fraction of
       5/16. Changing the clock setup without changing this line changes the
       baud rate, and the symptom is unreadable output rather than no output. */
    USART_BRR = (4u << 4) | 5u;

    USART_CR1 = CR1_UE | CR1_TE | CR1_RE;
}

void uart_putc(char c)
{
    while (!(USART_SR & SR_TXE)) { }
    USART_DR = (uint8_t)c;
}

void uart_puts(const char *s)
{
    while (*s) {
        uart_putc(*s++);
    }
}
