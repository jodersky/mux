#include "mux/io.h"
#include "mux/list.h"
#include "mux/rbuffer.h"
#include "mux/sched.h"
#include "mux/lock.h"
#include "mcu/usart.h"
#include "mcu/context.h"
#include <avr/interrupt.h>
#include <avr/io.h>

struct usart_private {
    char __rx_buffer[USART_BUFFER_SIZE];
    char __tx_buffer[USART_BUFFER_SIZE];

    struct rbuffer_t rx_buffer;
    struct rbuffer_t tx_buffer;

    struct list_head rx_queue;
    struct list_head tx_queue;

    volatile unsigned char* const ucsrxa;
    volatile unsigned char* const ubrrxh;
    volatile unsigned char* const ubrrxl;
    volatile unsigned char* const ucsrxb;
    char u2xx;
    char rxenx;
    char txenx;
    char rxciex;
    char udriex;
};

int usart_open(struct file* usart) {
    struct usart_private* priv = (struct usart_private*) usart->private_data;

    INIT_LIST_HEAD(&priv->rx_queue);
    INIT_LIST_HEAD(&priv->tx_queue);
    INIT_RBUFFER(&priv->rx_buffer, priv->__rx_buffer, USART_BUFFER_SIZE);
    INIT_RBUFFER(&priv->tx_buffer, priv->__tx_buffer, USART_BUFFER_SIZE);

    return 0;
}

int usart_ioctl(struct file* usart, int cmd, long args) {
    struct usart_private* priv = (struct usart_private*) usart->private_data;

    switch (cmd) {
        case IOCTL_SET_BAUD: {
            long baud = args;
            (*priv->ucsrxa) |= (1 << priv->u2xx);
            long baud_setting = (F_CPU / 4 / baud - 1) / 2;
            (*priv->ubrrxh) = baud_setting >> 8;
            (*priv->ubrrxl) = baud_setting;
            (*priv->ucsrxb) |= (1 << priv->rxenx) | (1 << priv->txenx) | (1 << priv->rxciex);
            (*priv->ucsrxb) &= ~(1 << priv->udriex);
            return 0;
        }
        default:
            return -1;
    }
}

ssize_t usart_write(struct file* usart, const char* const buffer, size_t length) {
    struct usart_private* priv = (struct usart_private*) usart->private_data;
    volatile unsigned char* ucsrxb = (priv->ucsrxb);
    char write_enable = (1 << priv->udriex);

    ssize_t wrote = 0;
    int r = 0;
    do {
        cli();
        r = rbuffer_write(&priv->tx_buffer, buffer[wrote]);
        wrote += 1;
        sei();
        *ucsrxb |= write_enable;
    } while (r == 0 && wrote < length);

    return wrote;
}

ssize_t usart_read(struct file* usart, char* const buffer, size_t length) {
    struct usart_private* priv = (struct usart_private*) usart->private_data;

    while (rbuffer_empty(&priv->rx_buffer)) {
        sleep_queue(&priv->rx_queue);
        yield();
    }
    size_t read = 0;
    size_t r = 0;
    while (!rbuffer_empty(&priv->rx_buffer) && read < length && r == 0) {
        cli();
        r = rbuffer_read(&priv->rx_buffer, buffer + read);
        read += 1;
        sei();
    }
    return read;
}

int usart_close(struct file* usart) {
    return 0;
}

struct file_operations usart_fops = {
    .open = usart_open,
    .ioctl = usart_ioctl,
    .read = usart_read,
    .write = usart_write,
    .close = usart_close
};

static struct usart_private _usart0 = {
    .ucsrxa = &UCSR0A,
    .ubrrxh = &UBRR0H,
    .ubrrxl = &UBRR0L,
    .ucsrxb = &UCSR0B,
    .u2xx = U2X0,
    .rxenx = RXEN0,
    .txenx = TXEN0,
    .rxciex = RXCIE0,
    .udriex = UDRIE0
};

struct file usart0 = {
    .fops = &usart_fops,
    .private_data = &_usart0
};

//called when data register is empty
ISR(USART0_UDRE_vect) {
    struct usart_private* priv = (struct usart_private*) usart0.private_data;
    char c;

    if (rbuffer_read(&priv->tx_buffer, &c) == 0) {
        UDR0 = c;
    } else {
        UCSR0B &= ~(1 << UDRIE0); //buffer empty, disable interruot
    }
}

//called when byte is received
ISR(USART0_RX_vect) {

    struct usart_private* priv = (struct usart_private*) usart0.private_data;
    
    char c = UDR0;
    rbuffer_write(&priv->rx_buffer, c);
    wake_all_queue(&priv->rx_queue);
}