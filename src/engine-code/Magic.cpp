#include "Magic.h"

const auto EMPTY_BITMAP = 0ull;
const auto ONE_SHIFT = 1ull;

//FUNCTION set_occupancy(int index, int bits_in_mask, bitboard attack_mask)
//Turns attack mask into a counter with binary value of index represented by
//occupancy bits. This maps an index value to the occupancy of the relevant attack mask.
/*
    I.E. mask of

    0 0 0 0
    1 0 0 0
    1 0 0 0
    0 1 1 0  THIS IS SIMPLIFIES TO 4 SQUARES

    can count to an index of 15 as it has 4 active bits.
    Thus an index of 1 is
    0 0 0 0
    1 0 0 0
    0 0 0 0
    0 0 0 0
    and an index of 2 is
    0 0 0 0
    0 0 0 0
    1 0 0 0
    0 0 0 0
    and an index of 3 is
    0 0 0 0
    1 0 0 0
    1 0 0 0
    0 0 0 0
    and an index of 15 is
    0 0 0 0
    1 0 0 0
    1 0 0 0
    0 1 1 0
*/
bitboard Magic::set_occupancy(int index, int bits_in_mask, bitboard attack_mask)
{
    auto occupancy = EMPTY_BITMAP;
    for (auto iCount = 0; iCount < bits_in_mask; iCount++)
    {
        auto square = Bitboard::get_lsb_index(attack_mask);
        Bitboard::pop_bit(attack_mask,square);
        if (index & (ONE_SHIFT << iCount))
            occupancy |= (ONE_SHIFT << square);
    }

    return occupancy;
}

//This is the total number of possible occupancies for rook or bishop
//Rook has 4096 occupancies and bishop has 512 occupancies
const auto MAX_OCCUPANCIES = 4096;
const auto MAGIC_GENERATION_FAILURE = 100000000000ull;
const auto MINIMUM_BITS_FOR_MAGIC = 6;
const auto FINAL_8_BITS = 0xFF00000000000000;
const auto ZERO = 0;
const auto TOTAL_BITS = 64;
const auto TOTAL_SQUARES = 64;

//Note piece_type can only be rook or bishop
bitboard Magic::find_magics(int square, int occupancy_bit_count, int piece_type)
{
    //Initialize tables to check magic correctness
    bitboard occupancy_table[MAX_OCCUPANCIES] = {};
    bitboard attack_table[MAX_OCCUPANCIES] = {};
    bitboard used_attack_table[MAX_OCCUPANCIES] = {};
    //Generate relevant attack masks
    auto attack_mask = bitboard{};
    if (piece_type == bishop)
        attack_mask = BishopAttacks::mask_bishop_occupancies(square);
    else if (piece_type == rook)
        attack_mask = RookAttacks::mask_rook_occupancies(square);
    else
        throw MagicGenerationError{};
    int occupancy_indices = ONE_SHIFT << occupancy_bit_count;
    //Loop over index
    for (auto index = 0; index < occupancy_indices; index++)
    {
        //Load occupancy mappings to occupancy table
        occupancy_table[index] = set_occupancy(index, occupancy_bit_count, attack_mask);
        //Load attack tables with attack masks with blockers from occupancy table
        if (piece_type == bishop)
            attack_table[index] = BishopAttacks::gen_bishop_attacks_on_the_fly(square, occupancy_table[index]);
        else if (piece_type == rook)
            attack_table[index] = RookAttacks::gen_rook_attacks_on_the_fly(square, occupancy_table[index]);
        else
            throw MagicGenerationError{};
    }
    //Loop until a usable magic is found, if none is found then terminate the program
    auto magic_counter = EMPTY_BITMAP;
    while (magic_counter < MAGIC_GENERATION_FAILURE)
    {
        magic_counter++;
        //Generate magic number candidate
        bitboard magic_number = PsuedoRandom::generate_magic_candidate();
        //Skip unsuitable magic numbers
        if (Bitboard::count_bits((attack_mask * magic_number) & FINAL_8_BITS) < MINIMUM_BITS_FOR_MAGIC) continue;
        //Load used attacks
        memset(used_attack_table, EMPTY_BITMAP, sizeof(used_attack_table));
        auto index = ZERO;
        auto fail = false;
        for (index = ZERO, fail = false; !fail && index < occupancy_indices; index++)
        {
            //Generate magic index using formula, available at Chessprogrammingwiki
            auto magic_index = static_cast<int>((occupancy_table[index] * magic_number) >> (TOTAL_BITS - occupancy_bit_count));
            //Test to see if valid magic index
            if (used_attack_table[magic_index] == EMPTY_BITMAP)
                used_attack_table[magic_index] = attack_table[index];
            else if (used_attack_table[magic_index] != attack_table[index])
                fail = true;
        }
        //Magic number working
        if (!fail)
            return magic_number;
    }
    if (magic_counter >= MAGIC_GENERATION_FAILURE)
    {
        throw MagicGenerationError{};
    }
    return bitboard{};
}

void Magic::init_magic_numbers()
{
    for (auto square = 0; square < TOTAL_SQUARES; square++)
    {
        rook_magic_numbers[square] = find_magics(square,RookAttacks::rook_occupancy_bit_count[square], rook);
    }
    for (auto square = 0; square < TOTAL_SQUARES; square++)
    {
        bishop_magic_numbers[square] = find_magics(square,BishopAttacks::bishop_occupancy_bit_count[square], bishop);
    }
}


//PSUEDO RANDOM FUNCTIONS AND CONSTANTS

const auto SHIFT_13 = 13;
const auto SHIFT_17 = 17;
const auto SHIFT_5 = 5;
const auto SHIFT_16 = 16;
const auto SHIFT_32 = 32;
const auto SHIFT_48 = 48;
const auto FIRST_16_BITS = 0xFFFF;

//This 1804289383 is used to ensure specific magics are generated, but it can be changed.
//This generates the magics given in Magic.h
unsigned int PsuedoRandom::state_{1804289383};

void PsuedoRandom::set_seed(unsigned int state)
{
    state_ = state;
}

unsigned int PsuedoRandom::get_random_number_32Bit()
{
    auto number = state_;
    number ^= number << SHIFT_13;
    number ^= number >> SHIFT_17;
    number ^= number << SHIFT_5;
    PsuedoRandom::set_seed(number);
    return number;
}

bitboard PsuedoRandom::get_random_number_64Bit()
{
    auto randNum1 = bitboard{get_random_number_32Bit() & FIRST_16_BITS};
    auto randNum2 = bitboard{get_random_number_32Bit() & FIRST_16_BITS};
    auto randNum3 = bitboard{get_random_number_32Bit() & FIRST_16_BITS};
    auto randNum4 = bitboard{get_random_number_32Bit() & FIRST_16_BITS};
    return randNum1 | (randNum2 << SHIFT_16) | (randNum3 << SHIFT_32) | (randNum4 << SHIFT_48);
}

bitboard PsuedoRandom::generate_magic_candidate()
{
    return get_random_number_64Bit() & get_random_number_64Bit() & get_random_number_64Bit();
}

