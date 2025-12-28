#ifndef __UART_TYPES_H__
#define __UART_TYPES_H__

#ifdef __cplusplus
extern "C" {
#endif

#define USART_TX_RING_SIZE 1024
#define USART_RX_RING_SIZE 1024

typedef enum {
    UART_TX_RESULT_FAILURE = -1,
    UART_TX_RESULT_QUEUED = 0,
} uart_dma_enqueue_tx_result_t;

typedef enum {
    UART_RX_RESULT_FAILURE = -1,
    UART_RX_RESULT_QUEUED = 0,
} uart_dma_enqueue_rx_result_t;

typedef struct {
    uint8_t data[USART_TX_RING_SIZE];
    size_t head;
    size_t tail;
    size_t length;
    size_t available_size;
    size_t dma_last_size;
    int dma_busy;
} uart_dma_tx_ring_t;

typedef struct {
    uint8_t data[USART_RX_RING_SIZE];
    size_t head;
    size_t tail;
    size_t length;
    size_t available_size;
    size_t dma_last_size;
    size_t dma_received_during_current_transfer;
    int dma_busy;
} uart_dma_rx_ring_t;

size_t uart_get_free_tx_ring_space(uart_dma_tx_ring_t* ring);
size_t uart_get_tx_free_ring_space(uart_dma_tx_ring_t* ring);
size_t uart_get_free_rx_ring_space(uart_dma_rx_ring_t* ring);
size_t uart_get_pending_rx_ring_space(uart_dma_rx_ring_t* ring);
int uart_append_tx_ring(uart_dma_tx_ring_t* ring, uint8_t* source, size_t size);
void uart_alloc_tx_ring_space(uart_dma_tx_ring_t* ring, size_t size);
void uart_free_tx_ring_space(uart_dma_tx_ring_t* ring, size_t size);
int uart_read_rx_ring(uart_dma_rx_ring_t* ring, uint8_t* destination, size_t size);
void uart_alloc_rx_ring_space(uart_dma_rx_ring_t* ring, size_t size);
void uart_free_rx_ring_space(uart_dma_rx_ring_t* ring, size_t size);

#ifdef __cplusplus
}
#endif

#endif
