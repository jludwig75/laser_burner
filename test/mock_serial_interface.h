#pragma once

#include <vector>
#include <stdint.h>


void reset_rx_buffer();
void add_bytes_to_receive(const std::vector<uint8_t> &bytes);
