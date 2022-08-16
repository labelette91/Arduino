#pragma once

uint32_t GetRaw1bValue(uint8_t* data, uint16_t offset, uint8_t size);

uint32_t getRaw32bValue(uint8_t* data, uint8_t offset, uint8_t size);

uint16_t getRaw16bValue(uint8_t* data, uint8_t offset, uint8_t size);

uint8_t getRaw08bValue(uint8_t* data, uint8_t offset, uint8_t size);
