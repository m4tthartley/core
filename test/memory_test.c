
#include <core/core.h>

char* celestial_objects[] = {
    "Andromeda Galaxy",
    "Pleiades Star Cluster",
    "Orion Nebula",
    "Horsehead Nebula",
    "Sombrero Galaxy",
    "Crab Nebula",
    "Whirlpool Galaxy",
    "Boomerang Nebula",
    "Helix Nebula",
    "Eagle Nebula",
    "Triangulum Galaxy",
    "Centaurus A",
    "Lagoon Nebula",
    "Antennae Galaxies",
    "Cat's Eye Nebula",
    "M81 Galaxy",
    "Cigar Galaxy",
    "Veil Nebula",
    "Omega Centauri",
    "Ring Nebula",
    "Messier 104",
    "Tarantula Nebula",
    "Magellanic Clouds",
    "NGC 602",
    "Ghost Head Nebula",
    "Butterfly Nebula",
    "Witch Head Nebula",
    "Black Eye Galaxy",
    "Rosette Nebula",
    "Helmi Stream",
    "Carina Nebula",
    "Jewel Box Cluster",
    "NGC 2818",
    "Sculptor Galaxy",
    "Eta Carinae",
    "Ghost of Jupiter Nebula",
    "IC 2118",
    "Bug Nebula",
    "Leo Triplet",
    "NGC 6334",
    "Hubble's Variable Nebula",
    "NGC 7293",
    "Veiled Ghost Nebula",
    "NGC 6888",
    "Dumbbell Nebula",
    "NGC 2237",
    "NGC 3576",
    "Blue Flash Nebula",
    "NGC 6302",
    "NGC 5907",
    "NGC 4565",
    "Cassiopeia A",
    "Eskimo Nebula",
    "NGC 281",
    "NGC 6744",
    "NGC 7000",
    "IC 405",
    // "NGC 2392",
    // "NGC 1365",
    // "NGC 253",
    // "NGC 6357",
    // "NGC 7380",
    // "NGC 6992",
    // "NGC 1499",
    // "NGC 4945",
    // "NGC 3628",
    // "NGC 6960",
    // "NGC 2403",
    // "NGC 2238",
    // "NGC 5194",
    // "NGC 6334",
    // "NGC 1499",
    // "NGC 2239",
    // "NGC 7380",
    // "NGC 6826",
    // "NGC 7635",
    // "NGC 6543",
    // "NGC 7000",
    // "NGC 1977",
    // "NGC 6888",
    // "NGC 2477",
    // "NGC 2363",
    // "NGC 2392",
    // "NGC 2818",
    // "NGC 2244",
    // "NGC 6888",
};

int main() {
    char buffer[0x800];
    m_arena arena;
    m_freelist(&arena, buffer, sizeof(buffer));
    s_pool(&arena);

    core_print(TERM_RESET);

    FOR (i, 10000) {
        char* str = celestial_objects[r_int_range(0, array_size(celestial_objects))];
        char* object = s_create(str);
        if (object) {
            core_print(object);
            // core_print_arena(&arena);
            if (r_int_range(0, 1000) != 0) {
                s_free(object);
            }
        }
    }

    // char* str = s_create("This is a program that randomly selects a celestial body and prints out it's name.");
    // core_print(str);

    core_print_arena(&arena);
    int x = 0;
}