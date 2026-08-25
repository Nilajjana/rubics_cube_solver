#include <array>
#include <cstdint>
#include <iostream>
#include <cstdlib>
#include "encoder.hpp"
#include <stdexcept>

uint16_t Encoder::lehmer8coder(const uint8_t* cp)
{
    uint16_t idx = 0;

    int c0 = (cp[1] < cp[0]) +
             (cp[2] < cp[0]) +
             (cp[3] < cp[0]) +
             (cp[4] < cp[0]) +
             (cp[5] < cp[0]) +
             (cp[6] < cp[0]) +
             (cp[7] < cp[0]);

    int c1 = (cp[2] < cp[1]) +
             (cp[3] < cp[1]) +
             (cp[4] < cp[1]) +
             (cp[5] < cp[1]) +
             (cp[6] < cp[1]) +
             (cp[7] < cp[1]);

    int c2 = (cp[3] < cp[2]) +
             (cp[4] < cp[2]) +
             (cp[5] < cp[2]) +
             (cp[6] < cp[2]) +
             (cp[7] < cp[2]);

    int c3 = (cp[4] < cp[3]) +
             (cp[5] < cp[3]) +
             (cp[6] < cp[3]) +
             (cp[7] < cp[3]);

    int c4 = (cp[5] < cp[4]) +
             (cp[6] < cp[4]) +
             (cp[7] < cp[4]);

    int c5 = (cp[6] < cp[5]) +
             (cp[7] < cp[5]);

    int c6 = cp[7] < cp[6];
    return (c0 * 5040 +
           c1 * 720  +
           c2 * 120  +
           c3 * 24   +
           c4 * 6    +
           c5 * 2    +
           c6);
}
uint16_t Encoder::encodeCO(const uint8_t* co)
{
    uint16_t index = 0;

    for (int i = 0; i < 7; ++i)
        index = index * 3 + co[i];

    return index;
}
uint16_t Encoder::encodeEO(const uint8_t* eo)
{
    uint16_t index = 0;
    for (int i = 0; i < 11; ++i)
        index = (index << 1) | eo[i];

    return index;
}
int C(int n, int k)
{
    if (k < 0|| k > n)
        return 0;
    if (k == 0 || k == n)
        return 1;
    if (k > n/2)
        k = n - k;
    
    int res = 1;
    for(int i=0;i<k;i++)
        res = res * (n - i)/(i + 1);
    return res;
}
uint16_t Encoder::encodeSlice(const uint8_t* ep)
{
    int pos[4];
    int k = 0;

    for (int i = 0; i < 12; ++i)
    {
        if (ep[i] >= 8)
        {
            if (k >= 4)
            {
                std::cerr << "ERROR: more than 4 slice edges!\n";

                for (int j = 0; j < 12; ++j)
                    std::cerr << static_cast<int>(ep[j]) << ' ';

                std::cerr << '\n';

                std::abort();
            }

            pos[k++] = i;
        }
    }

    if (k != 4)
    {
        std::cerr << "ERROR: expected 4 slice edges, got "
                  << k << '\n';

        std::abort();
    }

    return static_cast<uint16_t>(
        C(pos[0], 1) +
        C(pos[1], 2) +
        C(pos[2], 3) +
        C(pos[3], 4)
    );
}
uint8_t Encoder::lehmer4(const uint8_t* p)
{
    int c0 = (p[1] < p[0]) +
             (p[2] < p[0]) +
             (p[3] < p[0]);

    int c1 = (p[2] < p[1]) +
             (p[3] < p[1]);

    int c2 = (p[3] < p[2]);

    return uint8_t(c0 * 6 +
           c1 * 2 +
           c2);
}


void Encoder::decodeCO(uint16_t index, uint8_t* co)
{
    int sum = 0;

    for (int i = 6; i >= 0; --i)
    {
        co[i] = index % 3;
        index /= 3;

        sum += co[i];
    }

    co[7] = (3 - (sum % 3)) % 3;
}

void Encoder::decodeEO(uint16_t index, uint8_t* eo)
{
    int sum = 0;

    for (int i = 10; i >= 0; --i)
    {
        eo[i] = index % 2;
        index /= 2;

        sum += eo[i];
    }

    eo[11] = sum % 2;
}
void Encoder::decodeSlice(uint16_t index, uint8_t* ep)
{
    int positions[4];

    int upper = 11;

    // Inverse of:
    // C(pos[0],1) + C(pos[1],2) + C(pos[2],3) + C(pos[3],4)

    for (int k = 4; k >= 1; --k)
    {
        int p = upper;

        while (p >= 0 && C(p, k) > index)
        {
            --p;
        }

        positions[k - 1] = p;
        index -= C(p, k);

        upper = p - 1;
    }

    // Fill the edge permutation.
    // 0..7  = non-slice edges
    // 8..11 = slice edges

    uint8_t nextNonSlice = 0;
    uint8_t nextSlice = 8;

    for (int pos = 0; pos < 12; ++pos)
    {
        bool isSlice = false;

        for (int k = 0; k < 4; ++k)
        {
            if (positions[k] == pos)
            {
                isSlice = true;
                break;
            }
        }

        if (isSlice)
        {
            ep[pos] = nextSlice++;
        }
        else
        {
            ep[pos] = nextNonSlice++;
        }
    }
}
namespace
{
    constexpr int factorial(int n)
    {
        return n <= 1 ? 1 : n * factorial(n - 1);
    }

    // Inverse of the Lehmer-code encoding used by lehmer8coder/lehmer4:
    // index = d0*(N-1)! + d1*(N-2)! + ... + d(N-2)*1!
    // where each d_i is the rank of out[i] among the values not yet used.
    //
    // Memory safety: `remaining` is a fixed-size std::array<uint8_t,N> with
    // N known at compile time. remCount only ever shrinks from N to 1, and
    // every index used to read/write it (`d`, and the shift loop's `j`,
    // `j+1`) is bounded by remCount <= N, so no access can leave the array.
    template <int N>
    void decodePermutation(uint16_t index, uint8_t* out)
    {
        static_assert(N >= 2 && N <= 8, "decodePermutation supports N in [2,8]");

        std::array<uint8_t, N> remaining{};
        for (int i = 0; i < N; ++i)
            remaining[i] = static_cast<uint8_t>(i);

        int remCount = N;
        unsigned val = index;

        for (int i = 0; i < N - 1; ++i)
        {
            const int weight = factorial(N - 1 - i);
            int digit = static_cast<int>(val / weight);
            val %= weight;

            // Defensive clamp: a well-formed index never needs this, but it
            // guarantees remaining[d] can never be read out of bounds even
            // if a corrupted/truncated coordinate is passed in.
            if (digit >= remCount)
                digit = remCount - 1;

            out[i] = remaining[digit];

            for (int j = digit; j < remCount - 1; ++j)
                remaining[j] = remaining[j + 1];

            --remCount;
        }

        // One candidate left; it's forced.
        out[N - 1] = remaining[0];
    }
}

void Encoder::decodeLehmer8(uint16_t index, uint8_t* p)
{
    constexpr uint16_t kPermCount = 40320; // 8!
    if (p == nullptr)
        throw std::invalid_argument("Encoder::decodeLehmer8: p is null");

    if (index >= kPermCount)
        throw std::out_of_range(
            "Encoder::decodeLehmer8: index " + std::to_string(index) +
            " out of range [0, 40320)");

    decodePermutation<8>(index, p);
}

void Encoder::decodeLehmer4(uint16_t index, uint8_t* p)
{
    constexpr uint16_t kPermCount = 24; // 4!

    if (p == nullptr)
        throw std::invalid_argument("Encoder::decodeLehmer4: p is null");

    if (index >= kPermCount)
        throw std::out_of_range(
            "Encoder::decodeLehmer4: index " + std::to_string(index) +
            " out of range [0, 24)");

    decodePermutation<4>(index, p);
}