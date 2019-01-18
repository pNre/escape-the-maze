#include "types.h"
#include "utils.h"

#include "main/fs.h"

ALLEGRO_PATH * fs_get_standard_path(void) {
    
    static ALLEGRO_PATH * standard_path = NULL;
    
    if (!standard_path) {
        standard_path = al_get_standard_path(ALLEGRO_RESOURCES_PATH);
        debugf("[FS] standard path: %s\n", fs_get_resources_path());
    }
    
    return standard_path;
    
}

void fs_destroy(void) {
    
    al_destroy_path(fs_get_standard_path());
    
}

const char * fs_get_resources_path(void) {

    //  path della cartella delle risorse
    return al_path_cstr(fs_get_standard_path(), FS_PATH_SEPARATOR);
    
}

char * _fs_construct_path(const char * first, ...)
{
    
    //  path risultante
    char * result_path = NULL;
    
    va_list args;
    va_start(args, first);
    
    //  dimensione da allocare per contenere tutta la stringa
    //  1 = \0
    size_t toalloc = strlen(first) + 1;
    
    //  parametro
    char * component = NULL;
    
    //  la lista degli argomenti va terminata da un NULL
    while ((component = va_arg(args, char *)) != NULL) {
        
        //  se si sta connettendo l'estensione non serve il +1
        if (component[0] == '.')
            toalloc += strlen(component);
        else //  spazio per il separatore / + la componente
            toalloc += strlen(component) + 1;
    }
    
    //  allocazione spazio calcolato
    result_path = memalloc(char, toalloc);
    
    //  reset
    va_start(args, first);
    
    strcpy(result_path, first);
    
    //  fase di copia
    while ((component = va_arg(args, char *)) != NULL) {
        
        //  esensione del file
        if (component[0] != '.' && result_path[strlen(result_path) - 1] != FS_PATH_SEPARATOR) {
            size_t off = strlen(result_path);
            result_path[off] = FS_PATH_SEPARATOR;
            result_path[off + 1] = 0;
        }

        strcat(result_path, component);
        
    }
    
    va_end(args);
    
    return result_path;
    
}
