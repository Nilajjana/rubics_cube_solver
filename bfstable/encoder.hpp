#ifndef ENCODER
#define ENCODER

#include <array>
#include <cstdint>

class Encoder
{
    public:
         uint16_t lehmer8coder(const uint8_t* cp);
         uint16_t encodeCO(const uint8_t* co);
         uint16_t encodeEO(const uint8_t* eo);
         uint16_t encodeSlice(const uint8_t* ep);
         uint8_t lehmer4(const uint8_t* p);
         void decodeCO(uint16_t index, uint8_t* co);
         void decodeEO(uint16_t index, uint8_t* eo);
         void decodeSlice(uint16_t index, uint8_t* ep);
         void decodeLehmer8(uint16_t index, uint8_t (&cp)[8]);
         void decodeLehmer4(uint16_t index, uint8_t* p);
};
#endif // ENCODER
