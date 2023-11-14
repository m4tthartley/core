
#define CORE_HEADERS_ONLY
#include <core/core.h>

#include <core/math.h>

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

#if 1
void defrag() {
    char buffer[0x800];
    core_allocator_t arena = core_allocator(buffer, sizeof(buffer));
    core_use_allocator(&arena);

    core_print(TERM_RESET);

    FOR (i, 1000) {
        char* str = celestial_objects[core_rand(0, array_size(celestial_objects))];
        char* object = core_str(str);
        if (object) {
            core_print(object);
            // core_print_arena(&arena);
            if (core_rand(0, 1000) != 0) {
                core_strfree(object);
            }
        }
    }

    // char* str = s_create("This is a program that randomly selects a celestial body and prints out it's name.");
    // core_print(str);

    core_print_arena(&arena);
}

void strings() {
    char* string = "Hello, my name is Matt";
    char* parts[10];
    int num = core_strsplit(parts, 10, string, " ");
    FOR (i, num) {
        core_print(parts[i]);
    }

    char* padded = "     Hello World   \n";
    core_print(padded);
    core_strtrim(&padded);
    core_print(padded);

    char* sub = "Whatever is the matter, Bilbo?";
    core_print(core_substr(sub, 16, 6));

    char* insert = "Hello World";
    core_strinsert(&insert, 5, " Stupid");
    core_print(insert);

    int x = 0;
}

void dynarrs() {
    u8 buffer[PAGE_SIZE];
    core_dynarr_t arr = core_dynarr_static(buffer, sizeof(buffer), sizeof(int));

    FOR (i, 100) {
        int num = core_rand(0, 100);
        core_dynarr_push(&arr, &num);
    }

    FOR (i, arr.count) {
        int* num = core_dynarr_get(&arr, i);
        core_print("%i", *num);
    }

    core_print("clear");
    core_dynarr_clear(&arr);

    FOR (i, 10) {
        int num = core_rand(0, 100);
        core_dynarr_push(&arr, &num);
    }

    FOR (i, arr.count) {
        int* num = core_dynarr_get(&arr, i);
        core_print("%i", *num);
    }

    core_print("pop");
    core_dynarr_pop(&arr, 0);
    core_dynarr_pop(&arr, 1);

    FOR (i, arr.count) {
        int* num = core_dynarr_get(&arr, i);
        core_print("%i", *num);
    }
}

void pool() {
    struct {
        b32 used;
        int value;
    } numbers[64] = {0};

    typeof(numbers[0]) a = {0, core_rand(1, 100)};
    core_pool_add(numbers, a);
    typeof(numbers[0]) b = {0, core_rand(1, 100)};
    core_pool_add(numbers, b);
    typeof(numbers[0]) c = {0, core_rand(1, 100)};
    core_pool_add(numbers, c);

    core_pool_for(numbers, {
        core_print("%i", numbers[i].value);
    })
}

int main() {

    u8 buffer[PAGE_SIZE];
    core_allocator_t arena = core_allocator(buffer, sizeof(buffer));
    core_use_allocator(&arena);

    defrag();
    
}
#endif