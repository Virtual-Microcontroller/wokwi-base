void uart_init(void);
void uart_puts(const char *s);
void led_init(void);
void led_set(int on);
void button_init(void);
int  button_pressed(void);

/* Busy waiting rather than a timer, because the point of this project is the
   pins and the serial line. A timer-driven version is a later exercise. */
static void delay(volatile unsigned long n)
{
    while (n--) { }
}

/* How many passes of the loop make up one half of the blink. The loop below
   waits a short time and counts, rather than waiting the whole half-period in
   one go.

   That distinction matters. An earlier version blinked with two long delays
   and read the button once per cycle, so a press only registered if it was
   still held when the loop came round -- a quick press was simply lost, and
   the board felt broken. Reading the input often and driving the output on a
   count keeps one from setting the pace of the other. */
#define BLINK_PASSES 100

/* How many consecutive passes the button must read the same before the change
   is believed.

   A mechanical switch does not close once. Its contacts separate and reconnect
   many times over roughly a millisecond, so a single press produces a burst of
   edges. Reading the pin quickly enough to feel responsive is also fast enough
   to see every one of them, and without this the firmware reports one press
   several times.

   The count is deliberately generous: it need only outlast the chatter, and a
   delay of a few milliseconds before a press is believed is far below what a
   person can notice. */
#define DEBOUNCE_PASSES 20

/* Both characters are needed. A serial terminal emulates a teletype, where
   line feed moves the carriage down and carriage return moves it back to the
   left margin. Sending only \n moves down without returning, so each line
   starts where the previous one ended and the output walks off the screen. */
#define EOL "\r\n"

int main(void)
{
    uart_init();
    led_init();
    button_init();

    uart_puts("Hello from Blue Pill!" EOL);

    /* What the pin currently reads, how long it has read that, and what the
       firmware has actually accepted as the button's state. */
    int candidate = 0;
    unsigned steady = 0;
    int accepted = 0;

    int lit = 0;
    unsigned passes = 0;

    for (;;) {
        int raw = button_pressed();

        if (raw != candidate) {
            candidate = raw;
            steady = 0;
        } else if (steady < DEBOUNCE_PASSES) {
            steady++;
        }

        /* Act on the edge, once the reading has held still long enough to
           trust. Acting on the level instead would print continuously for as
           long as a finger rested on the button. */
        if (steady >= DEBOUNCE_PASSES && candidate != accepted) {
            accepted = candidate;
            if (accepted) {
                uart_puts("Button pressed" EOL);
            }
        }

        if (++passes >= BLINK_PASSES) {
            passes = 0;
            lit = !lit;
            led_set(lit);
        }

        delay(1000);
    }
}
