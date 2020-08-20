#include "mcu_queue.h"

int8_t MCU_Queue_Initialize(mcu_queue * queue, uint8_t * buffer, uint32_t size)
{
  uint32_t address;
  int8_t return_value;
  
  if (size > 4U)
  {
    queue->buffer = buffer;
    queue->first = 0U;
    queue->last = 0U;
    queue->length = 0U;
    queue->size = size - 4U;
    
    /* initialize cannary */
    address = queue->size + 3U;
    queue->buffer[address] = 'P';
    address = address - 1U;
    queue->buffer[address] = 'C';
    address = address - 1U;
    queue->buffer[address] = 'Q';
    address = address - 1U;
    queue->buffer[address] = 'U';
    
    /* */
    return_value = MCU_QUEUE_SUCCESS;
  }
  else
  {
    return_value = MCU_QUEUE_UNINITIALIZED;
  }
  
  return return_value;
}

int8_t MCU_Queue_Length(mcu_queue * queue, uint32_t * length)
{
  uint32_t cannary;
  uint32_t address;
  int8_t return_value;
  
  if (queue->size > 4U)
  {
    address = queue->size + 3U;
    cannary = (uint32_t) queue->buffer[address];
    address = address - 1U;
    cannary = cannary << 0x08U;
    cannary = cannary | ((uint32_t) (queue->buffer[address]));
    address = address - 1U;
    cannary = cannary << 0x08U;
    cannary = cannary | ((uint32_t) (queue->buffer[address]));
    address = address - 1U;
    cannary = cannary << 0x08U;
    cannary = cannary | ((uint32_t) (queue->buffer[address]));
    
    if (cannary == 0x50435155U)
    {
      length[0] = queue->length;
      return_value = MCU_QUEUE_SUCCESS;
    }
    else
    {
      return_value = MCU_QUEUE_MEMORY_LEAK;
    }
  }
  else
  {
    return_value = MCU_QUEUE_UNINITIALIZED;
  }
  
  return return_value;
}

int8_t MCU_Queue_Push(mcu_queue * queue, uint8_t * buffer, uint32_t size)
{
  uint32_t cannary;
  uint32_t address;
  uint32_t counter;
  int8_t return_value;
  
  counter = 0U;
  
  if (queue->size > 4U)
  {
    address = queue->size + 3U;
    cannary = (uint32_t) queue->buffer[address];
    address = address - 1U;
    cannary = cannary << 0x08U;
    cannary = cannary | ((uint32_t) (queue->buffer[address]));
    address = address - 1U;
    cannary = cannary << 0x08U;
    cannary = cannary | ((uint32_t) (queue->buffer[address]));
    address = address - 1U;
    cannary = cannary << 0x08U;
    cannary = cannary | ((uint32_t) (queue->buffer[address]));
    
    if (cannary == 0x50435155U)
    {
      return_value = MCU_QUEUE_SUCCESS;
      
      while (counter < size)
      {
        queue->buffer[queue->last] = buffer[counter];
        queue->last = queue->last + 1U;
        if (queue->last >= queue->size)
        {
          queue->last = 0U;
        }
        if (queue->length < queue->size)
        {
          queue->length = queue->length + 1U;
        }
        else
        {
          return_value = MCU_QUEUE_OVERFLOW;
        }
        counter = counter + 1U;
      }
      address = queue->size;
    }
    else
    {
      return_value = MCU_QUEUE_MEMORY_LEAK;
    }
  }
  else
  {
    return_value = MCU_QUEUE_UNINITIALIZED;
  }
  
  return return_value;
}

int8_t MCU_Queue_Push_Decimal(mcu_queue * queue, uint8_t value)
{
  uint32_t cannary;
  uint32_t address;
  uint8_t characters[3];
  uint8_t counter;
  int8_t return_value;
  
  if (queue->size > 4U)
  {
    address = queue->size + 3U;
    cannary = (uint32_t) queue->buffer[address];
    address = address - 1U;
    cannary = cannary << 0x08U;
    cannary = cannary | ((uint32_t) (queue->buffer[address]));
    address = address - 1U;
    cannary = cannary << 0x08U;
    cannary = cannary | ((uint32_t) (queue->buffer[address]));
    address = address - 1U;
    cannary = cannary << 0x08U;
    cannary = cannary | ((uint32_t) (queue->buffer[address]));
    
    if (cannary == 0x50435155U)
    {
      characters[2] = '0';
      characters[1] = '0';
      characters[0] = '0';
      
      while (value > 99U)
      {
        value = value - 100U;
        characters[2] = characters[2] + 1U;
      }
      
      while (value > 9U)
      {
        value = value - 10U;
        characters[1] = characters[1] + 1U;
      }
      
      while (value > 0U)
      {
        value = value - 1U;
        characters[0] = characters[0] + 1U;
      }
      
      if (characters[2] > '0')
      {
        counter = 3U;
        
      }
      else if (characters[1] > '0')
      {
        counter = 2U;
      }
      else
      {
        counter = 1U;
      }
      
      queue->length = queue->length + counter;
      
      while (counter > 0U)
      {
        counter = counter - 1U;
        queue->buffer[queue->last] = characters[counter];
        queue->last = queue->last + 1U;
        if (queue->last >= queue->size)
        {
          queue->last = 0U;
        }
      }
      
      if (queue->length > queue->size)
      {
        return_value = MCU_QUEUE_OVERFLOW;
        queue->length = queue->size;
      }
      else
      {
        return_value = MCU_QUEUE_SUCCESS;
      }
    }
    else
    {
      return_value = MCU_QUEUE_MEMORY_LEAK;
    }
  }
  else
  {
    return_value = MCU_QUEUE_UNINITIALIZED;
  }
  
  return return_value;
}

int8_t MCU_Queue_Pop(mcu_queue * queue, uint8_t * character)
{
  uint32_t cannary;
  uint32_t address;
  int8_t return_value;
  
  if (queue->size > 0U)
  {
    if (queue->length > 0U)
    {
      address = queue->size + 3U;
      cannary = (uint32_t) queue->buffer[address];
      address = address - 1U;
      cannary = cannary << 0x08U;
      cannary = cannary | ((uint32_t) (queue->buffer[address]));
      address = address - 1U;
      cannary = cannary << 0x08U;
      cannary = cannary | ((uint32_t) (queue->buffer[address]));
      address = address - 1U;
      cannary = cannary << 0x08U;
      cannary = cannary | ((uint32_t) (queue->buffer[address]));
      
      if (cannary == 0x50435155U)
      {
        queue->length = queue->length - 1U;
        character[0] = queue->buffer[queue->first];
        queue->first = queue->first + 1U;
        if (queue->first >= queue->size)
        {
          queue->first = 0U;
        }
        return_value = MCU_QUEUE_SUCCESS;
      }
      else
      {
        return_value = MCU_QUEUE_MEMORY_LEAK;
      }
    }
    else
    {
      return_value = MCU_QUEUE_EMPTY;
    }
  }
  else
  {
    return_value = MCU_QUEUE_UNINITIALIZED;
  }
  
  return return_value;
}
