#include <cstdint>
#include <cstddef> // Inclure l'en-tête pour avoir accès à size_t


#ifndef ICONS_
#define ICONS_

struct icons {
    static const uint8_t i_save[];
    static const size_t i_save_size;

    static const uint8_t i_add[];
    static const size_t i_add_size;

    static const uint8_t i_folder[];
    static const size_t i_folder_size;

    static const uint8_t i_settings[];
    static const size_t i_settings_size;
    
    static const uint8_t i_file[];
    static const size_t i_file_size;

    static const uint8_t i_FolderIcon[];
    static const size_t i_FolderIcon_size;
};


#endif // ICONS_